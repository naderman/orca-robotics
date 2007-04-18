/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef SIMPLEDRIVER_H
#define SIMPLEDRIVER_H

#include "driver.h"

namespace gps2localise2d {

//!
//! @author Alex Brooks
//!
class SimpleDriver
{

public: 

    SimpleDriver();
    ~SimpleDriver();

    // Converts the GPS info into localise2d format.
    // Returns 'false' if the conversion can't be done (eg because GPS doesn't have a fix).
    bool compute( const orca::GpsData &gpsData, orca::Localise2dData &localiseData );

private: 


};

}

#endif
