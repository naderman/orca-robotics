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

#include "component.h"
#include "imagehandler.h"

#include <orcaice/orcaice.h>
#include <iostream>

// implementations of Ice objects
#include "cameraconsumer_i.h"


using namespace std;
using namespace imageviewer;

Component::Component()
    : orcaice::Component( "ImageViewer", orcaice::HomeInterface  ),
      imageHandler_(0)
{
}

Component::~Component()
{
    // do not delete inputLoop_ or imageHandler_!!! They derive from Ice::Thread and deletes itself.
}

// NOTE: this function returns after it's done, all variables that need to be permanent must
//       be declared as member variables.
void Component::start()
{

    //
    // REQUIRED INTERFACE: Camera
    //

    // Connect directly to the interface
    orca::CameraPrx cameraPrx;
    // TODO: this will not actually quit on ctrl-c
    while( true ) // isActive() )
    {
        try
        {
            orcaice::connectToInterfaceWithTag<orca::CameraPrx>( context(), cameraPrx, "Camera" );
            break;
        }
        catch ( const orcaice::NetworkException & e )
        {
            tracer()->error( "failed to connect to remote object. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
        // NOTE: connectToInterfaceWithTag() can also throw ConfigFileException,
        //       but if this happens it's ok if we just quit.
    }
    
    // Get the geometry
    cout << "Camera Geometry: " << cameraPrx->getGeometry() << endl;

    // Get the configuration
    cout << "Camera Config:   " << cameraPrx->getConfig() << endl;

    // Get the data once
    try
    {
        cout << "TODO(imageviewer/component.cpp): Check that ImageServer has loaded data into the buffer before trying to get data" << endl;
        // workaround... if imageserver and imageviewer are being run in an icebox, the
        // imageviewer needs to wait until the imageserver has loaded data
        // into the buffer... this should check rather than waiting
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));

        tracer()->info( "Trying to get image info as a test" );
        tracer()->print( orcaice::toString( cameraPrx->getData() ) );
    }
    catch ( const orca::HardwareFailedException & e )
    {
        tracer()->error( "hardware failure reported when getting a scan. Will subscribe anyway." );
    }

    // create a callback object to recieve images
    Ice::ObjectPtr consumer = new CameraConsumerI( cameraDataBuffer_ );
    orca::CameraConsumerPrx callbackPrx =
        orcaice::createConsumerInterface<orca::CameraConsumerPrx>( context(), consumer );
    //
    // ENABLE NETWORK CONNECTIONS
    //
    // this may throw an exception which will be caught in Application
    // but will cause the app to exit
    activate();
    
    //
    // Subscribe for data
    //
    // will try forever until the user quits with ctrl-c
    // TODO: this will not actually quit on ctrl-c
    while ( true ) // ( isActive() )
    {
        try
        {
            cameraPrx->subscribe( callbackPrx );
            break;
        }
        catch ( const orca::SubscriptionFailedException & e )
        {
            tracer()->error( "failed to subscribe for data updates. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }
    
    imageHandler_ = new ImageHandler( cameraPrx,
                                      cameraDataBuffer_,
                                      context() );
    imageHandler_->start();
    
    // the rest is handled by the application/service
}

void Component::stop()
{
    tracer()->debug( "component is stopping...",5 );
    // make sure that the main loop was actually created

    if ( imageHandler_ ) {
        // Tell the main loop to stop
        imageHandler_->stop();
    
        // Then wait for it
        imageHandler_->getThreadControl().join();
    }

}
