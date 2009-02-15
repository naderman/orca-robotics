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
    orcaice::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context),
    snrWarningThreshhold_(0)
{
}

void 
MainThread::initialise()
{   
    subStatus().setMaxHeartbeatInterval( 10.0 );

    // These functions catch their exceptions.
    activate( context_, this, subsysName() );
    // check for stop signal after retuning from multi-try
    if ( isStopping() )
        return;

    initNetworkInterface();
    initHardwareDriver();
}

void 
MainThread::work()
{   
    subStatus().setMaxHeartbeatInterval( 3.0 );

    while ( !isStopping() )
    {
        // this try makes this component robust to exceptions
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
        catch ( ... ) 
        {
            orcaice::catchMainLoopExceptions( subStatus() );
        }
            
    } // end of big while loop
    
}

///////////////

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
        throw gbxutilacfr::Exception( ERROR_INFO, "Unknown driver type. Valid driver values are {'hardware','fake'}" );
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
