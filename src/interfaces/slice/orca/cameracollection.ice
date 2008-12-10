/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Tom Burdick, Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_CAMERACOLLECTION_INTERFACE_ICE
#define ORCA2_CAMERACOLLECTION_INTERFACE_ICE

#include <orca/common.ice>
#include <orca/bros1.ice>
#include <orca/camera.ice>

module orca
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_cameracollection CameraCollection
    @brief Multiple Camera System.

Atomic remote access to images captured by multiple cameras.  The classes inherit those found in camera.ice.

    @{
*/

//! A sequence of Camera Descrptions
sequence<CameraDescription> CameraDescriptionSeq;

//! MultiCamera configuration object
class CameraCollectionDescription 
{
    CameraDescriptionSeq descriptions;
}; 

//! A sequence of Camera Data objects
sequence<CameraData> CameraDataSeq;

//!
//! Camera Collection data structure: essentially an image
//!
class CameraCollectionData 
{
    CameraDataSeq data;
};

//! Interface to the image consumer.
interface CameraCollectionConsumer
{
    //! Transmits the data to the consumer.
    void setData( CameraCollectionData obj );
};

//! Interface to the camera
interface CameraCollection
{
    //! Returns the image source description.
    idempotent CameraCollectionDescription getDescription();

    //! Returns the latest data.
    idempotent CameraCollectionData getData()
        throws DataNotExistException, HardwareFailedException;            

    //! Tries to subscribe the specified subscriber for data updates.
    //! If successfuly, returns a proxy to the IceStorm topic which can be later used by the 
    //! client to unsubscribe itself. For reference, the Slice definition of the Topic
    //! interface for unsubscribing:
    //! @verbatim
    //! idempotent void unsubscribe(Object* subscriber);
    //! @endverbatim
    IceStorm::Topic* subscribe( CameraCollectionConsumer* subscriber )
        throws SubscriptionFailedException;

};


//!  //@}
}; // module

#endif
