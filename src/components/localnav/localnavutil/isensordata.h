/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ISENSORDATA_H
#define ISENSORDATA_H

#include <orca/orca.h>

namespace localnav {

//!
//! @author Alex Brooks
//!

//! Abstract class for wrapping up sensor information data such as LaserScanner2dData and OgMapData.
//! Pathplanning drivers are passed as an argument, data in this abstract form and must dynamic_cast
//! the data into the required format.
//!
//! All sensor information data should inherit from this class.
class ISensorData
{

public: 

    virtual ~ISensorData(){};

    //! method for wrapping up access to the object's timestamp.
    //! This is needed since RangeScanner2dData is in the form of a pointer whereas
    //! OgMapData is not. Thus access to object members use different syntax.
    virtual const orca::Time &timeStamp() const=0;
    

private: 


};

}

#endif
