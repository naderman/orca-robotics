/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Ben Upcroft, Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_INS_INTERFACE_ICE
#define ORCA2_INS_INTERFACE_ICE

#include <orca/common.ice>
#include <orca/bros1.ice>
#include <IceStorm/IceStorm.ice>

module orca
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_ins Ins
    @brief Access to Inertial Navigation Systems
    @{
*/

//! Ins configuration objectd
struct InsDescription
{
    //! Time when data was measured.
    Time timeStamp;

    //! Offset of the sensor with respect to the robot,
    //! in the robot local coordinate system.
    Frame3d offset; 

    //! Dimensions of the sensor
    Size3d  size;
};

//! The @p Ins interface returns position, velocity and orientation
//! from the INS sensor. 
struct InsData
{
    //! Time when data was measured.
    Time timeStamp;

    //! Latitude (degrees)
    double latitude;
    //! Longitude (degrees)
    double longitude;
    //! [m] (above ellipsoid)
    //! usual output of GPS receivers
    double altitude;
    //! Height (metres above geoid, aka height A_bove M_ean S_ea L_evel)
    //! useful to relate to maps/gis
    double heightAMSL;

    //! E_ast N_orth U_p (ENU) velocity; relative to local UTM grid
    CartesianVelocity vENU;
    //! Orientation relative to local UTM grid (yaw == 0 --> East)
    OrientationE   o;

};


/*!
 * Ins PVA state information consumer interface
 */
interface InsConsumer
{
    //! Transmits the data to the consumer.
    void setData( InsData obj );
};

/*!
    @brief Access to INS data.
*/
interface Ins
{
    //! Returns device description.
    idempotent InsDescription getDescription();

    //! Returns the latest data.
    //! @note In Orca1 this would be called ClientPull_Supplier interface.
    idempotent InsData getData()
            throws HardwareFailedException;

    //! Tries to subscribe the specified subscriber for data updates.
    //! If successfuly, returns a proxy to the IceStorm topic which can be later used by the 
    //! client to unsubscribe itself. For reference, the Slice definition of the Topic
    //! interface for unsubscribing:
    //! @verbatim
    //! idempotent void unsubscribe(Object* subscriber);
    //! @endverbatim
    IceStorm::Topic* subscribe( InsConsumer* subscriber )
        throws SubscriptionFailedException;
};


//!  //@}
}; // module

#endif
