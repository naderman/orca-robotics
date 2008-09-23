/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef LOCALNAV_STATS_H
#define LOCALNAV_STATS_H

#include <hydronavutil/pose.h>
#include <hydronavutil/velocity.h>

namespace localnav {


/*
Not thread safe.

Unlike the motion stats for a robotic platform e.g. Segway, we 
assume here that the pose is provided by the localiser which does
not have an infinite slow drift overtime. Therefore all reported 
motion is integrated into total distance travelled.

Attempts to discount large localization corrections.
*/
class Stats
{

public: 
    Stats();

    void addData( int seconds, int useconds,
                  const hydronavutil::Pose& pose, 
                  const hydronavutil::Velocity& vel,
                  bool isEnabled );

    // total distance travelled [m]
    double distance() const { return distance_; };
    // total time while in motion [m/s]
    double timeInMotion() const { return timeInMotion_; };
    // maximum (forward) speed [m/s]
    double maxSpeed() const { return maxSpeed_; };

private: 

    int lastSeconds_;
    int lastUseconds_;
    hydronavutil::Pose lastPose_;
    bool lastIsEnabled_;
    bool haveData_;
    
    double distance_;
    double timeInMotion_;
    double maxSpeed_;
};

} // namespace

#endif
