/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include "component.h"
#include "mainloop.h"

// implementations of Ice objects
#include <orcaifaceimpl/cameraI.h>

// define flags for building drivers
#include "configimageserver.h"

#ifdef OPENCV_FOUND
#   include <orcaimagegrabber/cvgrabber.h>
#endif 
#ifdef DIGICLOPS_AND_TRICLOPS_FOUND
#   include <orcaimagegrabber/digiclopsgrabber.h>
#endif 

// Various bits of hardware we can drive
#include "fakedriver.h"
#include "monodriver.h"

namespace imageserver {

using namespace std;
using namespace orca;

Component::Component() : 
    orcaice::Component( "ImageServer" ),
    mainLoop_(0),
    hwDriver_(0),
    imageGrabber_(0)
{
}

Component::~Component()
{
    delete hwDriver_;
    delete imageGrabber_;
    
    // do not delete mainLoop_!!! They derive from Ice::Thread and self-destruct.
}

void
Component::start()
{
    Ice::PropertiesPtr prop = properties();
    std::string prefix = tag()+".Config.";

    //
    // DRIVER CONFIGURATION
    //

    // read config options
    Driver::Config desiredCfg;

    desiredCfg.imageWidth = orcaice::getPropertyAsIntWithDefault( prop, prefix+"ImageWidth", 0 );
    desiredCfg.imageHeight = orcaice::getPropertyAsIntWithDefault( prop, prefix+"ImageHeight", 0 );
    desiredCfg.frameRate = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"FrameRate", 0.0 );

    string format = orcaice::getPropertyWithDefault( prop, prefix+"Format", "ModeNfi" );
    // only need to specify these formats as opencv can automatically find the other formats
    if( format == "BayerBg" ) {
        desiredCfg.format = orca::ImageFormatBayerBg;
    }
    else if( format == "BayerGb" ) {
        desiredCfg.format = orca::ImageFormatBayerGb;
    }
    else if( format == "BayerRg" ) {
        desiredCfg.format = orca::ImageFormatBayerRg;
    }
    else if( format == "BayerGr" ) {
        desiredCfg.format = orca::ImageFormatBayerGr;
    }
    else if( format == "DigiclopsStereo" ) {
        desiredCfg.format = orca::ImageFormatDigiclopsStereo;
    }
    else if( format == "DigiclopsRight" ) {
        desiredCfg.format = orca::ImageFormatDigiclopsRight;
    }
    else if( format == "DigiclopsBoth" ) {
        desiredCfg.format = orca::ImageFormatDigiclopsBoth;
    }

    string compression =  orcaice::getPropertyWithDefault( prop, prefix+"Compression", "none" );
    if( compression == "none" ) {
        // compression hasn't been included yet
        desiredCfg.compression = orca::ImageCompressionNone;
    }    

    if ( !desiredCfg.validate() ) {
        tracer()->error( "Failed to validate camera configuration. "+desiredCfg.toString() );
        // this will kill this component
        throw orcaice::Exception( ERROR_INFO, "Failed to validate camera configuration" );
    }

    // if we have multiple cameras, this indicates which one we want to use
    int cameraIndex = orcaice::getPropertyAsIntWithDefault( prop, prefix+"CameraIndex", 0 );
    if ( cameraIndex<0  || cameraIndex>1 )
    {
        std::string errString = "Only two cameras are supported \n \t cameraIndex must equal 0 for a single camera or 1 for a second camera'";
        errString += prefix + "CameraIndex'";
        throw orcaice::Exception( ERROR_INFO, errString );
    }

    //
    // HARDWARE INTERFACES
    //
    std::string driverName = orcaice::getPropertyWithDefault( prop, prefix+"Driver", "fake" );

    if ( driverName == "fake" )
    {
        context().tracer()->debug( "Loading 'fake' driver",3);
        hwDriver_ = new FakeDriver( desiredCfg, context() );
        imageGrabber_ = 0;
    }
    else if ( driverName == "monoopencv" )
    {
#ifdef OPENCV_FOUND
        context().tracer()->debug( "Loading 'monoopencv' driver",3);
        // Use opencv implementation for a monocular camera...

        // Initialize Opencv ImageGrabber
        imageGrabber_ = new orcaimage::CvGrabber( cameraIndex, context() );

        hwDriver_ = new MonoDriver( imageGrabber_, desiredCfg, context() );
#else
        throw orcaice::Exception( ERROR_INFO, "Can't instantiate driver 'monoopencv' because it wasn't built!" );
#endif // OPENCV_FOUND
    }
    else if ( driverName == "digiclops" )
    {
#ifdef DIGICLOPS_AND_TRICLOPS_FOUND
        context().tracer()->debug( "Loading 'digiclops' driver",3);
        // Use digiclops/triclops implementation for a digiclops camera...

        // Initialize digiclops ImageGrabber
        imageGrabber_ = new orcaimage::DigiclopsGrabber( context() );

        hwDriver_ = new MonoDriver( imageGrabber_, desiredCfg, context() );
#else
        throw orcaice::Exception( ERROR_INFO, "Can't instantiate driver 'digiclops' because it wasn't built!" );
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

    //
    // Init driver
    //
    hwDriver_->init();

    // query driver for the actual configuration after initialization
    Driver::Config actualCfg = hwDriver_->config();

    
    //
    // SENSOR DESCRIPTION
    //

    orca::CameraDescription descr;
    descr.timeStamp = orcaice::getNow();

    // transfer internal sensor configs
    descr.imageWidth   = actualCfg.imageWidth;
    descr.imageHeight  = actualCfg.imageHeight;
    descr.frameRate    = actualCfg.frameRate;
    descr.format       = actualCfg.format;
    descr.compression  = actualCfg.compression;
    descr.imageSize    = actualCfg.imageSize;

    // offset from the robot coordinate system
    orcaice::setInit( descr.offset );
    descr.offset = orcaice::getPropertyAsFrame3dWithDefault( prop, prefix+"Offset", descr.offset );

    orcaice::setInit( descr.size );
    descr.size = orcaice::getPropertyAsSize3dWithDefault( prop, prefix+"Size", descr.size );

    //
    // EXTERNAL PROVIDED INTERFACE: Camera
    //

    // create servant for direct connections
    orcaifaceimpl::CameraI* cameraI = new orcaifaceimpl::CameraI( descr, "Camera", context() );
    // to register with the adapter, it's enough to have a generic pointer
    cameraObjPtr_ = cameraI;

    //
    // MAIN DRIVER LOOP
    //
    mainLoop_ = new MainLoop( *cameraI, hwDriver_, imageGrabber_, context() );
    mainLoop_->start();
}

void 
Component::stop()
{
    tracer()->debug("stopping component...",2);
    orcaice::Thread::stopAndJoin( mainLoop_ );
}

} // namespace
