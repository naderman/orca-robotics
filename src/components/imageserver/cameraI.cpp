/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "cameraI.h"
#include <iostream>
#include "camerautil.h"
#include <orcaice/orcaice.h>

using namespace std;
using namespace orca;

namespace imageserver {

CameraI::CameraI( orca::CameraGeometryPtr geometry,
                orcaice::Context              context )
    : geometry_(geometry),
      context_(context)
{
    // Initialize the current config with something
    CameraConfigPtr currentConfig = new CameraConfig;
    currentConfig->imageWidth = 0;
    currentConfig->imageHeight  = 0;
    currentConfig->frameRate = 0.0;
    //! @TODO is this a standard format?
    currentConfig->format = MODERGB;
    currentConfig->compression = COMPRESSIONTYPENONE;
    currentConfig->isEnabled = false;
    currentConfigBuffer_.push( currentConfig );

    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<CameraConsumerPrx>
        ( context_, consumerPrx_, "Camera" );
}

orca::CameraDataPtr 
CameraI::getData(const Ice::Current& current) const
{
    std::cout << "getData()" << std::endl;

    // we don't need to pop the data here because we don't block on it.
    if ( dataBuffer_.isEmpty() )
    {
        // @todo what should happens if there's no data?
        cout << "ERROR(laser_i.cpp): getData() called when no data had been generated!!" << endl;
        cout << "ERROR(laser_i.cpp): Don't know what to do!" << endl;
        exit(EXIT_FAILURE);
    }

    // create a null pointer. data will be cloned into it.
    orca::CameraDataPtr data;
    dataBuffer_.get( data );

    return data;
}

// served out the data to the client (it was stored here earlier by the driver)
orca::CameraConfigPtr 
CameraI::getConfig(const Ice::Current& current) const
{
    std::cout << "getConfig()" << std::endl;

    CameraConfigPtr config;
    currentConfigBuffer_.get( config );

    return config;
}

// Get Camera Geometry
::orca::CameraGeometryPtr 
CameraI::getGeometry(const ::Ice::Current& ) const
{
    std::cout << "getGeometry()" << std::endl;
    return geometry_;
}

void 
CameraI::setConfig(const ::orca::CameraConfigPtr &config, const ::Ice::Current& )
{
    cout<<"setConfig()" << endl;

    if ( camerautil::cameraCanDoConfig( config ) )
    {
        desiredConfigBuffer_.push( config );
    }
    else
    {
        throw orca::ConfigurationNotExistException( "the camera cannot implement this configuration request." );
    }
}

// Subscribe people
void 
CameraI::subscribe(const ::orca::CameraConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout<<"subscribe()"<<endl;
    IceStorm::QoS qos;
    topicPrx_->subscribe( qos, subscriber );
}

// Unsubscribe people
void 
CameraI::unsubscribe(const ::orca::CameraConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout<<"unsubscribe()"<<endl;
    topicPrx_->unsubscribe( subscriber );
}

void
CameraI::localSetData( const ::orca::CameraDataPtr data )
{
    dataBuffer_.push( data );

    // Try to push it out to IceStorm too
    try {
        consumerPrx_->setData( data );
    }
    catch ( Ice::ConnectionRefusedException &e )
    {
        // This could happen if IceStorm dies.
        // If we're running in an IceBox and the IceBox is shutting down, 
        // this is expected (our co-located IceStorm is obviously going down).
        context_.tracer()->warning( "Failed push to IceStorm." );
    }
}

}
