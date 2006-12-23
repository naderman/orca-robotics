/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Ben Upcroft
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
    @ingroup interfaces
    @defgroup orca_interface_camera Camera
    @brief Monocular camera.

Remote access to images captured by a variety of monocular cameras.

    @{
*/

//! Specifies the format once it is decoded.
//! @TODO: is this list reasonable/exhaustive?
enum ImageFormat {
    ImageFormatModeNfi,
    ImageFormatModeGray,  
    ImageFormatModeRgb,   
    ImageFormatModeBgr,   
    ImageFormatModeYuv422,
    ImageFormatBayerBg,  
    ImageFormatBayerGb,   
    ImageFormatBayerRg,   
    ImageFormatBayerGr,
    ImageFormatDigiclopsStereo,
    ImageFormatDigiclopsRight,
    ImageFormatDigiclopsBoth
};

//! Specifies any encoding of the image. 
//! @TODO: Is this list reasonable/exhaustive?
enum ImageCompression { 
    ImageCompressionNone,
    ImageCompressionJpeg
};

//! Camera configuration objectd
struct CameraDescription {
    //! Time when data was measured.
    Time timeStamp;

    //! Image width [pixels]
    int   imageWidth; 

    //! Image height [pixels]
    int   imageHeight;

    //! Frame rate [frames/seconds]
    double frameRate; 

    //! Image format type
    ImageFormat format;

    //! Image compression type
    ImageCompression compression;

    //! Image size [bytes]
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

    //! Image width [pixels]
    int imageWidth;

    //! Image height [pixels]
    int imageHeight;

    //! Image format type.
    ImageFormat format;

    //! Image compression type.
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
    nonmutating CameraData      getData()
        throws HardwareFailedException;
            
    //! Returns the current configuration.
    nonmutating CameraDescription getDescription();

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
