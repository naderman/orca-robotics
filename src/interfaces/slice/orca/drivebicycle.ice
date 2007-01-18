/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_DRIVE_BICYCLE_INTERFACE_ICE
#define ORCA2_DRIVE_BICYCLE_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/vehicledescription.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_drivebicycle DriveBicycle
    @brief Controls a mobile robot with bicycle steering.
    @{
*/

//! Bicycle steering velocity command.
struct DriveBicycleData
{
    //! Angle of the steering wheel [rad]
    double steerAngle;

    //! Forward speed [m/s]. Negative for reverse.
    double speed;
};

/*!
    Controls a mobile robot with differential steering.
*/
interface DriveBicycle
{
    //! Get vehicle description
    nonmutating VehicleDescription getDescription();

    //! Set velocity command
    idempotent void setCommand( DriveBicycleData data )
            throws HardwareFailedException;    
};

//! @}
}; // module

#endif
