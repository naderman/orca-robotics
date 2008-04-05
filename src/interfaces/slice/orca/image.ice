/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_IMAGE_INTERFACE_ICE
#define ORCA2_IMAGE_INTERFACE_ICE

#include <orca/orca.ice>

module orca
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_image Image
    @brief Represents an image source.

This and several other interfaces -- @ref orca_interface_camera, @ref orca_interface_pixmap 
-- are related. They could probably be merged into one or at least refactored.

    @{
*/

//! Specifies the format once it is decoded.
//! @TODO: is this list reasonable/exhaustive?
//! @TODO: is digiclops a special format?
enum ImageFormat {
    //! Not Quite Sure
    ImageFormatModeNfi,
    //! Binary
    ImageFormatModeBinary,
    //! Gray scale
    ImageFormatModeGray,
    //! Red-Green-Blue  
    ImageFormatModeRGB8,
    //! Red-Green-Blue-Alpha  
    ImageFormatModeRGBA8,
    //! Alpha-Red-Green-Blue  
    ImageFormatModeARGB8,
    //! Blue-Green-Red   
    ImageFormatModeBGR8,
    //! Blue-Green-Red-Alpha   
    ImageFormatModeBGRA8,
    //! YUV422   
    ImageFormatModeYUV422,
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
    ImageFormatDigiclopsBoth,
    //! Last Enum for iteration purposes only
    ImageFormatEnumSize
};

//! Specifies any encoding of the image. 
//! @TODO: Is this list reasonable/exhaustive?
enum ImageCompression { 
    //! None
    ImageCompressionNone,
    //! JPEG
    ImageCompressionJpeg,
    //! PNG
    ImageCompressionPng
};


//! Static description of the image source.
class ImageDescription 
{
    //! %Image width [pixels]
    int imageWidth; 

    //! %Image height [pixels]
    int imageHeight;

    //! %Image format type
    ImageFormat format;

    //! %Image compression type
    ImageCompression compression;

}; 

//! A single image.
class ImageData
{
    //! TimeStamp of Data
    Time timeStamp;    


    //! The image data itself. The structure of this byte sequence
    //! depends on the image format and compression.
    ByteSequence data;
};

//! Interface to the image consumer.
interface ImageConsumer
{
    //! Transmits the data to the consumer.
    void setData( ImageData obj );
};

//! Interface to the image provider.
interface Image
{
    //! Returns the latest data.
    idempotent ImageData getData()
        throws HardwareFailedException;
            
    //! Returns the image source description.
    idempotent ImageDescription getDescription();

    /*!
     * Mimics IceStorm's subscribe(). @p subscriber is typically a direct proxy to the consumer object.
     * The implementation may choose to implement the push directly or use IceStorm.
     * This choice is transparent to the subscriber. The case when the @p subscriber is already subscribed
     * is quietly ignored.
     *
     * @see unsubscribe
     */
    void subscribe( ImageConsumer* subscriber )
            throws SubscriptionFailedException;

    /*!
     * Unsubscribe an existing @p subscriber. The case when the @p subscriber is not subscribed
     * is quietly ignored.
     *
     * @see subscribe
     */
    idempotent void unsubscribe( ImageConsumer* subscriber );
};


//!  //@}
}; // module

#endif
