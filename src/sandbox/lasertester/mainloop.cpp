/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "mainloop.h"
#include <iostream>
#include <assert.h>
#include <orcaice/orcaice.h>
#include <orcaifaceimpl/proxiedconsumers.h>

using namespace std;
using namespace orca;

MainLoop::MainLoop( const orcaice::Context                      &context )
    : context_(context)
{
}

MainLoop::~MainLoop()
{
}

void 
MainLoop::run()
{
    //
    // ENABLE NETWORK CONNECTIONS
    //
    // multi-try function
    orcaice::activate( context_, this );

    //
    // REQUIRED INTERFACE: Laser
    //
    // Connect directly to the interface
    orca::LaserScanner2dPrx laserPrx;
    orcaice::connectToInterfaceWithTag<orca::LaserScanner2dPrx>( context_, laserPrx, "Laser" );
    // Get the geometry
    cout << "Laser Description: " << orcaice::toString(laserPrx->getDescription()) << endl;


    //
    // Subscribe for data
    //
    // The guy that will receive the data stream
    orcaifaceimpl::ProxiedRangeScanner2dConsumerImplPtr consumer =
        new orcaifaceimpl::ProxiedRangeScanner2dConsumerImpl( context_ );
    // multi-try function
    consumer->subscribeWithTag( "Laser", this );

    const int TIMEOUT_MS_DURING_OPERATION      = 1000;
    const int TIME_TO_CHANGE_CONFIG            = 20000;

    // Force an immediate reconfiguration
    lastConfigChangeTime_ = IceUtil::Time::now()-IceUtil::Time::seconds(MAX_MS_BETWEEN_RECONFIGURATIONS);

    RangeScanner2dDataPtr data = new orca::LaserScanner2dData;
    IceUtil::Time lastHearbeatTime = IceUtil::Time::now();
    bool isEnabled  = true;
    laserOperating_ = false;

    while ( !isStopping() )
    {
        //
        // Reconfigure
        //
        isEnabled = possiblyReconfigure( isEnabled );

        if ( isEnabled &&
             msSinceReconfigure() > TIME_TO_CHANGE_CONFIG )
        {
            // Should be working by now.  Timeouts from now on are erroneous.
            laserOperating_ = true;
        }

        //
        // Check for data
        //
        int ret = consumer->proxy().getNext( data, TIMEOUT_MS_DURING_OPERATION );
        if ( ret == -1 )
        {
            // Timed out
            if ( laserOperating_ )
            {
                // This is an error
                stringstream ss;
                ss << "Read timed out during operation, " << msSinceReconfigure() << "ms after reconfiguration." << endl;
                context_.tracer()->error( ss.str() );
            }
        }
        else if ( ret == 1 )
        {
            // spurious wakeup
            assert(false && "Spurious wakeup");
        }
        else if ( ret == 0 )
        {
            // Got data
            std::stringstream ss;
            if ( isEnabled        &&
                 !laserOperating_ && 
                  msSinceReconfigure() > 2000 )
            {
                ss << "Got first data, max(2000," << msSinceReconfigure() << ")ms after reconfiguration.";
                context_.tracer()->print(ss.str());
                laserOperating_ = true;
            }
        }
        else
        {
            // Shouldn't get here.
            assert(false && "Shouldn't get here");
        }

        if ( (IceUtil::Time::now()-lastHearbeatTime).toMilliSecondsDouble() > 3000 )
        {
            lastHearbeatTime = IceUtil::Time::now();
            std::stringstream ss;
            ss << "Heartbeat: \n\tisEnabled: " << isEnabled
               << "\n\tlaserOperating_: " << laserOperating_
               << "\n\ttimeSinceReconfigure: " << msSinceReconfigure()/1000.0 << "sec"
               << "\n\ttime for this config: " << msTillNextReconfigure_/1000.0 << "sec"
               << "\n\tlaserConfig (read directly): "
                // alexm: this is a remote call. do we actually want to do this?
               << orcaice::toString(laserPrx->getDescription());
            context_.tracer()->print(ss.str());
        }
    }
}

int
MainLoop::msSinceReconfigure()
{
    IceUtil::Time timeSinceReconfigure = IceUtil::Time::now() - lastConfigChangeTime_;
    return (int) (timeSinceReconfigure.toMilliSecondsDouble());
}

bool
MainLoop::possiblyReconfigure( bool wasEnabled )
{
// orca v.2.0.2+: removed setConfig() from LaserScanner2d interface.
/*
    if ( msSinceReconfigure() <= msTillNextReconfigure_ )
    {
        // Don't do anything.
        cout<<"TRACE(mainloop.cpp): msSinceReconfigure: " << msSinceReconfigure() << ", msTillNextReconfigure_: " << msTillNextReconfigure_ << ": doing nothing" << endl;

        return wasEnabled;
    }

    // We should reconfigure
    orca::RangeScanner2dConfigPtr cfg = new RangeScanner2dConfig;
    
    // 
    // Generate a random configuration that the SICK can do
    //
    if ( rand() < (RAND_MAX/2.0) )
    {
//         cfg->rangeResolution = 0.01;
        cfg->maxRange        = 80.0;
    }
    else
    {
        cfg->rangeResolution = 0.001;
        cfg->maxRange        = 8.0;
    }

    if ( rand() < (RAND_MAX/2.0) )
        cfg->angleIncrement = DEG2RAD( 1.0 );
    else
        cfg->angleIncrement = DEG2RAD( 0.5 );

    if ( rand() < (RAND_MAX/2.0) )
        cfg->isEnabled = true;
    else
        cfg->isEnabled = false;

    laserOperating_ = false;

    std::stringstream ss;
    ss << msSinceReconfigure() << "ms after last reconfig, Reconfiguring laser with config:" << orcaice::toString( cfg );
    context_.tracer()->print( ss.str() );

    lastConfigChangeTime_ = IceUtil::Time::now();
    laserPrx_->setConfig( cfg );

    double p = (double)rand()/(double)RAND_MAX; // in range [0,1]
    msTillNextReconfigure_ = p * (double)MAX_MS_BETWEEN_RECONFIGURATIONS;
    {
        std::stringstream ss;
        ss << " msTillNextReconfigure_: " << msTillNextReconfigure_;
        context_.tracer()->print( ss.str() );
    }

    return cfg->isEnabled;
*/
    return true;
}
