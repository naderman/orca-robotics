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

#include "networkthread.h"
#include "display.h"
#include "events.h"
#include "velocitycontroldriver.h"
#include "drivebicycledriver.h"

using namespace std;
using namespace orcateleop;

NetworkThread::NetworkThread( Display* display, const orcaice::Context& context ) :
    SafeThread( context.tracer() ),
    events_(new hydroiceutil::EventQueue),
    display_(display),
    context_(context)
{
    // install event optimizer
    hydroiceutil::EventQueueOptimizerPtr opt = new TeleopEventQueueOptimizer;
    events_->setOptimizer( opt );
}

NetworkThread::~NetworkThread()
{
}

void 
NetworkThread::newMixedCommand( const hydrointerfaces::HumanInput2d::Command& command )
{
//     cout<<"DEBUG: NetworkThread::newMixedCommand: "<<command.toString()<<endl;
    
    hydroiceutil::EventPtr e = new MixedCommandEvent( 
        command.longitudinal, command.isLongIncrement,
        command.transverse, command.isTransverseIncrement,
        command.angular, command.isAngularIncrement );
    events_->optimizedAdd( e );
}

void 
NetworkThread::newIncrementCommand( int longitudinal, int transverse, int angle )
{
// cout<<"DEBUG: got command incresment : "<<longitudinal<<" "<<transverse<<" "<<angle<<endl;
    hydroiceutil::EventPtr e = new IncrementCommandEvent( longitudinal, transverse, angle );
    events_->optimizedAdd( e );
}

void 
NetworkThread::newRelativeCommand( double longitudinal, double transverse, double angle )
{
// cout<<"DEBUG: got relative command : "<<longitudinal<<"% "<<transverse<<"% "<<angle<<"%"<<endl;
    hydroiceutil::EventPtr e = new RelativeCommandEvent( longitudinal, transverse, angle );
    events_->optimizedAdd( e );
}

void
NetworkThread::walk()
{
    // The only provided interfaces are the 2 standard ones: Home and Status.
    // We can just skip this activation step and they will not be visible on
    // on the network (if network traffic is an issue, for example).
    //
    // multi-try function
    orcaice::activate( context_, this );

    //
    // Read settings
    //
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    // multi-try function
    std::string ifaceId = orcaice::getInterfaceIdWithTag( context_, "Generic", this );

    // based on the ID of the interface, create the right network driver
    if ( ifaceId == "::orca::VelocityControl2d" )
    {
        context_.tracer().info("loading 'VelocityControl2d' driver");
        driver_ = new VelocityControl2dDriver( display_, context_ );
    }
    else if ( ifaceId == "::orca::DriveBicycle" )
    {
        context_.tracer().info("loading 'DriveBicycle' driver");
        driver_ = new DriveBicycleDriver( display_, context_ );
    }
    else {
        string errorStr = "Unsupported interface ID="+ifaceId;
        context_.tracer().error( errorStr); 
        context_.tracer().info( "Valid driver values are {'VelocityControl2d', 'DriveBicycle'}" );
        throw gbxutilacfr::Exception( ERROR_INFO, errorStr );
    }    

    // don't forget to enable the driver, but check isStopping() to see if we should quit
    while ( driver_->enable() && !isStopping() ) {
        context_.tracer().warning("Failed to enable network driver. Will try again in 2 seconds.");
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }

    // check again to make sure we are not being terminated
    if ( isStopping() ) {
        return;
    }
    context_.tracer().debug("Network driver enabled",2);


    hydroiceutil::EventPtr event;    
    int timeoutMs = (int)floor(1000.0 * orcaice::getPropertyAsDoubleWithDefault(
            context_.properties(), prefix+"RepeatInterval", 0.2 ) );
    //
    // Main loop
    //
    while ( !isStopping() )
    {
        context_.tracer().debug( "NetworkThread: waiting for event...", 10 );
        if ( !events_->timedGet( event, timeoutMs ) ) {
            driver_->repeatCommand();
            continue;
        }

        switch ( event->type() )
        {
        // approx in order of call frequency
        case MixedCommand : {
            MixedCommandEventPtr e = MixedCommandEventPtr::dynamicCast( event );
            if ( !e ) break;

            driver_->processMixedCommand( 
                e->longitudinal, e->isLongIncrement, 
                e->transverse, e->isTransverseIncrement,
                e->angular, e->isAngularIncrement );
            break;
        }        
        case IncrementCommand : {
            //cout<<"focus changed event"<<endl;
            IncrementCommandEventPtr e = IncrementCommandEventPtr::dynamicCast( event );
            if ( !e ) break;
            driver_->processIncrementCommand( e->longitudinal, e->transverse, e->angular );
            break;
        }        
        case RelativeCommand : {
            //cout<<"focus changed event"<<endl;
            RelativeCommandEventPtr e = RelativeCommandEventPtr::dynamicCast( event );
            if ( !e ) break;
            driver_->processRelativeCommand( e->longitudinal, e->transverse, e->angular );
            break;
        }        
        default : {
            cout<<"unknown network event "<<event->type()<<". Ignoring..."<<endl;
            break;
        }
        } // switch
    } // end of main loop
}
