/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_RANGER_ARRAY_INTERFACE_ICE
#define ORCA2_RANGER_ARRAY_INTERFACE_ICE

#include <orca/common.ice>
#include <orca/bros1.ice>
#include <IceStorm/IceStorm.ice>

module orca
{

sequence<Time> TimeSeq;
sequence<Frame3d> Frame3dSeq;

/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_rangerarray RangerArray
    @brief Multi-origin array of ranger sensors

This interface provides access to a multi-origin array of ranger sensors.

    @{
*/

//! Device description
struct RangerArrayDescription
{
    //! Time when description was generated.
    Time timeStamp;

    //! Minimum range [m]
    double minRange;

    //! Maximum range [m]
    double maxRange;

    //! Detection cone angle of every ranger [rad]
    double detectionConeAngle;

    //! Offset of the centre of each sensor with respect to the robot,
    //! in the robot local coordinate system. Note that the length of
    //! this sequence also defines the number of rangers
    Frame3dSeq poses;
};

/*!
    Scan data structure
*/
class RangerArrayData
{
    //! Time when the object was observed, created, etc.
    TimeSeq timeStamps;

    //! Scan ranges [m]
    FloatSeq ranges;
};

/*!
    @brief Consumesrange scans
*/
interface RangerArrayConsumer
{
    //! Transmits the data to the consumer.
    void setData( RangerArrayData obj );
};

/*!
    @brief Multi-origin array of ranger sensors
 */
interface RangerArray
{
    //! Returns device description.
    idempotent RangerArrayDescription getDescription();

    //! Returns the latest data.
    //! May raise DataNotExistException if the requested information is not available.
    //! May raise HardwareFailedException if there is some problem with hardware.
    //! @note In Orca1 this would be called ClientPull_Supplier interface.
    idempotent RangerArrayData      getData()
            throws DataNotExistException, HardwareFailedException;

    //! Tries to subscribe the specified subscriber for data updates.
    //! If successfuly, returns a proxy to the IceStorm topic which can be later used by the 
    //! client to unsubscribe itself. For reference, the Slice definition of the Topic
    //! interface for unsubscribing:
    //! @verbatim
    //! idempotent void unsubscribe(Object* subscriber);
    //! @endverbatim
    IceStorm::Topic* subscribe( RangerArrayConsumer* subscriber )
        throws SubscriptionFailedException;
};

/*! @} */
}; // module

#endif
