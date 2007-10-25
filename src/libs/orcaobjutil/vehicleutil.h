/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAOBJUTIL_VEHICLE_DESCR_UTILS_H
#define ORCAOBJUTIL_VEHICLE_DESCR_UTILS_H

#include <Ice/Ice.h>
#include <orca/vehicledescription.h>
#include <string>

namespace orcaobjutil {

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

// The prefix will have '.Control.VelocityDifferential' appended to it before looking for properties.
void
readVehicleControlVelocityDifferentialDescription( Ice::PropertiesPtr prop,
                                                   const std::string &prefix,
                                                   orca::VehicleControlVelocityDifferentialDescription &c );

// The prefix will have '.Control.VelocityDifferential' appended to it before looking for properties.
void
readVehicleControlVelocityBicycleDescription( Ice::PropertiesPtr prop,
                                              const std::string &prefix,
                                              orca::VehicleControlVelocityBicycleDescription &c );

//! Performs some basic sanity checks, and
//! throws orcaice::ConfigFileException if something is wrong.
void  checkVehicleControlVelocityDifferentialDescription( 
    const orca::VehicleControlVelocityDifferentialDescription &d );

//! Performs some basic sanity checks, and
//! throws orcaice::ConfigFileException if something is wrong.
void  checkVehicleControlVelocityBicycleDescription( 
    const orca::VehicleControlVelocityBicycleDescription &d );

//@}

}

#endif
