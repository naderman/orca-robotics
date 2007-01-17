/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAMISCCONFIGUTILS_H
#define ORCAMISCCONFIGUTILS_H

#include <Ice/Ice.h>
#include <orca/vehicledescription.h>
#include <string>

namespace orcamisc {

/*!
 *  @name Configuration Utilities
 */
//@{

//! Read in a VehicleDescription struct from a config file.
//! The properties are expected to be laid out like the struct is.
//!
//! The prefix will have 'VehicleDescription' appended to it before looking for properties.
//!
//! Throws orcaice::ConfigFileException if things are malformed.
//!
void readVehicleDescription( Ice::PropertiesPtr prop,
                             const std::string &prefix,
                             orca::VehicleDescription &d );

//@}

}

#endif
