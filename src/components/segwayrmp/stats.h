/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Duncan Mercer
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef SEGWAYRMP_STATS_H
#define SEGWAYRMP_STATS_H

#include <hydrointerfaces/segwayrmp.h>

namespace segwayrmp {


/*!
Not thread safe.

@author Alex Makarenko, based on earlier code by Duncan Mercer
*/
class Stats
{

public: 
    Stats();

    void addData( const hydrointerfaces::SegwayRmp::Data& data );

    // total distance travelled [m]
    double distance() const { return distance_; };
    // total time while in motion [m/s]
    double timeInMotion() const { return timeInMotion_; };
    // maximum (forward) speed [m/s]
    double maxSpeed() const { return maxSpeed_; };

private: 

    hydrointerfaces::SegwayRmp::Data lastData_;
    bool haveData_;
    
    double distance_;
    double timeInMotion_;
    double maxSpeed_;
};

} // namespace

#endif
