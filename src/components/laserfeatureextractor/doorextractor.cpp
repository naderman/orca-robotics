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
#include "doorextractor.h"

#include <iostream>
#include <cmath>

#include <orca/featuremap2d.h>


using namespace std;

namespace laserfeatures {

namespace {
    const double P_FALSE_POSITIVE = 0.3;
    const double P_TRUE_POSITIVE  = 0.6;
}
    
void DoorExtractor::addFeatures( const orca::LaserScanner2dDataPtr &laserData,
                                       orca::PolarFeature2dDataPtr &features )
{    
    unsigned int i;
    bool buildingTarget = false;

    int featureNumPnts = 0;

    double delta_range = 0;
    double door_width_sq;
    double startBearing = - M_PI/2;
    double startRange = -1.0;
    double stopBearing = - M_PI/2;
    double stopRange = -1.0;
    double startX = 0.0;
    double startY =0.0;
    double stopX, stopY;
    double min_width = 0.9*0.9;
    double max_width = 1.5*1.5;

    //std::cout << "Looking for open doors..." << std::endl;
    // for each return
    for ( i=10; i<laserData->ranges.size()-10; i++ )
    {
        // compute the step change in range
        delta_range = laserData->ranges[i] - laserData->ranges[i-1];

        if (buildingTarget == true)
        {
            // if we have reached the end of a cluster, process a new observation
            if ( delta_range < -2.0 )
            {
                buildingTarget = false;
                stopRange = laserData->ranges[i];
                stopBearing = M_PI*(i)/(laserData->ranges.size()-1) - M_PI/2;
                stopX = stopRange*cos(stopBearing);
                stopY = stopRange*sin(stopBearing);


                door_width_sq = pow(stopX - startX, 2.0) + pow(stopY - startY, 2.0);

                //std::cout << " Checking a new cluster at range " << featureRange / featureNumPnts << " bearing " << featureBearing / featureNumPnts << " of width " << pole_width << " with " << featureNumPnts << " points" << std::endl;
                // again make sure that the feature is not a foreground point
                if (featureNumPnts > 5 && door_width_sq > min_width &&
                    door_width_sq < max_width )
                {
                    orca::PointPolarFeature2dPtr pp1 = new orca::PointPolarFeature2d;
                    pp1->type = orca::feature::DOOR;
                    pp1->p.r = startRange;
                    pp1->p.o = startBearing;
                    pp1->pFalsePositive = P_FALSE_POSITIVE;
                    pp1->pTruePositive  = P_TRUE_POSITIVE;

                    orca::PointPolarFeature2dPtr pp2 = new orca::PointPolarFeature2d;
                    pp2->type = orca::feature::DOOR;
                    pp2->p.r = stopRange;
                    pp2->p.o = stopBearing;
                    pp2->pFalsePositive = P_FALSE_POSITIVE;
                    pp2->pTruePositive  = P_TRUE_POSITIVE;

                    features->features.push_back(pp1);
                    features->features.push_back(pp2);
                }

                buildingTarget = false;
            } else {
                featureNumPnts++;
            }  // end of end-of-cluster if
        }     // end for each return
        if ( delta_range > 2.0 )
        {
            // start building a new door shape
            if ( buildingTarget == false)
            {
                buildingTarget = true;
                featureNumPnts = 1;
                startRange = laserData->ranges[i-1];
                startBearing = M_PI*(i-1)/(laserData->ranges.size()-1) - M_PI/2;
                startX = startRange*cos(startBearing);
                startY = startRange*sin(startBearing);
                continue; // go to the next return
            }
        }
    }                                 
    
}

}
