/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_LASER_SCANNER_2D_INTERFACE_ICE
#define ORCA2_LASER_SCANNER_2D_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>
#include <orca/rangescanner2d.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_laserscanner2d Laser
    @brief Single-origin scanning range and intensity sensor

The laser interface provides access to a single-origin scanning range and intensity
sensor, such as a SICK laser range-finder. In terms of interface definition, Laser
extends RangeScanner by extending the data structure with return-intensity information.

Devices supporting the laser interface can be configured to scan at
different angles and resolutions.  As such, the data returned by the
laser interface can take different forms.  To make interpretation of the
data simple, the laser data packet contains some extra fields before the
actual range data.  These fields tell the client the starting and ending
angles of the scan, the angular resolution of the scan, and the number of
range readings included.  Scans proceed counterclockwise about the laser
(0 degrees is forward).  

    @{
*/

//! A sequence of return intensities.
sequence<byte> IntensitySequence;

/*!
    @brief %Laser scan data structure adds return-intensity information to rangescanner data.
*/
class LaserScanner2dData extends RangeScanner2dData
{
    //! Adds the intensities to the data already in RangeScanner2dData.
    //! - RangeSequence     ranges;
    //! - float             startAngle;
    //! - float             angleIncrement;
    IntensitySequence intensities;
};

/*!
    @brief Single-origin scanning range sensor

    An implementaion of this interface is understood to return LaserData instead of
    RangeScannerData when implementing getData operation inherited from @ref orca_interface_rangescanner.
 */
interface LaserScanner2d extends RangeScanner2d
{
};

/*! @} */
}; // module

#endif
