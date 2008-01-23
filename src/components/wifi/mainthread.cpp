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
#include <vector>
#include <hydrowifi/wifiutil.h>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include "mainthread.h"
#include "hardwaredriver.h"
#include "fakedriver.h"

using namespace std;
using namespace wifi;


MainThread::MainThread( const orcaice::Context& context ) :
    hydroiceutil::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context),
    snrWarningThreshhold_(0)
{
    subStatus().setMaxHeartbeatInterval( 10.0 );
}

void 
MainThread::initNetworkInterface()
{
    wifiInterface_ = new orcaifaceimpl::WifiImpl( "Wifi", context_ );
    wifiInterface_->initInterface( this, subsysName());
}

void 
MainThread::initHardwareDriver()
{
    //
    // Read settings
    //
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";
    
    // get warning threshhold first
    snrWarningThreshhold_ = orcaice::getPropertyAsIntWithDefault( prop, prefix+"SnrWarningThreshhold",10);
    
    string driverName = orcaice::getPropertyWithDefault( prop, prefix+"Driver", "hardware" );
    context_.tracer().debug( std::string("loading ")+driverName+" driver",3);
    if ( driverName == "hardware" )
    {
        driver_ = new HardwareDriver( context_ );
    }
    else if (driverName == "fake" )
    {
        driver_ = new FakeDriver();
    }
    else
    {
        // unrecoverable error
        context_.shutdown(); 
        throw hydroutil::Exception( ERROR_INFO, "Unknown driver type. Valid driver values are {'hardware','fake'}" );
    }
    context_.tracer().debug("driver instantiated",5);
    
    driver_->init();
    context_.tracer().debug("driver initialized",5);
}

void
MainThread::checkWifiSignal( orca::WifiData &data )
{
    for (unsigned int i=0; i<data.interfaces.size(); i++)
    {
        const orca::WifiInterface &iface = data.interfaces[i];
        
        if ( iface.linkType != orca::LinkQualityTypeDbm )
        {
            // we can't judge how good the signal is in relative mode, so just say ok
            subStatus().ok();
            continue;
        }
        
        // if we are below the threshhold, spit out a warning
        if ( (iface.signalLevel-iface.noiseLevel) < snrWarningThreshhold_ ) 
        {
            stringstream ss;
            ss << "Wifi signal strength of interface " << iface.interfaceName << " is below " << snrWarningThreshhold_ << " dBm";
            subStatus().warning( ss.str() );
            context_.tracer().warning( ss.str() );
        }
        else
        {
            subStatus().ok();
        }
    }
    
}

void 
MainThread::walk()
{   
    // These functions catch their exceptions.
    activate( context_, this, subsysName() );

    initNetworkInterface();
    initHardwareDriver();

    subStatus().setMaxHeartbeatInterval( 3.0 );

    while ( !isStopping() )
    {
        try
        {
            orca::WifiData data;
            driver_->read( data );
            
            context_.tracer().debug("Got new wifi data from driver. Sending it out now.", 3);
            context_.tracer().debug(orcaobj::toString( data ), 5);
                    
            wifiInterface_->localSetAndSend( data );
            
            checkWifiSignal( data );
            
            IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(500));            
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
            context_.tracer().error( ss.str() );
            subStatus().fault( ss.str() );
        }
        catch ( const orca::OrcaException & e )
        {
            stringstream ss;
            ss << "unexpected (remote?) orca exception: " << e << ": " << e.what;
            context_.tracer().error( ss.str() );
            subStatus().fault( ss.str() );
        }
        catch ( const hydroutil::Exception & e )
        {
            stringstream ss;
            ss << "unexpected (local?) orcaice exception: " << e.what();
            context_.tracer().error( ss.str() );
            subStatus().fault( ss.str() );
        }
        catch ( const Ice::Exception & e )
        {
            stringstream ss;
            ss << "unexpected Ice exception: " << e;
            context_.tracer().error( ss.str() );
            subStatus().fault( ss.str() );
        }
        catch ( const std::exception & e )
        {
            stringstream ss;
            ss << "unexpected std exception: " << e.what();
            context_.tracer().error( ss.str() );
            subStatus().fault( ss.str() );
        }
        catch ( ... )
        {
            string err = "unexpected exception from somewhere.";
            context_.tracer().error( err );
            subStatus().fault( err );
        }
            
    } // end of big while loop
    
}
