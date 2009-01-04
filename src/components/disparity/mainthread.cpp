/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2008 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 * 
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "mainthread.h"

#include <iostream>
#include <memory>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h> // for getPropertyAs...()

namespace disparity
{

MainThread::MainThread( const orcaice::Context &context )
    : orcaice::SubsystemThread( context.tracer(), context.status(), "MainThread" )
    , context_(context)
    , outgoingData_(0)
    , outgoingDescr_(new orca::ImageDescription())
{
    init();
}
void 
MainThread::initialise()
{
}

void
MainThread::work() 
{
    const int timeoutMs = 500;
    
    subStatus().setMaxHeartbeatInterval( 1.0 );

    while( !isStopping() )
    {
        try
        {
            //read data in
            int ret = incomingInterface_->buffer().getAndPopNext( incomingData_, timeoutMs );

            if( !ret )
            {
                context_.tracer().debug("Successfuly fetched Images", 6);
               
                // ensure we recieved 2 images
                if( incomingData_->cameraDataVector.size() != 2 )
                {
                    throw gbxutilacfr::Exception( ERROR_INFO, "Recieved incorrect number of images, disparity expects 2 images, no more no less" );
                }

                // check descriptions, otherwise update our copy and re-init the data structures
/*               if(  incomingData_->cameraDataVector[0]->description != incomingDescr_->descriptions[0] 
                  || incomingData_->cameraDataVector[1]->description != incomingDescr_->descriptions[1] )
                {
                    incomingDescr_->descriptions[0] = orca::CameraDescriptionPtr::dynamicCast(incomingData_->cameraDataVector[0]->description);
                    incomingDescr_->descriptions[1] = orca::CameraDescriptionPtr::dynamicCast(incomingData_->cameraDataVector[1]->description);
                    initDataStructures();
                }
*/
                //set the pointers of left and right image data            
                pluginLeftData_.pixelData = &(incomingData_->cameraDataVector[0]->pixelData[0]);
                pluginRightData_.pixelData = &(incomingData_->cameraDataVector[1]->pixelData[0]);

                //process it
                pluginInterface_->process( pluginLeftData_, pluginRightData_, pluginOutputData_ );
                
                //push processed data out
                outgoingInterface_->localSetAndSend( outgoingData_ );

                //state is good, so say so
                subStatus().ok();
            }
            else
            {
                subStatus().warning( "Images were not successfully copied to local buffer" );
            }
        }
        catch( ... )
        {
        }
    }
}

void
MainThread::finalise() 
{
}

////////////////////

bool
MainThread::hasPluginEventLoop() const
{
    return pluginInterface_->hasEventLoop();
}

void
MainThread::executePluginEventLoop()
{
    pluginInterface_->executeEventLoop();
}

void
MainThread::init()
{

    readSettings();

    initPluginInterface();

    initNetworkInterface();

    initDataStructures();
}

void
MainThread::readSettings()
{
    // read in settings from a config file
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";
   
    //connect with a proxy and fetch the incoming description object
    orcaice::connectToInterfaceWithTag<orca::MultiCameraPrx>( context_
            , incomingPrx_
            , "MultiCamera"
            , this
            , subsysName()
            );

    incomingDescr_ = incomingPrx_->getDescription();

    context_.tracer().info( orcaobj::toString( incomingDescr_ ) );

    pluginConfig_.width  = incomingDescr_->descriptions[0]->width;
    pluginConfig_.height = incomingDescr_->descriptions[0]->height;
    pluginConfig_.size   = incomingDescr_->descriptions[0]->width * incomingDescr_->descriptions[0]->height;
    pluginConfig_.format = incomingDescr_->descriptions[0]->format;
    pluginConfig_.shifts = orcaice::getPropertyAsIntWithDefault( prop, prefix+"Shifts", 16 ); 
    pluginConfig_.offset = orcaice::getPropertyAsIntWithDefault( prop, prefix+"Offset", 0 );
    
}

void 
MainThread::initPluginInterface()
{
    subStatus().setMaxHeartbeatInterval( 20.0 );

    //copy from description to config

    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    // Dynamically load the library and find the factory
    std::string pluginLibName = 
        orcaice::getPropertyWithDefault( prop, prefix+"DriverLib", "libHydroDisparitySimple.so" );
    context_.tracer().info( "MainThread: Loading driver library "+pluginLibName  );

    // The factory which creates the driver
    std::auto_ptr<hydrointerfaces::DisparityFactory> pluginFactory;
    try {
        pluginLib_.reset( new hydrodll::DynamicallyLoadedLibrary(pluginLibName) );
        pluginFactory.reset( 
            hydrodll::dynamicallyLoadClass<hydrointerfaces::DisparityFactory,DriverFactoryMakerFunc>
            ( *pluginLib_, "createDriverFactory" ) );
    }
    catch (hydrodll::DynamicLoadException &e)
    {
        // unrecoverable error
        context_.shutdown(); 
        throw;
    }

    // create the driver
    while ( !isStopping() )
    {
        std::stringstream exceptionSS;
        try {
            context_.tracer().info( "HwThread: Creating driver..." );
            pluginInterface_.reset(0);
            pluginInterface_.reset( pluginFactory->createDriver( pluginConfig_, context_.toHydroContext() ) );
            break;
        }
        catch ( IceUtil::Exception &e ) {
            exceptionSS << "MainThread: Caught exception while creating driver: " << e;
        }
        catch ( std::exception &e ) {
            exceptionSS << "MainThread: Caught exception while initialising driver: " << e.what();
        }
        catch ( char *e ) {
            exceptionSS << "MainThread: Caught exception while initialising driver: " << e;
        }
        catch ( std::string &e ) {
            exceptionSS << "MainThread: Caught exception while initialising driver: " << e;
        }
        catch ( ... ) {
            exceptionSS << "MainThread: Caught unknown exception while initialising driver";
        }

        // we get here only after an exception was caught
        context_.tracer().error( exceptionSS.str() );
        subStatus().fault( exceptionSS.str() );          

        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));        
    }

    // copy the config structure to the outgoing description structure
    outgoingDescr_->width = pluginConfig_.width;
    outgoingDescr_->height = pluginConfig_.height;
    outgoingDescr_->format = "GRAY8";
    outgoingDescr_->size = pluginConfig_.width*pluginConfig_.height;

    subStatus().setMaxHeartbeatInterval( 1.0 );

}

void
MainThread::initNetworkInterface()
{
    activate( context_, this, subsysName() );

    // incoming network interface
    incomingInterface_ = new orcaifaceimpl::BufferedMultiCameraConsumerImpl( 1
            , gbxiceutilacfr::BufferTypeCircular
            , context_ 
            );

    incomingInterface_->subscribeWithTag( "MultiCamera"
            , this
            , subsysName() 
            );

    // outgoing network interface
    outgoingInterface_ = new orcaifaceimpl::ImageImpl( outgoingDescr_
        , "Image"
        , context_ 
        );

    outgoingInterface_->initInterface( this, subsysName() );

}

void
MainThread::initDataStructures()
{
    outgoingData_ = new orca::ImageData();
    outgoingData_->description = outgoingDescr_;

    //setup data structures
    outgoingData_->pixelData.resize(outgoingDescr_->size);
    pluginOutputData_.pixelData = &(outgoingData_->pixelData[0]);

}

} // namespace

