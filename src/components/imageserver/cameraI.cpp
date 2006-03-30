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
