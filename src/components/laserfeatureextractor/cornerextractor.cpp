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

double lineLength( const Section &s )
{
    const SectionEl &start = s.elements[0];
    const SectionEl &end   = s.elements[s.elements.size()-1];

    // A bit unstable if the end-points are screwy, but should be mostly OK...
    return hypotf( start.y()-end.y(), start.x()-end.x() );
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
    
    std::vector<Section> sections;

    // Use the laser data to generate a set of sections
    connectSections(laserData,sections);

    // Fit lines to the sections
    extractLines( sections, MIN_POINTS_IN_LINE );

    // Find corners, add them to the list of features
    addCorners( sections, features );
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
        if (itr->isALine && itr->isNextCon && next->isALine) {// &&
            //(itr->elements.size() < MAX_POINTS_IN_LINE) &&
            //(next->elements.size() < MAX_POINTS_IN_LINE)) {

            if ( lineLength( *itr ) < minLineLength_ )
            {
                cout<<"TRACE(cornerextractor.cpp): line too small: length is " << lineLength(*itr) << endl;
                continue;
            }
            if ( lineLength( *next ) < minLineLength_ )
            {
                cout<<"TRACE(cornerextractor.cpp): line too small: length is " << lineLength(*next) << endl;
                continue;
            }

            double pFalsePositive = P_FALSE_POSITIVE;

            if (REJECT_GROUND_OBSERVATIONS && 
            ((itr->elements.size() > MAX_POINTS_IN_LINE && fabs(itr->eigVectY) < 0.1) ||
            (next->elements.size() > MAX_POINTS_IN_LINE && fabs(next->eigVectY) < 0.1))) 
            {
              //std::cout << "We have a big line with a near horizontal slope.  Could be the ground??? Slope A : " << itr->eigVectY << " Slope B : " << next->eigVectY << std::endl;
              pFalsePositive = P_FALSE_POSITIVE_POSSIBLE_GROUND;
            }
  
            // We have a corner
            double A1 = itr->eigVectX;
            double B1 = itr->eigVectY;
            double C1 = itr->C;
  
            double A2 = next->eigVectX;
            double B2 = next->eigVectY;
            double C2 = next->C;
  
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
    
                    orca::SinglePolarFeature2dPtr pp = new orca::SinglePolarFeature2d;
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

void 
CornerExtractor::connectSections( const orca::LaserScanner2dDataPtr & laserDataPtr,
                                  std::vector<Section> &sections )
{
    //Section *head = new Section();
    //Section *current = head;
    Section current;
  
    bool inSection = true;
    if (laserDataPtr->ranges[0] >= laserMaxRange_) {
        inSection = false;
    } else {
        double r = laserDataPtr->ranges[0];
        double b = - M_PI/2;
        SectionEl pos(r, b);
        current.elements.push_back(pos);
    }

    for (unsigned int i = 1; i < laserDataPtr->ranges.size(); i++) {
        if (laserDataPtr->ranges[i] >= laserMaxRange_) {
            if (laserDataPtr->ranges[i-1] < laserMaxRange_) {
              // found the end of the current section with an out of range reading
              // ignore elements with less candidate points than would constitute 2 lines
              if (current.elements.size() > 2*MIN_POINTS_IN_LINE)
              {
                sections.push_back(current);
              }
              current.elements.clear();
            } else {
                // We are still in an out of range section
                // ignore...
            }
        } else if (laserDataPtr->ranges[i-1] >= laserMaxRange_ ||
                   fabs(laserDataPtr->ranges[i] - laserDataPtr->ranges[i-1]) < RANGE_DELTA) 
        {
            // Add this point to the current section
            double r = laserDataPtr->ranges[i];
            double b = M_PI*i/(laserDataPtr->ranges.size()-1) - M_PI/2;
            SectionEl pos(r, b);
            current.elements.push_back(pos);
         } else {
           // There has been a step change in range, start a new section
           // ignore elements with less candidate points than would constitute 2 lines
           if (current.elements.size() > 2*MIN_POINTS_IN_LINE)
           {
             sections.push_back(current);
           }
           current.elements.clear();
         }
    }

    if (current.elements.size() > 2*MIN_POINTS_IN_LINE)
    {
      sections.push_back(current);
    }
    current.elements.clear();
    
    //std::cout << "FeatureExtractor : Found " << sections.size() << " sections" << std::endl;
    //printSections();
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
//                     orca::SinglePolarFeature2dPtr pp = new orca::SinglePolarFeature2d;
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
//                     orca::SinglePolarFeature2dPtr pp = new orca::SinglePolarFeature2d;
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
