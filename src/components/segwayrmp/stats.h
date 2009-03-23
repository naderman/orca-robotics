/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Duncan Mercer
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef SEGWAYRMP_STATS_H
#define SEGWAYRMP_STATS_H

#include <hydrointerfaces/segwayrmp.h>

namespace segwayrmp {


/*
Not thread safe.

@author Alex Makarenko, based on earlier code by Duncan Mercer
*/
class Stats
{

public: 
    Stats();

    // The raw data stream may be reset during the life time of Stats object.
    // This happens for example when the hardware driver is reinitialized.
    // We need to know, or the robot will appear to have teleported.
    // The stats collected so far will not be lost.
    void resetRawData();

    // Add new piece of data to stats
    void addData( const hydrointerfaces::SegwayRmp::Data& data );

    // total distance travelled [m]
    double distance() const { return distance_; };
    // total time while in motion [m/s]
    double timeInMotion() const { return timeInMotion_; };
    // maximum (forward) speed [m/s]
    double maxSpeed() const { return maxSpeed_; };

private: 
    // keep record of raw data
    hydrointerfaces::SegwayRmp::Data lastData_;
    bool haveData_;
    
    // stats
    double distance_;
    double timeInMotion_;
    double maxSpeed_;
};

} // namespace

#endif
