/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ISENSORDESCRIPTION_H
#define ISENSORDESCRIPTION_H

#include <orca/orca.h>

namespace localnav {

//!
//! @author Ben Upcroft
//!

//! Abstract class for wrapping up sensor information descriptions such as LaserScanner2dDescription.
//! Pathplanning drivers are passed as an argument, descriptions in this abstract form and must dynamic_cast
//! the description into the required format.
//!
//! All sensor information descriptions should inherit from this class.
class ISensorDescription
{

public: 

    virtual ~ISensorDescription(){};

private: 


};

}

#endif
