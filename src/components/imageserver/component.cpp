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
#include "mainloop.h"

// Various bits of hardware we can drive
#include "driver.h"
#include "fakedriver.h"
#include "monodriver.h"

// implementations of Ice objects
#include "cameraI.h"

#include "imagegrabber/cvgrabber.h"
//#include <libcamera/camera.hpp>

#include <orcaice/orcaice.h>

namespace imageserver {

using namespace std;
using namespace orca;

Component::Component()
    : orcaice::Component( "ImageServer" ),
      mainLoop_(0)
{
}

Component::~Component()
{
    delete hwDriver_;
    
    // do not delete mainLoop_!!! They derive from Ice::Thread and self-destruct.
}

void
Component::start()
{
    //
    // INITIAL CONFIGURATION
    //

    Ice::PropertiesPtr prop = communicator()->getProperties();
    std::string prefix = tag();
    prefix += ".Config.";

    // Camera geometry
    orca::CameraGeometryPtr geometry = new orca::CameraGeometry;
    geometry->offset.p.x = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Offset.p.x", 0.0 );
    geometry->offset.p.y = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Offset.p.y", 0.0 );
    geometry->offset.p.z = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Offset.p.z", 0.0 );
    geometry->offset.o.r = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Offset.o.r", 0.0 )*DEG2RAD_RATIO;
    geometry->offset.o.p = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Offset.o.p", 0.0 )*DEG2RAD_RATIO;
    geometry->offset.o.y = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Offset.o.y", 0.0 )*DEG2RAD_RATIO;

//     geometry->size.l = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Size.l", 0.0 );
//     geometry->size.w = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Size.w", 0.0 );
//     geometry->size.h = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Size.h", 0.0 );


    // EXTERNAL PROVIDED INTERFACE: Camera

    // create servant for direct connections
    // don't need to store it as a member variable, adapter will keep it alive
    CameraI *cameraObj = new CameraI( geometry,
                                   context() );
    Ice::ObjectPtr cameraObjPtr = cameraObj;
    orcaice::createInterfaceWithTag( context(), cameraObjPtr, "Camera" );


    bool startEnabled = orcaice::getPropertyAsIntWithDefault( prop, prefix+"StartEnabled", true );

    orca::CameraConfigPtr   cameraConfig = new CameraConfig;

    cameraConfig->imageWidth   = 
        orcaice::getPropertyAsIntWithDefault( prop, prefix+"ImageWidth", 0 );

    cameraConfig->imageHeight = 
        orcaice::getPropertyAsIntWithDefault( prop, prefix+"ImageHeight", 0 );

    cameraConfig->frameRate = 
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"FrameRate", 1.0 );

//     cameraConfig->format = 
//         orcaice::getPropertyAsIntWithDefault( prop, prefix+"Format", MODEBGR );

//     cameraConfig->compression = 
//         orcaice::getPropertyAsIntWithDefault( prop, prefix+"Compression", COMPRESSIONTYPENONE );

    // This is the initial config if someone reads before we can enable
    cameraConfig->isEnabled         = false;
    cameraObj->localSetCurrentConfig( cameraConfig );

    // This is what we'll immediately try to configure
    cameraConfig->isEnabled         = startEnabled;
    cameraObj->localSetDesiredConfig( cameraConfig );

    std::string driverName;
    int ret = orcaice::getProperty( prop, prefix+"Driver", driverName );
    if ( ret != 0 )
    {
        std::string errString = "Couldn't determine camera type.  Expected property '";
        errString += prefix + "Driver'";
        throw orcaice::Exception( ERROR_INFO, errString );
    }

    //
    // HARDWARE INTERFACES
    //

    if ( driverName == "fake" )
    {
        hwDriver_ = new FakeDriver;
    }

    else if ( driverName == "monofirewire" )
    {
        cout << "TODO(component.cpp): Still need to add in a config file." << endl;
        // this is the stuff that worked with libcamera...
        // std::string imageGrabberConfigFile    = orcaice::getPropertyWithDefault(      prop, prefix+"Camera.ImageGrabberConfigFile",    "fw.cfg" );
        // ImageGrabber* imageGrabber = cfg.getImageGrabber();
        // ImageGrabberConfig cfg(imageGrabberConfigFile.c_str());


        // Initialize ImageGrabber
        // Use opencv implementation...
        string grabberI = "opencv";
        if ( grabberI == "opencv"  )
        {
            CvGrabber* cvGrabber = new CvGrabber();

            cout<<"ImageServer: using CvGrabber: " << endl;
            hwDriver_ = new MonoFireWireDriver( cvGrabber, context() );
        }

    }

    else
    {
        std::string errString = "unknown camera type: "+driverName;
        context().tracer()->error( errString );
        throw orcaice::Exception( ERROR_INFO, errString );
        return;
    }
    tracer()->debug( "loaded '"+driverName+"' driver", 2 );
    
    ////////////////////////////////////////////////////////////////////////////////

    //
    // ENABLE NETWORK CONNECTIONS
    //
    activate();

    tracer()->debug( "entering main loop...",5 );

    //
    // MAIN DRIVER LOOP
    //

    mainLoop_ = new MainLoop( *cameraObj,
                               hwDriver_,
                               context(),
                               startEnabled );
    
    mainLoop_->start();    
}

void Component::stop()
{
    tracer()->debug("stopping component Component...",5);

    // make sure that the main loop was actually created
    if ( mainLoop_ ) {
        // Tell the main loop to stop
        mainLoop_->stop();
    
        IceUtil::ThreadControl tc = mainLoop_->getThreadControl();
    
        // Then wait for it
        tc.join();
    }
}

}
