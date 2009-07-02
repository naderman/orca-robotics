/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_DRIVE_BICYCLE_INTERFACE_ICE
#define ORCA2_DRIVE_BICYCLE_INTERFACE_ICE

#include <orca/common.ice>
#include <orca/vehicledescription.ice>
#include <IceStorm/IceStorm.ice>

module orca
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_drivebicycle DriveBicycle
    @brief Controls a mobile robot with bicycle steering.
    @{
*/

//! Bicycle command: steering angle & velocity.
struct DriveBicycleCommand
{
    //! Time when data was measured.
    Time timeStamp;

    //! Reference value for the angle of the steered wheel [rad].
    //! This is the command currently executed by the vehicle.
    double steerAngle;

    //! Reference value for the forward speed of the steered wheel [m/s].
    //! The value is negative when the vehicle is to move backwards.
    double speed;
};

//! Bicycle data: commanded and actual steering angle & velocity.
struct DriveBicycleData
{
    //! Time when data was measured.
    Time timeStamp;

    //! Reference value for the angle of the steered wheel [rad].
    //! This is the command currently executed by the vehicle.
    double referenceSteerAngle;

    //! Reference value for the forward speed of the steered wheel [m/s].
    //! The value is negative when the vehicle is to move backwards.
    double referenceSpeed;

    //! Current angle of the steered wheel [rad]
    double currentSteerAngle;

    //! Current forward speed of the steered wheel [m/s]. 
    //! The value is negative when the vehicle moves backwards.
    double currentSpeed;
};

//! Data consumer interface.
interface DriveBicycleConsumer
{
    //! Transmits the data to the consumer.
    void setData( DriveBicycleData obj );
};


/*!
    Controls a mobile robot with bicycle steering.
*/
interface DriveBicycle
{
    //! Get vehicle description
    idempotent VehicleDescription getDescription();

    //! Returns the latest data.
    //! May raise DataNotExistException if the requested information is not available.
    //! May raise HardwareFailedException if there is some problem with hardware.
    //! May raise EStopTriggeredException if an e-stop is active.
    idempotent DriveBicycleData getData()
            throws DataNotExistException, HardwareFailedException, EStopTriggeredException;
    
    //! Set velocity command
    idempotent void setCommand( DriveBicycleCommand data )
            throws HardwareFailedException;    

    //! Tries to subscribe the specified subscriber for data updates.
    //! If successfuly, returns a proxy to the IceStorm topic which can be later used by the 
    //! client to unsubscribe itself. For reference, the Slice definition of the Topic
    //! interface for unsubscribing:
    //! @verbatim
    //! idempotent void unsubscribe(Object* subscriber);
    //! @endverbatim
    IceStorm::Topic* subscribe( DriveBicycleConsumer* subscriber )
        throws SubscriptionFailedException, SubscriptionPushFailedException;
};

//! @}
}; // module

#endif
