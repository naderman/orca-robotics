/*
 *  Orca-Base: Components for robotics.
 *
 *  Copyright (C) 2004
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

#ifndef ORCA2_LASER_INTERFACE_ICE
#define ORCA2_LASER_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>
#include <orca/rangescanner.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_laser Laser
    @brief Single-origin scanning range and intensity sensor

The laser interface provides access to a single-origin scanning range and intensity
sensor, such as a SICK laser range-finder (e.g., @ref orca_component_sicklaser).

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
sequence<byte>         IntensitySequence;

/*!
    Laser scan data structure
*/
class LaserData extends RangeScannerData
{
    //! Simply adds the intensities to:
    //! - RangeSequence     ranges;
    //! - float             startAngle;
    //! - float             angleIncrement;
    IntensitySequence intensities;
};

/*!
    @brief Single-origin scanning range sensor
 */
interface Laser extends RangeScanner
{
};

/*! @} */
}; // module

#endif
