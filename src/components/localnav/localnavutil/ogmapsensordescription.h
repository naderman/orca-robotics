/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef OGMAPSENSORDESCRIPTION_H
#define OGMAPSENSORDESCRIPTION_H

#include "isensordescription.h"
#include <orca/rangescanner2d.h>

namespace localnav {

//!
//! @author Alex Brooks
//!

//! Specific implementation of ISensorDescription for OgMaps. Note that
//! OgMaps do not have a descriptions and thus this implementation is just a place
//! holder when an ISensorDescription is required in localnav::MainLoop
class OgMapSensorDescription : public ISensorDescription
{

public: 

    ~OgMapSensorDescription(){};

    // void setDescr( const orca::OgMapDescription ogMapDescr ){ ogMapDescr_ = ogMapDescr; };
    // orca::OgMapDescription ogMapDescr(){ return ogMapDescr_; };

private: 

    // orca::OgMapDescription ogMapDescr_;

};

}

#endif
