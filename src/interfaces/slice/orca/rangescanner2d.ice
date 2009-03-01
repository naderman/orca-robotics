/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_RANGE_SCANNER_2D_INTERFACE_ICE
#define ORCA2_RANGE_SCANNER_2D_INTERFACE_ICE

#include <orca/common.ice>
#include <orca/bros1.ice>
#include <IceStorm/IceStorm.ice>

module orca
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_rangescanner2d RangeScanner2d
    @brief Single-origin scanning range sensor

The rangescanner interface provides access to a single-origin scanning range
sensor.

Devices supporting the rangescanner interface can be configured to scan at
different angles and resolutions.  As such, the data returned by the
rangescanner interface can take different forms.  To make interpretation of the
data simple, the rangescan data packet contains some extra fields before the
actual range data.  These fields tell the client the starting and ending
angles of the scan, the angular resolution of the scan, and the number of
range readings included.  Scans proceed counterclockwise about the sensor
(0 degrees is forward).  

    @{
*/

//! Device description
struct RangeScanner2dDescription
{
    //! Time when description was generated.
    Time timeStamp;

    //! Minimum range [m]
    double minRange;

    //! Maximum range [m]
    double maxRange;

    //! Angular field of view [rad]
    double fieldOfView;

    //! Starting angle of the scan [rad]
    //! In other words, this is the bearing to the first return in 
    //! local coordinate system. All other returns have bearing angle 
    //! larger than this one.
    double startAngle;

    //! Number of range samples in a single scan
    int    numberOfSamples;

    //! Offset of the centre of the sensor with respect to the robot, 
    //! in the robot local coordinate system.
    Frame3d offset;

    //! Dimensions of the sensor
    Size3d  size;
};

/*!
    Planar range scan data structure
*/
class RangeScanner2dData
{
    //! Time when the object was observed, created, etc.
    Time timeStamp;

    //! Scan ranges [m]
    FloatSeq     ranges;

    //! Minimum range [m]
    double minRange;

    //! Maximum range [m]
    double maxRange;

    //! Angular field of view [rad]
    double fieldOfView;

    //! Starting angle of the scan [rad]
    //! In other words, this is the bearing to the first return in 
    //! local coordinate system. All other returns have bearing angle 
    //! larger than this one.
    double startAngle;
};

/*!
    @brief Consumesrange scans
*/
interface RangeScanner2dConsumer
{
    //! Transmits the data to the consumer.
    void setData( RangeScanner2dData obj );
};

/*!
    @brief Single-origin scanning range sensor
 */
interface RangeScanner2d
{
    //! Returns device description.
    idempotent RangeScanner2dDescription getDescription();

    //! Returns the latest data.
    //! May raise DataNotExistException if the requested information is not available.
    //! May raise HardwareFailedException if there is some problem with hardware.
    //! @note In Orca1 this would be called ClientPull_Supplier interface.
    idempotent RangeScanner2dData      getData()
            throws DataNotExistException, HardwareFailedException;            

    //! Tries to subscribe the specified subscriber for data updates.
    //! If successfuly, returns a proxy to the IceStorm topic which can be later used by the 
    //! client to unsubscribe itself. For reference, the Slice definition of the Topic
    //! interface for unsubscribing:
    //! @verbatim
    //! idempotent void unsubscribe(Object* subscriber);
    //! @endverbatim
    IceStorm::Topic* subscribe( RangeScanner2dConsumer* subscriber )
        throws SubscriptionFailedException;
};

/*! @} */
}; // module

#endif
