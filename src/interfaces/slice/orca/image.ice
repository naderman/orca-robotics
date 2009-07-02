/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_IMAGE_INTERFACE_ICE
#define ORCA2_IMAGE_INTERFACE_ICE

#include <orca/common.ice>
#include <IceStorm/IceStorm.ice>

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

//! Static description of the image source.
class ImageDescription 
{
    //! %Image width [pixels]
    int width; 

    //! %Image height [pixels]
    int height;

    //! %Image size [bytes]
    int size;

    //! %Image format string
    string format;
}; 

//! A single image.
class ImageData
{ 
    //! TimeStamp of Data
    Time timeStamp;   

    //! ImageDescription of Data, for convienence purposes
    ImageDescription description;

    //! The image data itself. The structure of this byte sequence
    //! depends on the image format and compression.
    ByteSeq pixelData;
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
    //! Returns the image source description.
    idempotent ImageDescription getDescription();

    //! Returns the latest data.
    idempotent ImageData getData()
        throws DataNotExistException, HardwareFailedException;            

    //! Tries to subscribe the specified subscriber for data updates.
    //! If successfuly, returns a proxy to the IceStorm topic which can be later used by the 
    //! client to unsubscribe itself. For reference, the Slice definition of the Topic
    //! interface for unsubscribing:
    //! @verbatim
    //! idempotent void unsubscribe(Object* subscriber);
    //! @endverbatim
    IceStorm::Topic* subscribe( ImageConsumer* subscriber )
        throws SubscriptionFailedException, SubscriptionPushFailedException;
};


//!  //@}
}; // module

#endif
