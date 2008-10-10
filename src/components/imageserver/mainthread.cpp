/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include "mainthread.h"

using namespace std;
using namespace imageserver;

MainThread::MainThread( const orcaice::Context &context ) 
: orcaimagecommon::ImageComponentThread( context )
, orcaImageDescr_(new orca::ImageDescription())
{
}

void
MainThread::initNetworkInterface()
{
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    //
    // SENSOR DESCRIPTION
    //
    
    //transfer internal sensor configs
    orcaImageDescr_->width = config_.width;
    orcaImageDescr_->height = config_.height;
    orcaImageDescr_->format = config_.format;

    //
    // EXTERNAL PROVIDED INTERFACE
    //
    
    //create new interface
    imageInterface_ = new orcaifaceimpl::ImageImpl( orcaImageDescr_
                                                  , "Image"
                                                  , context_ );

    //initialize the interface
    imageInterface_->initInterface( this, subsysName() );

}

void
MainThread::readData()
{
    //
    // Read from the image driver
    //
    hydroImageData_.haveWarnings = false;
    driver_->read( hydroImageData_ );

    orcaImageData_->timeStamp.seconds  = hydroImageData_.timeStampSec;
    orcaImageData_->timeStamp.useconds = hydroImageData_.timeStampUsec;
}

void
MainThread::walk()
{
    // Set up the image objects
    orcaImageData_ = new orca::ImageData;
    orcaImageData_->data.resize( config_.width*config_.height*3 );

    // Point the pointers in hydroImageData_ at orcaImageData_
    hydroImageData_.data      = &(orcaImageData_->data[0]);

    // These functions catch their exceptions.
    activate( context_, this, subsysName() );

    initNetworkInterface();
    initHardwareDriver();

    //
    // IMPORTANT: Have to keep this loop rolling, because the '!isStopping()' call checks for requests to shut down.
    //            So we have to avoid getting stuck anywhere within this main loop.
    //
    while ( !isStopping() )
    {
        stringstream exceptionSS;
        try 
        {
            // this blocks until new data arrives
            readData();
            
            imageInterface_->localSetAndSend( orcaImageData_ );
            if ( hydroImageData_.haveWarnings )
            {
                subStatus().warning( hydroImageData_.warnings );
            }
            else
            {
                subStatus().ok();
            }

            stringstream ss;
            ss << "MainThread: Read image data: " << orcaobj::toString(orcaImageData_);
            context_.tracer().debug( ss.str(), 5 );

            continue;

        } // end of try
        catch ( Ice::CommunicatorDestroyedException & ) {
            // This is OK: it means that the communicator shut down (eg via Ctrl-C)
            // somewhere in mainLoop. Eventually, component will tell us to stop.
        }
        catch ( const Ice::Exception &e ) {
            exceptionSS << "ERROR(mainthread.cpp): Caught unexpected exception: " << e;
        }
        catch ( const std::exception &e ) {
            exceptionSS << "ERROR(mainthread.cpp): Caught unexpected exception: " << e.what();
        }
        catch ( const std::string &e ) {
            exceptionSS << "ERROR(mainthread.cpp): Caught unexpected string: " << e;
        }
        catch ( const char *e ) {
            exceptionSS << "ERROR(mainthread.cpp): Caught unexpected char *: " << e;
        }
        catch ( ... ) {
            exceptionSS << "ERROR(mainthread.cpp): Caught unexpected unknown exception.";
        }

        if ( !exceptionSS.str().empty() ) {
            context_.tracer().error( exceptionSS.str() );
            subStatus().fault( exceptionSS.str() );     
            // Slow things down in case of persistent error
            sleep(1);
        }

        // If we got to here there's a problem.
        // Re-initialise the driver.
        initHardwareDriver();

    } // end of while

    // Image hardware will be shut down in the driver's destructor.
}
