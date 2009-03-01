/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <algorithm>
#include <orcaice/orcaice.h>

#include "randomdriver.h"

using namespace std;
using namespace reactivewalker;

RandomDriver::RandomDriver()
    : isAvoiding_(false),
      forceRandomCourseChange_(false)
{
}
 
//
// based on Player's examples/randomwalk.cc
//
int RandomDriver::computeCommand( const orca::RangeScanner2dDataPtr& laserData,
                                const orca::Odometry2dData& odometryData,
                                orca::VelocityControl2dData& commandData )
{
    double minfrontdistance = 0.750;

    double newturnrate = 0.0;
    double newspeed = 0.0;
    
    int randint;
    bool obs = false;

    // look for obstacles
    for ( unsigned int i=0; i<laserData->ranges.size(); ++i ) {
        if( laserData->ranges[i] < minfrontdistance ) {
            obs = true;
            break;
        }
    }
    
    const double avoidTimeMs = 1.5e3;

//     if( obs || avoidTimer_.elapsedMs()<avoidTimeMs || odometryData.stalled )
    if( obs || avoidTimer_.elapsedMs()<avoidTimeMs )
    {
        if ( obs ) cout<<"see obstacle"<<endl;
        if ( avoidTimer_.elapsedMs()<avoidTimeMs ) cout<<"continue avoiding"<<endl;
//         if ( odometryData.stalled ) cout<<"stalled"<<endl;

        // randomize these variables for robustness
        const double avoidSpeed = -0.050;
        const double avoidTurnRate = DEG2RAD(40);

        if ( isAvoiding_ == false ) {
            // pick direction of rotation and stick with it
            // are the obstacles closer on the left or on the right
            int half = (int)floor(laserData->ranges.size()/2.0);
            std::vector<float>::const_iterator leftIt =
                    std::min_element( laserData->ranges.begin(), laserData->ranges.begin()+half );
            float minLeft = *leftIt;
            std::vector<float>::const_iterator rightIt =
                    std::min_element( laserData->ranges.begin()+half, laserData->ranges.end() );
            float minRight = *rightIt;
    
            if( minLeft < minRight ) {
                avoidDirection_ = 1;
            } else {
                avoidDirection_ = -1;
            }
            
            isAvoiding_ = true;
            avoidTimer_.restart();
        }
        
        newspeed = avoidSpeed;
        newturnrate = avoidDirection_*avoidTurnRate;
        

        
        // we've been avoiding long enough
        if( avoidTimer_.elapsedMs()>=avoidTimeMs )
        {
            isAvoiding_ = false;
            forceRandomCourseChange_ = true;
        }
    }
    else
    {
        cout<<"no obstacle"<<endl;
        
        const double normalSpeed = 0.200;
        const double randomTimeMs = 2.0e3;
    
        newspeed = normalSpeed;

        // it's time to change course
        if( forceRandomCourseChange_ || randomTimer_.elapsedMs()>=randomTimeMs )
        {
            // make random int tween -20 and 20
            //randint = (1+(int)(40.0*rand()/(RAND_MAX+1.0))) - 20;
            randint = rand() % 41 - 20;
    
            newturnrate = DEG2RAD(randint);

            forceRandomCourseChange_ = false;
            randomTimer_.restart();
        }
    }

    // write commands to robot
    commandData.motion.v.x = newspeed;
    commandData.motion.v.y = 0.0;
    commandData.motion.w = newturnrate;
    
    return 0;
}
