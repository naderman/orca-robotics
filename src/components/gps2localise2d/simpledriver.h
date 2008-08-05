/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef SIMPLEDRIVER_H
#define SIMPLEDRIVER_H

#include <memory>
#include <orcaice/context.h>
#include "driver.h"
#include "gpsheuristics.h"
#include <hydronavutil/pose.h>

namespace gps2localise2d {

//
// @author Alex Brooks, Tobias Kaupp
//
class SimpleDriver : public Driver
{

public: 

    SimpleDriver( const orca::GpsDescription     &gpsDescr, 
                  const orca::VehicleDescription &vehicleDescr,
                  const orcaice::Context         &context );

    // Converts the GPS info into localise2d format.
    // Returns 'false' if the conversion can't be done (eg because GPS doesn't have a fix).
    bool compute( const orca::GpsData &gpsData, orca::Localise2dData &localiseData );

private: 

    // Offset from gps origin to global coord system
    hydronavutil::Pose offset_;

    orca::GpsDescription gpsDescr_;
    
    hydronavutil::Pose    antennaTransform_;
    orcaice::Context      context_;
    
    std::auto_ptr<GpsHeuristics> gpsHeuristics_;

};

}

#endif
