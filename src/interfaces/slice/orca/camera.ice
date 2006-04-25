/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
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
    MODENFI,
    MODEGRAY,  
    MODERGB,   
    MODEBGR,   
    MODEYUV422,
    BAYERBG,  
    BAYERGB,   
    BAYERRG,   
    BAYERGR,
    DIGICLOPSSTEREO,
    DIGICLOPSRIGHT,
    DIGICLOPSBOTH};

//! Specifies any encoding of the image. 
//! @TODO: Is this list reasonable/exhaustive?
enum ImageCompression { 
    COMPRESSIONTYPENONE,
    COMPRESSIONTYPEJPEG};

//! Camera configuration objectd
class CameraConfig extends OrcaObject {
    //! Image width in pixels
    int   imageWidth; 
    //! Image height in pixels
    int   imageHeight;
    //! Frame rate in fps
    float frameRate; 
    //! Image Format
    ImageFormat format;
    //! Image compression type
    ImageCompression compression;
    //! Are we talking to the hardware?
    bool  isEnabled;
}; 

//!
//! Geometric info
//!
class CameraGeometry extends OrcaObject 
{
    //! Offset of the camera with respect to the platform
    Frame3d offset; 
}; 

//!
//! Camera data structure: an image
//!
class CameraData extends OrcaObject
{
    //! Image width in pixels
    int imageWidth;
    //! Image height in pixels
    int imageHeight;
    //! Image format type.
    ImageFormat format;
    //! Image compression type.
    ImageCompression compression;
    //! The image data itself.  The structure of this byte sequence
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
    nonmutating CameraConfig    getConfig();
    
    //! Returns the current geometry information.
    nonmutating CameraGeometry  getGeometry();

    //! Set the configuration of the sensor.
    //! Throws remote exceptions if given a configuration it can't implement.
    idempotent  void setConfig( CameraConfig config ) 
        throws ConfigurationNotExistException;

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
