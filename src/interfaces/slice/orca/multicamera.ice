/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Tom Burdick, Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_MULTICAMERA_INTERFACE_ICE
#define ORCA2_MULTICAMERA_INTERFACE_ICE

#include <orca/common.ice>
#include <orca/bros1.ice>
#include <orca/camera.ice>

module orca
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_multicamera MultiCamera
    @brief Multiple Camera System.

Atomic remote access to images captured by multiple cameras.  The classes inherit those found in camera.ice.

    @{
*/

//! A sequence of Camera Descrptions
sequence<CameraDescription> CameraDescriptionSeq;

//! MultiCamera configuration object
class MultiCameraDescription extends CameraDescription 
{
    CameraDescriptionSeq extraDescriptions;
}; 

//! A sequence of Camera Data objects
sequence<CameraData> CameraDataSeq;

//!
//! Camera data structure: essentially an image
//!
class MultiCameraData extends CameraData
{
    CameraDataSeq extraData;
};


//! Interface to the camera
interface MultiCamera extends Camera
{
};


//!  //@}
}; // module

#endif
