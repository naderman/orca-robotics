/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Ben Upcroft, Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_INS_INTERFACE_ICE
#define ORCA2_INS_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>

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
    //! Elevation (metres above ellipsoid)
    //! usual output of GPS receivers
    double elevation;
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
    void setData( InsData obj );
};

/*!
    @brief Access to INS data.
*/
interface Ins
{
    //! Returns the latest data.
    //! @note In Orca1 this would be called ClientPull_Supplier interface.
    ["cpp:const"] idempotent InsData getData()
            throws HardwareFailedException;

    ["cpp:const"] idempotent InsDescription getDescription();

    /*!
     * Mimics IceStorm's subscribe() but without QoS, for now. The
     * implementation may choose to implement the data push internally
     * or use IceStorm. This choice is transparent to the subscriber.
     *
     * @param subscriber The subscriber's proxy.
     *
     * @see unsubscribe
     */
    void subscribe( InsConsumer* subscriber )
            throws SubscriptionFailedException;

    /*!
     * Unsubscribe the given [subscriber].
     *
     * @param subscriber The proxy of an existing subscriber.
     *
     * @see subscribe
     */
    idempotent void unsubscribe( InsConsumer* subscriber );
};


//!  //@}
}; // module

#endif
