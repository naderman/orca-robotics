/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_CAMERA_INTERFACE_ICE
#define ORCA2_CAMERA_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>
#include <orca/image.ice>

module orca
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_camera Camera
    @brief Single or Multiple Camera System.

Remote access to images captured by a variety of cameras either individually or as a group. All data is sent in a sequence, or vector of classes. The classes inherit those found in image.ice.

    @{
*/

//! Camera configuration objectd
class CameraDescription extends ImageDescription {
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

sequence<CameraData> CameraDataSequence;
sequence<CameraDescription> CameraDescriptionSequence;

//!
//! Data consumer interface
//!
interface CameraConsumer
{
    //! Transmits the data to the consumer.
    void setData( CameraDataSequence obj );
};

//! Interface to the camera
interface Camera
{
    //! Returns the latest data.
    ["cpp:const"] idempotent CameraDataSequence getData()
        throws HardwareFailedException;
            
    //! Returns the current configuration.
    ["cpp:const"] idempotent CameraDescriptionSequence getDescription();

    /*!
     * Mimics IceStorm's subscribe(). @p subscriber is typically a direct proxy to the consumer object.
     * The implementation may choose to implement the push directly or use IceStorm.
     * This choice is transparent to the subscriber. The case when the @p subscriber is already subscribed
     * is quietly ignored.
     *
     * @see unsubscribe
     */
    void subscribe( CameraConsumer *subscriber )
            throws SubscriptionFailedException;

    /*!
     * Unsubscribe an existing @p subscriber. The case when the @p subscriber is not subscribed
     * is quietly ignored.
     *
     * @see subscribe
     */
    idempotent void unsubscribe( CameraConsumer *subscriber );
};


//!  //@}
}; // module

#endif
