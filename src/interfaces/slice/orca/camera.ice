/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Tom Burdick, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_CAMERA_INTERFACE_ICE
#define ORCA2_CAMERA_INTERFACE_ICE

#include <orca/common.ice>
#include <orca/bros1.ice>
#include <orca/image.ice>

module orca
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_camera Camera
    @brief Single Camera System.

Remote access to images captured by a camera. The classes inherit those found in image.ice.

    @{
*/

//! Camera configuration objectd
class CameraDescription extends ImageDescription 
{
    //! Frame rate [frames/seconds]
    double frameRate; 

    //! Focal Length of Lens [mm]
    double focalLength;

    //! Offset of the sensor with respect to the robot, 
    //! in the robot local coordinate system.
    Frame3d offset; 

    //! Dimensions of the sensor
    Size2d  sensorSize;

    //! Dimensions of the case
    Size3d caseSize;
}; 

//!
//! Camera data structure: essentially an image
//!
class CameraData extends ImageData
{
};

//! Interface to the camera provider.
interface Camera extends Image
{
};

/*! @} */
}; //module
#endif
