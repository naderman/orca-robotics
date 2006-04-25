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

// define flags for building drivers
#include "configimageserver.h"

#ifdef OPENCV7_FOUND
#   include "imagegrabber/cvgrabber.h"
#endif 
#ifdef DIGICLOPS_AND_TRICLOPS_FOUND
#   include "imagegrabber/digiclopsgrabber.h"
#endif 

// implementations of Ice objects
#include "cameraI.h"

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
    // delete imageGrabber if it has been created (only fakedriver doesn't initialise this)
    if ( imageGrabber_ != NULL )
    {
        delete imageGrabber_;
    }
    
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


    // EXTERNAL PROVIDED INTERFACE: Camera

    // create servant for direct connections
    CameraI *cameraObj = new CameraI( geometry,
                                   context() );
    cameraObjPtr_ = cameraObj;
    orcaice::createInterfaceWithTag( context(), cameraObjPtr_, "Camera" );


    bool startEnabled = orcaice::getPropertyAsIntWithDefault( prop, prefix+"StartEnabled", true );

    orca::CameraConfigPtr   cameraConfig = new CameraConfig;

    cameraConfig->imageWidth   = 
        orcaice::getPropertyAsIntWithDefault( prop, prefix+"ImageWidth", 0 );

    cameraConfig->imageHeight = 
        orcaice::getPropertyAsIntWithDefault( prop, prefix+"ImageHeight", 0 );

    cameraConfig->frameRate = 
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"FrameRate", 0.0 );

    string format = 
        orcaice::getPropertyWithDefault( prop, prefix+"Format", "MODEDNFI" );
    // only need to specify these formats as opencv can automatically find the other formats
    if( format == "BAYERBG" )
    {
        cameraConfig->format = BAYERBG;
    }
    else if( format == "BAYERGB" )
    {
        cameraConfig->format = BAYERGB;
    }
    else if( format == "BAYERRG" )
    {
        cameraConfig->format = BAYERRG;
    }
    else if( format == "BAYERGR" )
    {
        cameraConfig->format = BAYERGR;
    }
    else if( format == "TRICLOPSRGB" )
    {
        cameraConfig->format = TRICLOPSRGB;
    }
    else if( format == "TRICLOPSPACKED" )
    {
        cameraConfig->format = TRICLOPSPACKED;
    }

    string compression = 
        orcaice::getPropertyWithDefault( prop, prefix+"Compression", "NONE" );
    if( compression == "NONE" )
    {
        // compression hasn't been included yet
        cameraConfig->compression = COMPRESSIONTYPENONE;
    }    

    // This is the initial config if someone reads before we can enable
    cameraConfig->isEnabled         = false;
    cameraObj->localSetCurrentConfig( cameraConfig );

    // This is what we'll immediately try to configure
    cameraConfig->isEnabled         = startEnabled;
    cameraObj->localSetDesiredConfig( cameraConfig );

    // if we have multiple cameras, this indicates which one we want to use
//     int cameraIndex = orcaice::getPropertyAsIntWithDefault( prop, prefix+"CameraIndex", 0 );

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
        imageGrabber_ = NULL;
    }

    else if ( driverName == "monoopencv" )
    {
#ifdef OPENCV7_FOUND
        // Use opencv implementation for a monocular camera...

        // Initialize Opencv ImageGrabber
//        imageGrabber_ = new CvGrabber( cameraIndex );
        imageGrabber_ = new CvGrabber();

        hwDriver_ = new MonoDriver( imageGrabber_, context() );

        cout<<"ImageServer: using opencv image grabber - CvGrabber for a monocular camera" << endl;
#endif // OPENCV7_FOUND
    }
    else if ( driverName == "digiclops" )
    {
#ifdef DIGICLOPS_AND_TRICLOPS_FOUND
        // Use digiclops/triclops implementation for a digiclops camera...

        // Initialize digiclops ImageGrabber
//        imageGrabber_ = new DigiclopsGrabber( cameraIndex );
        imageGrabber_ = new DigiclopsGrabber();

        hwDriver_ = new MonoDriver( imageGrabber_, context() );

        cout<<"ImageServer: using digiclops image grabber for a digiclops camera" << endl;
#endif // DIGICLOPS_AND_TRICLOPS_FOUND
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
                              imageGrabber_,
                              startEnabled,
                              driverName );
    
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
