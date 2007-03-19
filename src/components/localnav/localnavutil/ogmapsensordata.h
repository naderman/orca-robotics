/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef OGMAPSENSORDATA_H
#define OGMAPSENSORDATA_H

#include "isensordata.h"
#include <orca/ogmap.h>

namespace localnav {

//!
//! @author Alex Brooks
//!
class OgMapSensorData : public ISensorData
{

public: 

    ~OgMapSensorData(){};

    virtual const orca::Time &timeStamp() const{ return ogMapData_.timeStamp; };

    void setData( const orca::OgMapData ogMapData ){ ogMapData_ = ogMapData; };
    orca::OgMapData ogMapData(){ return ogMapData_; };

private: 

    orca::OgMapData ogMapData_;

};

}

#endif
