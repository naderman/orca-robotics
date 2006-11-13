/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_RANGE_SCANNER_2D_INTERFACE_ICE
#define ORCA2_RANGE_SCANNER_2D_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>

module orca
{
/*!
    @ingroup interfaces
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

//! A sequence of ranges to the target.
sequence<float> RangeSequence;

//! RangeScanner configuration data structure
class RangeScanner2dConfig extends OrcaObject
{
    //! maximum range of each return
    float maxRange;

    //! Are we talking to the hardware?
    bool  isEnabled;
};


//! RangeScanner Geometry data structure
class RangeScanner2dGeometry extends OrcaObject
{
    //! Offset of the centre of the sensor from the robot, int the robot CS
    Frame3d offset;

    //! Dimensions of the sensor
    Size3d  size;
};

/*!
    Range scan data structure
*/
class RangeScanner2dData extends OrcaObject
{
    //! Distance to returns
    RangeSequence     ranges;

    //! Bearing to the first return in local coordinate system
    float             startAngle;

    //! Constant angle between successive returns.
    float             angleIncrement;
};

/*!
    @brief Consumer of range scans
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
    //! Returns the latest data.
    //! May raise DataNotExistException if the requested information is not available.
    //! May raise HardwareFailedException if there is some problem with hardware.
    //! @note In Orca1 this would be called ClientPull_Supplier interface.
    nonmutating RangeScanner2dData      getData()
            throws DataNotExistException, HardwareFailedException;
            
    //! Returns the current configuration.
    nonmutating RangeScanner2dConfig    getConfig();
    
    //! Returns the current geometry information.
    nonmutating RangeScanner2dGeometry  getGeometry();

    /*!
     * Mimics IceStorm's subscribe(). @p subscriber is typically a direct proxy to the consumer object.
     * The implementation may choose to implement the push directly or use IceStorm.
     * This choice is transparent to the subscriber. The case when the @p subscriber is already subscribed
     * is quietly ignored.
     *
     * @see unsubscribe
     */
    void subscribe( RangeScanner2dConsumer *subscriber )
            throws SubscriptionFailedException;

    /*!
     * Unsubscribe an existing @p subscriber. The case when the @p subscriber is not subscribed
     * is quietly ignored.
     *
     * @see subscribe
     */
    idempotent void unsubscribe( RangeScanner2dConsumer *subscriber );
};

/*! @} */
}; // module

#endif
