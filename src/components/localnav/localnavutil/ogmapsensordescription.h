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
