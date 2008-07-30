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

#include "driver.h"
#include <orcaice/context.h>

namespace gps2localise2d {

//
// @author Alex Brooks
//
class SimpleDriver : public Driver
{

public: 

    SimpleDriver( const orca::GpsDescription &descr, const orcaice::Context &context );

    // Converts the GPS info into localise2d format.
    // Returns 'false' if the conversion can't be done (eg because GPS doesn't have a fix).
    bool compute( const orca::GpsData &gpsData, orca::Localise2dData &localiseData );

private: 

    // Offset from gps origin to global coord system
    orca::Frame2d offset_;

    orca::GpsDescription  descr_;
    // Antenna offset as Frame2d.
    orca::Frame2d         antennaTransform_;
    orcaice::Context      context_;

};

}

#endif
