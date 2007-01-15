/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <vector>
#include <orcawifi/wifiutil.h>
#include <orcaice/orcaice.h>
#include "mainloop.h"
#include "hardwaredriver.h"
#include "fakedriver.h"

using namespace std;
using namespace orca;

namespace wifi {
    

MainLoop::MainLoop( const orcaice::Context & context )
    : context_(context)
{
    initNetwork();
    initDriver();
}

MainLoop::~MainLoop()
{
}

void 
MainLoop::initNetwork()
{
    wifiInterface_ = new orcaifaceimpl::WifiI( "Wifi", context_ );

    while ( isActive() ) 
    {
        try {
            wifiInterface_->initInterface();
            context_.tracer()->debug( "wifi interface initialized",2);
            break;
        }
        catch ( const orcaice::NetworkException& e ) {
            context_.tracer()->warning( "Failed to setup interface. Check Registry and IceStorm. Will try again in 2 secs...");
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        }
        catch ( const Ice::Exception& e ) {
            context_.tracer()->warning( "Failed to setup interface. Check Registry and IceStorm. Will try again in 2 secs...");
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        }
    }
    
}

void MainLoop::initDriver()
{
    //
    // Read settings
    //
    std::string prefix = context_.tag() + ".Config.";
    
    string driverName = orcaice::getPropertyWithDefault( context_.properties(), prefix+"Driver", "hardware" );
    context_.tracer()->debug( std::string("loading ")+driverName+" driver",3);
    if ( driverName == "hardware" )
    {
        driver_ = new HardwareDriver();
    }
    else if (driverName == "fake" )
    {
        driver_ = new FakeDriver();
    }
    else
    {
        string errorStr = "Unknown driver type.";
        context_.tracer()->error( errorStr);
        context_.tracer()->info( "Valid driver values are {'hardware','fake'}" );
        throw orcaice::Exception( ERROR_INFO, errorStr );
    }
    
    context_.tracer()->debug("driver instantiated",5);
}

void 
MainLoop::run()
{   
    while ( isActive() )
    {
        try
        {
            orca::WifiData data;
            driver_->read( data );
            
            context_.tracer()->debug("Got fresh wifi data from driver. Sending it out now." , 5);
                    
            wifiInterface_->localSetAndSend( data );
            
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
            
        }
        catch ( const wifiutil::Exception & e )
        {
            stringstream ss;
            if (e.type()==wifiutil::NoWirelessInterface) {
                ss << "No wireless interface available at the moment. We'll try again in two seconds\n" << e.what();
                IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
            } else {
                ss << e.what();
            }
            context_.tracer()->error( ss.str() );
        }
        catch ( const orca::OrcaException & e )
        {
            stringstream ss;
            ss << "unexpected (remote?) orca exception: " << e << ": " << e.what;
            context_.tracer()->error( ss.str() );
        }
        catch ( const orcaice::Exception & e )
        {
            stringstream ss;
            ss << "unexpected (local?) orcaice exception: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch ( const Ice::Exception & e )
        {
            stringstream ss;
            ss << "unexpected Ice exception: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch ( const std::exception & e )
        {
            // once caught this beast in here, don't know who threw it 'St9bad_alloc'
            stringstream ss;
            ss << "unexpected std exception: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch ( ... )
        {
            context_.tracer()->error( "unexpected exception from somewhere.");
        }
            
    } // end of big while loop
    
    context_.tracer()->debug( "TRACE(mainloop.cpp): End of run() now...", 5 );
    
    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    
}

} // end of namespace
