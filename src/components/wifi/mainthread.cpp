/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <vector>
#include <hydrowifi/wifiutil.h>
#include <orcaice/orcaice.h>
#include <orcaobj/wifi.h>
#include "mainthread.h"
#include "hardwaredriver.h"
#include "fakedriver.h"

using namespace std;
using namespace wifi;

std::string enumToString( orca::DiscreteSignalLevel level )
{
    switch (level)
    {
        case orca::SignalLevelUnknown: return "UNKNOWN";
        case orca::SignalLevelVeryLow: return "VERY LOW";
        case orca::SignalLevelLow: return "LOW";
        case orca::SignalLevelGood: return "GOOD";
        case orca::SignalLevelVeryGood: return "VERY GOOD";
        case orca::SignalLevelExcellent: return "EXCELLENT";
        default: assert( false && "Unknown signal level");
    }
}

MainThread::MainThread( const orcaice::Context& context ) :
    orcaice::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context),
    driver_(0)
{
}

MainThread::~MainThread()
{
    if (driver_!=0)
        delete driver_;
}

void 
MainThread::initialise()
{   
    setMaxHeartbeatInterval( 10.0 );

    initHardwareDriver();
    initNetworkInterface();
}

void 
MainThread::work()
{   
    // operation of this component is timed by sleep
    const int sleepIntervalMs = 500;
    setMaxHeartbeatInterval( sleepIntervalMs/1e3 );

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
            
            IceUtil::ThreadControl::sleep( IceUtil::Time::milliSeconds(sleepIntervalMs) );            
        }
        catch ( ... ) 
        {
            orcaice::catchMainLoopExceptions( health() );
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

        if (iface.linkType == orca::LinkQualityTypeDbm)
        {
            int dB = iface.signalLevel-iface.noiseLevel;
            stringstream ss;
            ss << "Checking signal strength: " << dB << "dB";
            context_.tracer().debug(ss.str(),3);
            
            if ( iface.discreteLevel == orca::SignalLevelVeryLow )
            {
                stringstream ss;
                ss << "Wifi signal strength of interface " << iface.interfaceName << " is " << enumToString( iface.discreteLevel ) << " (" << dB << "dB)";
                health().warning( ss.str() );
                context_.tracer().warning( ss.str() );
            }
            else
            {
                health().ok();
            }
        }
        else
        {
            // we can't judge how good the signal is in relative or unknown mode, so just say ok
            health().ok();        
        }
    } 
}
