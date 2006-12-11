/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "cornerextractor.h"
#include <orcaice/orcaice.h>

#include <iostream>
#include <assert.h>
#include <cmath>

#include <orca/featuremap2d.h>

#define RANGE_DELTA     0.5    // 0.10    0.2
#define MIN_POINTS_IN_LINE   6    // 6      6
#define MAX_POINTS_IN_LINE   20   // 360    25
#define CORNER_BOUND     0.2    // 0.45    0.45
#define POSSIBLE_BOUND    0.2    // ???    0.2
#define REJECT_GROUND_OBSERVATIONS 1  // 0  1


using namespace std;

namespace laserfeatures {

namespace {
    const double P_FALSE_POSITIVE = 0.3;
    const double P_FALSE_POSITIVE_POSSIBLE_GROUND = 0.5;
    const double P_TRUE_POSITIVE  = 0.6;
}

CornerExtractor::CornerExtractor( orcaice::Context context, double laserMaxRange )
    : laserMaxRange_( laserMaxRange )
{
    std::string prefix = context.tag() + ".Config.Reflectors.";
    Ice::PropertiesPtr prop = context.properties();

    minLineLength_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MinLineLength", 1.0 );
}
    
void CornerExtractor::addFeatures( const orca::LaserScanner2dDataPtr &laserData,
                                   orca::PolarFeature2dDataPtr &features )
{  
    assert( laserMaxRange_ > 0.0 );
    double angleIncrement = laserData->fieldOfView / (double)(laserData->ranges.size()+1);
    
    std::vector<Section> sections;

    // Use the laser data to generate a set of sections
    extractSections(laserData->ranges,
                    laserData->startAngle,
                    angleIncrement,
                    laserMaxRange_,
                    MIN_POINTS_IN_LINE,
                    RANGE_DELTA,
                    sections);

    // Fit lines to the sections
    extractLines( sections, MIN_POINTS_IN_LINE );

    // Find corner features, add them to the list of features
    addCorners( sections, features );

    // Find line features, add them to the list of features
    addLines( sections, features );
}

void
CornerExtractor::addLines( const std::vector<Section> &sections, 
                           orca::PolarFeature2dDataPtr &features )
{
    std::vector<Section>::const_iterator i;
    for ( i = sections.begin(); i != sections.end(); i++ )
    {
        if ( ! i->isALine() ) continue;

        orca::LinePolarFeature2dPtr f = new orca::LinePolarFeature2d;
        f->type = orca::feature::LINE;

        f->start.r = (*i).start().range();
        f->start.o = (*i).start().bearing();
        f->end.r   = (*i).end().range();
        f->end.o   = (*i).end().bearing();

        f->startSighted = true;
        f->endSighted = true;

        f->pFalsePositive = 0.3;
        f->pTruePositive  = 0.7;
        features->features.push_back( f );

#if 0        
        // Find p in x-y coords
        double denom = (*i).eigVectX*(*i).eigVectX+(*i).eigVectY*(*i).eigVectY;
        double px = -((*i).C*(*i).eigVectX)/denom;
        double py = -((*i).C*(*i).eigVectY)/denom;

        // convert to polar
        f->p.r = hypotf(py,px);
        f->p.o = atan2(py,px);

        f->pFalsePositive = 0.3;
        f->pTruePositive  = 0.7;

        features->features.push_back( f );

        double slopeEnd = ((*i).elements.back().y()-(*i).elements.front().y())/((*i).elements.back().x()-(*i).elements.front().x());
        cout<<"TRACE(cornerextractor.cpp): slope of line (based on endpoints): " << slopeEnd << endl;
        double slopeEig = -(*i).eigVectX/(*i).eigVectY;
        cout<<"TRACE(cornerextractor.cpp): slope of line (based on eigvect)  : " << slopeEig << endl;
        cout<<"TRACE(cornerextractor.cpp): slope of perp (based on eigvect)  : " << -1.0/slopeEig << endl;
        double slopeP = py/px;
        cout<<"TRACE(cornerextractor.cpp): slope of perp: " << slopeP << endl;
        cout<<"TRACE(cornerextractor.cpp): start: " << (*i).elements.front().x() <<", "<<(*i).elements.front().y() << endl;
        cout<<"TRACE(cornerextractor.cpp): p:     " << px << ", " << py << endl;
        cout<<"TRACE(cornerextractor.cpp): end:   " << (*i).elements.back().x() <<", "<<(*i).elements.back().y() << endl;
        cout << "----------------------------" << endl;
#endif
    }
}

void
CornerExtractor::addCorners( const std::vector<Section> &sections, 
                             orca::PolarFeature2dDataPtr &features )
{
    std::vector<Section>::const_iterator itr;
    std::vector<Section>::const_iterator next;
  
    for (itr = sections.begin(), next = sections.begin() + 1; itr != sections.end() && next != sections.end(); itr++, next++)
    {
        // check that we have two connected lines before computing the angle between them
        // We also want to avoid extracting corners at the junction between observations of the
        // ground and a wall.
        if (itr->isALine() && itr->isNextCon() && next->isALine()) {// &&
            //(itr->elements.size() < MAX_POINTS_IN_LINE) &&
            //(next->elements.size() < MAX_POINTS_IN_LINE)) {

            if ( itr->lineLength() < minLineLength_ )
            {
                cout<<"TRACE(cornerextractor.cpp): line too small: length is " << itr->lineLength() << endl;
                continue;
            }
            if ( next->lineLength() < minLineLength_ )
            {
                cout<<"TRACE(cornerextractor.cpp): line too small: length is " << next->lineLength() << endl;
                continue;
            }

            double pFalsePositive = P_FALSE_POSITIVE;

            if (REJECT_GROUND_OBSERVATIONS && 
            ((itr->elements().size() > MAX_POINTS_IN_LINE && fabs(itr->perpVectY()) < 0.1) ||
            (next->elements().size() > MAX_POINTS_IN_LINE && fabs(next->perpVectY()) < 0.1))) 
            {
              //std::cout << "We have a big line with a near horizontal slope.  Could be the ground??? Slope A : " << itr->eigVectY << " Slope B : " << next->eigVectY << std::endl;
              pFalsePositive = P_FALSE_POSITIVE_POSSIBLE_GROUND;
            }
  
            // We have a corner
            double A1 = itr->perpVectX();
            double B1 = itr->perpVectY();
            double C1 = itr->c();
  
            double A2 = next->perpVectX();
            double B2 = next->perpVectY();
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
                    pp->pFalsePositive = pFalsePositive;
                    pp->pTruePositive  = P_TRUE_POSITIVE;
                    features->features.push_back( pp );
                }
            }
        }
        //itr = itr->next;
    }
    //
    // Now we can get the possible corners...
    //
//  extractPossibleCorners(features);  
 
}

// bool CornerExtractor::extractPossibleCorners( const orca::PolarFeature2dDataPtr & featureDataPtr )
// {
// //  return false; // for now, we won't be looking for possible corners...

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
