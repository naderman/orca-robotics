/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef ORCA2_RANGE_SCANNER_INTERFACE_ICE
#define ORCA2_RANGE_SCANNER_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_rangescanner RangeScanner
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
sequence<float>        RangeSequence;

//! RangeScanner configuration data structure
class RangeScannerConfig extends OrcaObject
{
    //! minimum range step that can be sensed
    float rangeResolution;

    //! angle between successive returns
    float angleIncrement;

    //! Are we talking to the hardware?
    bool  isEnabled;
};


//! RangeScanner Geometry data structure
class RangeScannerGeometry extends OrcaObject
{
    //! Offset of the centre of the sensor from the robot, int the robot CS
    Frame3d offset;

    //! Dimensions of the sensor
    Size3d  size;
};

/*!
    Range scan data structure
*/
class RangeScannerData extends OrcaObject
{
    RangeSequence     ranges;
    float             startAngle;
    float             angleIncrement;
};

/*!
    @brief Consumer of range scans
*/
interface RangeScannerConsumer
{
    void setData( RangeScannerData obj );
};

/*!
    @brief Single-origin scanning range sensor
 */
interface RangeScanner
{
    //! Returns the latest data.
    //! @note In Orca1 this would be called ClientPull_Supplier interface.
    nonmutating RangeScannerData      getData()
            throws HardwareFailedException;
            
    //! Returns the current configuration.
    nonmutating RangeScannerConfig    getConfig();
    
    //! Returns the current geometry information.
    nonmutating RangeScannerGeometry  getGeometry();

    //! Set the configuration of the sensor.
    //! Throws remote exceptions if given a configuration it can't implement.
    idempotent  void setConfig( RangeScannerConfig config ) throws ConfigurationNotExistException;

    /*!
     *
     * Mimics IceStorm's subscribe().  The implementation may choose to
     * implement the push directly or use IceStorm.  This choice is transparent to the subscriber.
     *
     * @param subscriber The subscriber's proxy.
     *
     * @see unsubscribe
     *
     */
    void subscribe( RangeScannerConsumer *subscriber )
            throws SubscriptionFailedException;

    // this is what IceStorm's subscribe function looks like.
    //void subscribe(QoS theQoS, Object* subscriber);

    /**
     *
     * Unsubscribe the given [subscriber].
     *
     * @param subscriber The proxy of an existing subscriber.
     *
     * @see subscribe
     *
    **/
    idempotent void unsubscribe( RangeScannerConsumer *subscriber );
};

/*! @} */
}; // module

#endif
