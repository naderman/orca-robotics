/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_DRIVE_BICYCLE_INTERFACE_ICE
#define ORCA2_DRIVE_BICYCLE_INTERFACE_ICE

#include <orca/orca.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_drivebicycle DriveBicycle
    @brief Controls a mobile robot with bicycle steering.
    @{
*/

//! Description of differential steering mechanism
struct DriveBicycleDescription
{
    //! Distance between the front and rear wheels [m]
    double wheelBase;    

    //! Maximum forward speed [m/s]
    double maxSpeed;

    //! Maximum reverse speed [m/s]
    double maxReverseSpeed;

    //! Minimum steer angle [rad]
    double minSteerAngle;

    //! Maximum steer angle [rad]
    double maxSteerAngle;
};

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
    //! Get description
    nonmutating DriveBicycleDescription getDescription();

    //! Set velocity command
    idempotent void setCommand( DriveBicycleData data )
            throws HardwareFailedException;    
};

//! @}
}; // module

#endif
