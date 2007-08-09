/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "lineextractor.h"
#include <orcaice/orcaice.h>

#include <iostream>
#include <assert.h>
#include <cmath>

#include <orca/featuremap2d.h>

#define REJECT_GROUND_OBSERVATIONS 1  // 0  1

#define MIN_DELTA_RANGE_FOR_LINE_ENDPOINT 0.2

using namespace std;

namespace laserfeatures {

LineExtractor::LineExtractor( const orcaice::Context &context,
                              double laserMaxRange,
                              bool extractLines,
                              bool extractCorners,
                              bool extractLineEndpoints )
    : laserMaxRange_( laserMaxRange ),
      extractLines_(extractLines),
      extractCorners_(extractCorners),
      extractLineEndpoints_(extractLineEndpoints),
      context_(context)
{
    assert( laserMaxRange_ > 0.0 );
    assert( extractLines_ || extractCorners_ || extractLineEndpoints_ );

    std::string prefix = context.tag() + ".Config.Lines.";
    Ice::PropertiesPtr prop = context.properties();

    clusterMaxRangeDelta_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"ClusterMaxRangeDelta", 0.5 );
    breakDistThreshold_   = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"BreakDistThreshold", 0.2 );
    minPointsInLine_      = orcaice::getPropertyAsIntWithDefault( prop, prefix+"MinPointsInLine", 6 );
    minLineLength_        = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MinLineLength", 1.0 );
    rejectLikelyGroundObservations_ = orcaice::getPropertyAsIntWithDefault( prop, prefix+"RejectLikelyGroundObservations", true );
    linePFalsePositive_   = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"PFalsePositive", 0.4 );
    linePFalsePositivePossibleGround_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"PFalsePositivePossibleGround", 0.55 );
    linePTruePositive_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"PTruePositive", 0.6 );

    lineEndpointPFalsePositive_   = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"PFalsePositive", 0.2 );
    lineEndpointPTruePositive_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"PTruePositive", 0.6 );

    rangeSd_        = orcaice::getPropertyAsDoubleWithDefault( prop, "Config.RangeSd", 0.2 );
    bearingSd_      = (M_PI/180.0)*orcaice::getPropertyAsDoubleWithDefault( prop, "Config.BearingSd", 5.0 );

    cout<<"TRACE(lineextractor.cpp): Line Extractor Config:" << endl;
    cout<<"TRACE(lineextractor.cpp):   ClusterMaxRangeDelta : "<<clusterMaxRangeDelta_ << endl;
    cout<<"TRACE(lineextractor.cpp):   BreakDistThreshold   : "<<breakDistThreshold_ << endl;
    cout<<"TRACE(lineextractor.cpp):   MinPointsInLine      : "<<minPointsInLine_ << endl;
    cout<<"TRACE(lineextractor.cpp):   MinLineLength        : "<<minLineLength_ << endl;
    cout<<"TRACE(lineextractor.cpp):   ExtractLines         : "<<extractLines_ << endl;
    cout<<"TRACE(lineextractor.cpp):   ExtractCorners       : "<<extractCorners_ << endl;
    cout<<"TRACE(lineextractor.cpp):   ExtractLineEndpoints : "<<extractLineEndpoints_ << endl;
    cout<<"TRACE(lineextractor.cpp):   RejectLikelyGroundObservations: " << rejectLikelyGroundObservations_ << endl;
    cout<<"TRACE(lineextractor.cpp):   LinePFalsePositive               : " << linePFalsePositive_ << endl;
    cout<<"TRACE(lineextractor.cpp):   LinePFalsePositivePossibleGround : " << linePFalsePositivePossibleGround_ << endl;
    cout<<"TRACE(lineextractor.cpp):   LinePTruePositive                : " << linePTruePositive_ << endl;
    cout<<"TRACE(lineextractor.cpp):   LineEndpointPFalsePositive       : " << lineEndpointPFalsePositive_ << endl;
    cout<<"TRACE(lineextractor.cpp):   LineEndpointPTruePositive        : " << lineEndpointPTruePositive_ << endl;
}
    
void LineExtractor::addFeatures( const orca::LaserScanner2dDataPtr &laserData,
                                   orca::PolarFeature2dDataPtr &features )
{
//     cout<<"TRACE(lineextractor.cpp): ------------------- addFeatures() --------------------" << endl;

    double angleIncrement = orcaice::calcAngleIncrement( laserData->fieldOfView, 
                                                         laserData->ranges.size() );
    
    std::vector<Section> sections;

    // Use the laser data to generate a set of sections
    extractSections(laserData->ranges,
                    laserData->startAngle,
                    angleIncrement,
                    laserMaxRange_,
                    minPointsInLine_,
                    clusterMaxRangeDelta_,
                    sections);

#ifndef NDEBUG
    // Check that everything makes sense.
    for ( unsigned int i=0; i < sections.size(); i++ )
    {
        if ( sections[i].isALine() )
        {
            assert( sections[i].start().range() > 0 && 
                    sections[i].start().range() < laserData->maxRange );
            assert( sections[i].start().bearing() > laserData->startAngle &&
                    sections[i].start().bearing() < (laserData->startAngle+laserData->fieldOfView) );
            assert( sections[i].end().range() > 0 && 
                    sections[i].end().range() < laserData->maxRange );
            assert( sections[i].end().bearing() > laserData->startAngle &&
                    sections[i].end().bearing() < (laserData->startAngle+laserData->fieldOfView) );
        }
    }
#endif

    // Fit lines to the sections
    breakAndFitLines( sections, minPointsInLine_, breakDistThreshold_ );

    if ( extractCorners_ )
    {
        // Find corner features, add them to the list of features
        addCorners( sections, features );
    }

    if ( extractLines_ )
    {
        // Find line features, add them to the list of features
        addLines( sections, features );
    }

    if ( extractLineEndpoints_ )
    {
        addLineEndpoints( sections, features );
    }
}

namespace {
        const double ANGLE_THRESHOLD = 60*M_PI/180.0;
}

bool
isEndpointVisible( double endRange,
                   double rangeBeyondEnd,
                   bool   meetsLine,
                   double meetAngle,
                   bool   isInternalAngle,
                   double angleToEnd,
                   LineExtractor::VisibilityReason *reason )                   
{
    if ( meetsLine )
    {
        // cout<<"TRACE(lineextractor.cpp): junction of two lines" << endl;
        if ( fabs(meetAngle) > 45.0*M_PI/180.0 )
        {
            if ( reason != NULL )
            {
                if ( isInternalAngle )
                    *reason = LineExtractor::InternalLineJunction;                    
                else
                    *reason = LineExtractor::ExternalLineJunction;
            }
            return true;
        }
    }

    if ( angleToEnd > ANGLE_THRESHOLD )
    {
        // cout<<"TRACE(lineextractor.cpp): can't see around corner" << endl;
        return false;
    }

    if ( rangeBeyondEnd < 0 )
    {
        // cout<<"TRACE(lineextractor.cpp): ends with invalid range" << endl;
        return false;
    }

    if ( rangeBeyondEnd < ( endRange + MIN_DELTA_RANGE_FOR_LINE_ENDPOINT ) )
    {
        // cout<<"TRACE(lineextractor.cpp): obscured by fg obstacle" << endl;
        return false;
    }

//     cout<<"TRACE(lineextractor.cpp): isStartVisible() returning true." << endl;
//     cout<<"TRACE(lineextractor.cpp): rangeBeyondEnd: " << rangeBeyondEnd << endl;
    if ( reason != NULL )
        *reason = LineExtractor::Unknown;
    return true;
}

bool
LineExtractor::isStartVisible( const Section &section,
                               double alpha,
                               const Section *prevSection,
                               VisibilityReason *reason,
                               double minLineLength )
{
    // cout<<"TRACE(lineextractor.cpp): --------- isStartVisible() ---------" << endl;
    if ( section.lineLength() < minLineLength )
        return false;

    // Check for section at edge of scan
    if ( section.rangeBeforeStart() < 0 )
        return false;

    bool meetsLine = false;
    double meetAngle = -999;
    bool isInternalAngle = false;

    // Junction of two lines
    if ( prevSection != NULL &&
         prevSection->isALine() &&
         prevSection->isNextCon() )
    {
        meetsLine = true;
        // cout<<"TRACE(lineextractor.cpp): junction of two lines" << endl;
        // Find the angle between them
        double d = prevSection->eigVectX()*prevSection->eigVectX()+prevSection->eigVectY()*prevSection->eigVectY();
        double px = -prevSection->eigVectX()*prevSection->c() / d;
        double py = -prevSection->eigVectY()*prevSection->c() / d;
        double prevAlpha = atan2( py, px );
        meetAngle = alpha-prevAlpha;
        isInternalAngle = meetAngle > 0;

        NORMALISE_ANGLE(meetAngle);
    }

    // Is it a free-standing line that we can see the end of?
    // Find the angle (external to the endpoint) 
    // between the infinite line and the endpoint
    double angleToStart = M_PI/2.0 - (section.start().bearing() - alpha);

    return isEndpointVisible( section.elements().front().range(),
                              section.rangeBeforeStart(),
                              meetsLine,
                              meetAngle,
                              isInternalAngle,
                              angleToStart,
                              reason );
}

bool 
LineExtractor::isEndVisible( const Section &section,
                             double alpha,
                             const Section *nextSection,
                             VisibilityReason *reason,
                             double minLineLength )
{
    // cout<<"TRACE(lineextractor.cpp): ----------- isEndVisible() ------------" << endl;

    assert ( section.isALine() );
    if ( section.lineLength() < minLineLength )
        return false;

    // Check for section at edge of scan
    if ( section.rangeAfterEnd() < 0 )
        return false;

    bool meetsLine = false;
    double meetAngle = -999;
    bool isInternalAngle = true;

    // Junction of two lines
    if ( nextSection != NULL &&
         nextSection->isALine() &&
         section.isNextCon() )
    {
        meetsLine = true;
        // cout<<"TRACE(lineextractor.cpp): junction of two lines" << endl;
        // Find the angle between them
        double d = nextSection->eigVectX()*nextSection->eigVectX()+nextSection->eigVectY()*nextSection->eigVectY();
        double px = -nextSection->eigVectX()*nextSection->c() / d;
        double py = -nextSection->eigVectY()*nextSection->c() / d;
        double nextAlpha = atan2( py, px );
        meetAngle = alpha-nextAlpha;
        isInternalAngle = meetAngle < 0;
        NORMALISE_ANGLE(meetAngle);
    }

    // Is it a free-standing line that we can see the end of?
    // Find the angle (external to the endpoint) 
    // between the infinite line and the endpoint
    double angleToEnd = M_PI/2.0 + (section.end().bearing() - alpha);
    
    return isEndpointVisible( section.elements().back().range(),
                              section.rangeAfterEnd(),
                              meetsLine,
                              meetAngle,
                              isInternalAngle,
                              angleToEnd,
                              reason );
}
             
bool lineMightBeGround( const Section &section )
{
    // Look for lines with near-horizontal slope
    return fabs(section.eigVectY()) < 0.1;
}

void
LineExtractor::determineUncertainty( double &rhoSd, double &alphaSd, const Section &s )
{
    // Two sources of uncertainty: (1) sensor, and (2) line fitting.

    // (1) Sensor uncertainty.
    // If individual range returns were independent, range errors would induce 
    // alpha errors.  But they're not, they're correlated.  
    // Assume they're perfectly correlated.
    double sensorRhoSd   = rangeSd_;
    double sensorAlphaSd = bearingSd_;

    // (2) Line Fit Uncertainty
    // Errors in line fitting can:
    // (a) push the line back and forth,
    double fitRhoSd = s.maxLineFitError();
    // (b) rotate the line.  This effect is reduced for longer lines
    //     Assume the worst: the line could be off by maxLineFitError on the ends.
    double fitAlphaSd = 2 * atan( s.maxLineFitError() / s.lineLength() );

    // Add them up
    rhoSd   = sensorRhoSd   + fitRhoSd;
    alphaSd = sensorAlphaSd + fitAlphaSd;
}

void
LineExtractor::addLines( const std::vector<Section> &sections, 
                         orca::PolarFeature2dDataPtr &features )
{
    std::vector<Section>::const_iterator i=sections.begin();
    std::vector<Section>::const_iterator prev = sections.begin();
    for ( i = sections.begin(), prev=sections.begin();
          i != sections.end(); 
          prev=i, ++i )
    {
        if ( !i->isALine() ||
             i->lineLength() < minLineLength_ )
            continue;

        double pFalsePositive = linePFalsePositive_;
        if ( REJECT_GROUND_OBSERVATIONS )
        {
            // Look for lines with near-horizontal slope
            if ( rejectLikelyGroundObservations_ &&
                 fabs( i->eigVectY() ) < 0.1 )
            {
                context_.tracer()->debug( "Rejecting likely ground observation", 3 );
                continue;
            }
            if ( fabs( i->eigVectY() ) < 0.25 )
            {
                // cout<<"TRACE(lineextractor.cpp): lowering prob" << endl;
                pFalsePositive = linePFalsePositivePossibleGround_;
            }
        }

        if ( (*i).end().bearing() < (*i).start().bearing() )
        {
            std::stringstream ss;
            ss << "LineExtractor: strange... endBearing < startBearing.  Endpoints are: " << (*i).start().toStringRB() << " and " << (*i).end().toStringRB();
            context_.tracer()->debug( ss.str() );
            continue;
        }

        orca::LinePolarFeature2dPtr newFeature = new orca::LinePolarFeature2d;
        newFeature->type = orca::feature::LINE;

        newFeature->start.r = (*i).start().range();
        newFeature->start.o = (*i).start().bearing();
        newFeature->end.r   = (*i).end().range();
        newFeature->end.o   = (*i).end().bearing();

        determineUncertainty( newFeature->rhoSd, newFeature->alphaSd, *i );

        newFeature->pFalsePositive = pFalsePositive;
        newFeature->pTruePositive  = linePTruePositive_;

        //
        // Determine visibility of endpoints.
        //
        
        // Find the meeting between the (infinite) line and 
        // its perpendicular bisector from the origin.
        // Call this point (px,py).
        double d = i->eigVectX()*i->eigVectX()+i->eigVectY()*i->eigVectY();
        double px = -i->eigVectX()*i->c() / d;
        double py = -i->eigVectY()*i->c() / d;


        // Convert to rho-alpha
        // double rho   = hypotf( py, px );
        double alpha = atan2( py, px );

        const Section *prevPtr=NULL;
        if ( i != sections.begin() )
            prevPtr = &(*prev);
        newFeature->startSighted = isStartVisible( *i, alpha, prevPtr );
        const Section *nextPtr=NULL;
        if ( i+1 != sections.end() )
            nextPtr = &(*(i+1));
        newFeature->endSighted   = isEndVisible( *i, alpha, nextPtr );

        // Add it to the list
        features->features.push_back( newFeature );
    }
}

void
LineExtractor::addCorners( const std::vector<Section>  &sections, 
                           orca::PolarFeature2dDataPtr &features )
{
    const double P_FALSE_POSITIVE = 0.3;
    const double P_FALSE_POSITIVE_POSSIBLE_GROUND = 0.5;
    const double P_TRUE_POSITIVE  = 0.6;
    const double CORNER_BOUND = 0.2;

    std::vector<Section>::const_iterator itr;
    std::vector<Section>::const_iterator next;
  
    for (itr = sections.begin(), next = sections.begin() + 1; itr != sections.end() && next != sections.end(); itr++, next++)
    {
        // check that we have two connected lines before computing the angle between them
        // We also want to avoid extracting corners at the junction between observations of the
        // ground and a wall.
        if (itr->isALine() && itr->isNextCon() && next->isALine()) 
        {
            if ( itr->lineLength() < minLineLength_ )
            {
                //cout<<"TRACE(lineextractor.cpp): line too small: length is " << itr->lineLength() << endl;
                continue;
            }
            if ( next->lineLength() < minLineLength_ )
            {
                //cout<<"TRACE(lineextractor.cpp): line too small: length is " << next->lineLength() << endl;
                continue;
            }

            double pFalsePositive = P_FALSE_POSITIVE;

            if ( REJECT_GROUND_OBSERVATIONS && 
                 (lineMightBeGround( *itr ) || lineMightBeGround( *next )) )
            {
              //std::cout << "We have a line with a near horizontal slope.  Could be the ground??? Slope A : " << itr->eigVectY << " Slope B : " << next->eigVectY << std::endl;
              pFalsePositive = P_FALSE_POSITIVE_POSSIBLE_GROUND;
            }
  
            // We have a corner
            double A1 = itr->eigVectX();
            double B1 = itr->eigVectY();
            double C1 = itr->c();
  
            double A2 = next->eigVectX();
            double B2 = next->eigVectY();
            double C2 = next->c();
  
            double dot_prod = A1*A2 + B1*B2;
              
            // the dot product of the two unit vectors will be 0 if the lines are perpendicular
            if (fabs(dot_prod) < CORNER_BOUND) {
                double cornerX;
                double cornerY;
  
                // use Kramer's rule to find the intersection point of the two lines
                // The equations for the lines are:
                // A1 x + B1 y + C1 = 0 and
                // A2 x + B2 y + C2 = 0
                //
                // We need to solve for the (x, y) that satisfies both equations
                //
                // x = | C1 B1 |  / | A1 B1 | 
                //     | C2 B2 | /  | A2 B2 |
                //
                // y = | A1 C1 |  / | A1 B1 | 
                //     | A2 C2 | /  | A2 B2 |
  
                double den = (B1*A2 - B2*A1);
                  
                // this check isn't really necessary as we have already established that the lines
                // are close to perpendicular.  The determinant will only be zero if the lines
                // are parallel
                if (den != 0)
                {
                    cornerX = (B2*C1 - B1*C2)/den;
                    cornerY = (A1*C2 - A2*C1)/den;
    
                    double range = sqrt(pow(cornerX,2) + pow(cornerY,2));
                    double bearing = acos(cornerX/range);
                    if (cornerY < 0) {
                        bearing = -bearing;
                    }
    
                    orca::PointPolarFeature2dPtr pp = new orca::PointPolarFeature2d;
                    pp->type = orca::feature::CORNER;
                    pp->p.r = range;
                    pp->p.o = bearing;
                    pp->rangeSd   = rangeSd_;
                    pp->bearingSd = bearingSd_;
                    pp->pFalsePositive = pFalsePositive;
                    pp->pTruePositive  = P_TRUE_POSITIVE;
                    features->features.push_back( pp );
                }
            }
        }
    }
}

void
LineExtractor::addLineEndpoints( const std::vector<Section>  &sections, 
                                 orca::PolarFeature2dDataPtr &features )
{
    cout<<"TRACE(lineextractor.cpp): addLineEndpoints(): features.features.size() == " << features->features.size() << endl;

    const double MIN_LINE_LENGTH = 0.5;

    std::vector<Section>::const_iterator i=sections.begin();
    std::vector<Section>::const_iterator prev = sections.begin();
    VisibilityReason reason;
    for ( i = sections.begin(), prev=sections.begin();
          i != sections.end(); 
          prev=i, ++i )
    {
        if ( !i->isALine() )
            continue;

//         double pFalsePositive = linePFalsePositive_;
//         if ( REJECT_GROUND_OBSERVATIONS )
//         {
//             // Look for lines with near-horizontal slope
//             if ( rejectLikelyGroundObservations_ &&
//                  fabs( i->eigVectY() ) < 0.1 )
//             {
//                 context_.tracer()->debug( "Rejecting likely ground observation", 3 );
//                 continue;
//             }
//             if ( fabs( i->eigVectY() ) < 0.25 )
//             {
//                 // cout<<"TRACE(lineextractor.cpp): lowering prob" << endl;
//                 pFalsePositive = linePFalsePositivePossibleGround_;
//             }
//         }

        if ( (*i).end().bearing() < (*i).start().bearing() )
            continue;

        //
        // Determine visibility of endpoints.
        //
        
        // Find the meeting between the (infinite) line and 
        // its perpendicular bisector from the origin.
        // Call this point (px,py).
        double d = i->eigVectX()*i->eigVectX()+i->eigVectY()*i->eigVectY();
        double px = -i->eigVectX()*i->c() / d;
        double py = -i->eigVectY()*i->c() / d;

        // Convert to rho-alpha
        // double rho   = hypotf( py, px );
        double alpha = atan2( py, px );

        const Section *prevPtr=NULL;
        if ( i != sections.begin() )
            prevPtr = &(*prev);
        if ( isStartVisible( *i, alpha, prevPtr, &reason, MIN_LINE_LENGTH ) )
        {
            if ( reason == InternalLineJunction || reason == ExternalLineJunction )
            {
                orca::PointPolarFeature2dPtr newFeature = new orca::PointPolarFeature2d;
                if ( reason == ExternalLineJunction )
                    newFeature->type = orca::feature::EXTERNALCORNER;
                else if ( reason == InternalLineJunction )
                    newFeature->type = orca::feature::INTERNALCORNER;
                else
                    assert( false );
                newFeature->p.r = i->start().range();
                newFeature->p.o = i->start().bearing();
                newFeature->rangeSd = rangeSd_;
                newFeature->bearingSd = bearingSd_;
                newFeature->pFalsePositive = lineEndpointPFalsePositive_;
                newFeature->pTruePositive  = lineEndpointPTruePositive_;
                features->features.push_back( newFeature );
                cout<<"TRACE(lineextractor.cpp): added start-point: " 
                    << "r="<<newFeature->p.r<<", b="<<newFeature->p.o*180/M_PI << endl;

                if ( i->isNextCon() ) 
                {
                    // don't double-count.
                    continue;
                }
            }
        }
        const Section *nextPtr=NULL;
        if ( i+1 != sections.end() )
            nextPtr = &(*(i+1));
        if ( isEndVisible( *i, alpha, nextPtr, &reason, MIN_LINE_LENGTH ) )
        {
            if ( reason == InternalLineJunction || reason == ExternalLineJunction )
            {
                orca::PointPolarFeature2dPtr newFeature = new orca::PointPolarFeature2d;
                if ( reason == ExternalLineJunction )
                    newFeature->type = orca::feature::EXTERNALCORNER;
                else if ( reason == InternalLineJunction )
                    newFeature->type = orca::feature::INTERNALCORNER;
                else
                    assert( false );
                newFeature->p.r = i->end().range();
                newFeature->p.o = i->end().bearing();
                newFeature->rangeSd = rangeSd_;
                newFeature->bearingSd = bearingSd_;
                newFeature->pFalsePositive = lineEndpointPFalsePositive_;
                newFeature->pTruePositive  = lineEndpointPTruePositive_;
                features->features.push_back( newFeature );
                cout<<"TRACE(lineextractor.cpp): added end-point: " 
                    << "r="<<newFeature->p.r<<", b="<<newFeature->p.o*180/M_PI << endl;
            }
        }
    }
    cout<<"TRACE(lineextractor.cpp): addLineEndpoints returning: features.features.size() == " << features->features.size() << endl;
}


// bool LineExtractor::extractPossibleCorners( const orca::PolarFeature2dDataPtr & featureDataPtr )
// {
// //  return false; // for now, we won't be looking for possible corners...

//     const double POSSIBLE_BOUND = 0.2;
//
//     if (sections.size() <= 1) 
//         return false;

//     //Section *itr = sec->next;
//     //Section *prev = sec;
//     std::vector<Section>::iterator prev;
//     std::vector<Section>::iterator itr;
  
//     for (prev = sections.begin(), itr = sections.begin() + 1; itr != sections.end() && prev != sections.end(); itr++, prev++) 
//     {
//         if ( prev->elements.size() > 0 && itr->elements.size() > 0)
//         {
//             //while (itr != NULL) {
//             // check the end of the previous line 
//             if (!prev->isNextCon && prev->isALine) {
//                 SectionEl pret = prev->elements.back();
//                 SectionEl iret = itr->elements.front();
//                 if (iret.range() > pret.range() + POSSIBLE_BOUND) {
//                     orca::PointPolarFeature2dPtr pp = new orca::PointPolarFeature2d;
//                     pp->type = orca::feature::POSSIBLECORNER;
//                     pp->p.r = pret.range();
//                     pp->p.o = pret.bearing();
//                     pp->pFalsePositive = P_FALSE_POSITIVE;
//                     pp->pTruePositive  = P_TRUE_POSITIVE;
//                     featureDataPtr->features.push_back( pp );
//                 }
//             }
  
//             // Check the start of the line for possible corner...
//             if (itr->isALine && !prev->isNextCon) {
//                 SectionEl pret = prev->elements.back();
//                 SectionEl iret = itr->elements.front();
//                 if (pret.range() > iret.range() + POSSIBLE_BOUND) {
//                     orca::PointPolarFeature2dPtr pp = new orca::PointPolarFeature2d;
//                     pp->type = orca::feature::POSSIBLECORNER;
//                     pp->p.r = iret.range();
//                     pp->p.o = iret.bearing();
//                     pp->pFalsePositive = P_FALSE_POSITIVE;
//                     pp->pTruePositive  = P_TRUE_POSITIVE;
//                     featureDataPtr->features.push_back( pp );
//                 }
//             }
//         }
//     }
//     return true;
// }

}
