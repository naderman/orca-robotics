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

module orca
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_camera Camera
    @brief Monocular camera.

Remote access to images captured by a variety of monocular cameras.

    @{
*/

//! Specifies the format once it is decoded.
//! @TODO: is this list reasonable/exhaustive?
enum ImageFormat {
    //! Not Quite Sure
    ImageFormatModeNfi,
    //! Gray scale
    ImageFormatModeGray,
    //! Red-Green-Blue  
    ImageFormatModeRgb,
    //! Blue-Green-Red   
    ImageFormatModeBgr,
    //! YUV422   
    ImageFormatModeYuv422,
    //! Bayer Blue-Green
    ImageFormatBayerBg, 
    //! Bayer Green-Blue 
    ImageFormatBayerGb, 
    //! Bayer Red-Green  
    ImageFormatBayerRg,
    //! Bayer Green-Red   
    ImageFormatBayerGr,
    //! Digiclops Stereo
    ImageFormatDigiclopsStereo,
    //! Digiclops Right
    ImageFormatDigiclopsRight,
    //! Digiclops Both
    ImageFormatDigiclopsBoth
};

//! Specifies any encoding of the image. 
//! @TODO: Is this list reasonable/exhaustive?
enum ImageCompression { 
    //! None
    ImageCompressionNone,
    //! JPEG
    ImageCompressionJpeg
};

//! Camera configuration objectd
struct CameraDescription {
    //! Time when data was measured.
    Time timeStamp;

    //! %Image width [pixels]
    int   imageWidth; 

    //! %Image height [pixels]
    int   imageHeight;

    //! Frame rate [frames/seconds]
    double frameRate; 

    //! %Image format type
    ImageFormat format;

    //! %Image compression type
    ImageCompression compression;

    //! %Image size [bytes]
    int imageSize;

    //! Offset of the sensor with respect to the robot, 
    //! in the robot local coordinate system.
    Frame3d offset; 

    //! Dimensions of the sensor
    Size3d  size;
}; 

//!
//! Camera data structure: an image
//!
struct CameraData
{
    //! Time when data was measured.
    Time timeStamp;

    //! %Image width [pixels]
    int imageWidth;

    //! %Image height [pixels]
    int imageHeight;

    //! %Image format type.
    ImageFormat format;

    //! %Image compression type.
    ImageCompression compression;

    //! The image data itself. The structure of this byte sequence
    //! depends on the image format and compression.
    ByteSequence image;
};

//!
//! Data consumer interface
//!
interface CameraConsumer
{
    //! Transmits the data to the consumer.
    void setData( CameraData obj );
};

//! Interface to the camera
interface Camera
{
    //! Returns the latest data.
    ["cpp:const"] idempotent CameraData getData()
        throws HardwareFailedException;
            
    //! Returns the current configuration.
    ["cpp:const"] idempotent CameraDescription getDescription();

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
