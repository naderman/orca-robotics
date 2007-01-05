/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_DRIVE_DIFFERENTIAL_INTERFACE_ICE
#define ORCA2_DRIVE_DIFFERENTIAL_INTERFACE_ICE

#include <orca/orca.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_drivedifferential DriveDifferential
    @brief Controls a mobile robot with differential steering.
    @{
*/

//! Description of differential steering mechanism
struct DriveDifferentialDescription
{
    //! Distance between the wheels [m]
    double track;    

    //! Maximum left side speed [m/s]
    double maxLeftSpeed;

    //! Maximum right side speed [m/s]
    double maxRightSpeed;

    //! Maximum left side reverse speed [m/s]
    double maxLeftReverseSpeed;

    //! Maximum right side reverse speed [m/s]
    double maxRightReverseSpeed;

    //! Maximum speed differential [m/s]
    //! Maximum difference between speeds of the left and right sides.
    //! Assumed to be symmetric and independent of forward speed.
    double maxSpeedDifferential;
};

//! Differential steering velocity command.
struct DriveDifferentialData
{
    //! Left side speed [m/s]
    double leftSpeed;

    //! Right side speed [m/s]
    double rightSpeed;
};

/*!
    Controls a mobile robot with differential steering.
*/
interface DriveDifferential
{
    //! Get desciption
    nonmutating DriveDifferentialDescription getDescription();

    //! Set velocity command
    idempotent void setCommand( DriveDifferentialData data )
            throws HardwareFailedException;    
};

//! @}
}; // module

#endif
