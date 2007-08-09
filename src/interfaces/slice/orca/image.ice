/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_IMAGE_INTERFACE_ICE
#define ORCA2_IMAGE_INTERFACE_ICE

#include <orca/orca.ice>
// we are using a couple of enums from Camera interface
#include <orca/camera.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_image Image
    @brief Represents an image source.

This and several other interfaces -- @ref orca_interface_camera, @ref orca_interface_pixmap 
-- are related. They could probably be merged into one or at least refactored.

    @{
*/

//! Static description of the image source.
struct ImageDescription 
{
    //! Image width [pixels]
    int imageWidth; 

    //! Image height [pixels]
    int imageHeight;

    //! Image format type
    ImageFormat format;

    //! Image compression type
    ImageCompression compression;
}; 

//! A single image.
class ImageData
{
    //! Time when data was measured.
    Time timeStamp;

    //! Image width [pixels]
    int width; 

    //! Image height [pixels]
    int height;

    //! Image format type.
    ImageFormat format;

    //! Image compression type.
    ImageCompression compression;

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
