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
#include <orcaice/orcaice.h>

#include "networkhandler.h"
#include "display.h"
#include "events.h"
#include "velocitycontroldriver.h"
#include "drivebicycledriver.h"

using namespace std;
using namespace teleop;

NetworkHandler::NetworkHandler( Display* display, const orcaice::Context& context ) :
    events_(new orcaice::EventQueue),
    display_(display),
    context_(context)
{
    // install event optimizer
    orcaice::EventQueueOptimizerPtr opt = new TeleopEventQueueOptimizer;
    events_->setOptimizer( opt );
}

NetworkHandler::~NetworkHandler()
{
}

void 
NetworkHandler::newCommandIncrement( int longitudinal, int transverse, int angle )
{
// cout<<"DEBUG: got command incresment : "<<longitudinal<<" "<<transverse<<" "<<angle<<endl;
    orcaice::EventPtr e = new NewCommandIncrementEvent( longitudinal, transverse, angle );
    events_->optimizedAdd( e );
}

void 
NetworkHandler::newRelativeCommand( double longitudinal, double transverse, double angle )
{
// cout<<"DEBUG: got relative command : "<<longitudinal<<"% "<<transverse<<"% "<<angle<<"%"<<endl;
    orcaice::EventPtr e = new NewRelativeCommandEvent( longitudinal, transverse, angle );
    events_->optimizedAdd( e );
}

void
NetworkHandler::run()
{
    // we are in a different thread now, catch all stray exceptions
    try
    {
    //
    // Read settings
    //
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    std::string ifaceId;
    while ( isActive() )
    {
        try {
            ifaceId = getInterfaceIdWithTag( context_, "Generic" );
            break;
        }
        catch ( const orcaice::NetworkException& e ) {
            std::stringstream ss;
            ss << "Failed to get interface with tag 'Generic': " << e.what() << endl;
            ss << "Will try again in 2 seconds.";
            context_.tracer()->warning(ss.str());
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        }
    }

    // based on the ID of the interface, create the right network driver
    if ( ifaceId == "::orca::VelocityControl2d" )
    {
        context_.tracer()->info("loading 'VelocityControl2d' driver");
        driver_ = new VelocityControl2dDriver( display_, context_ );
    }
    else if ( ifaceId == "::orca::DriveBicycle" )
    {
        context_.tracer()->info("loading 'DriveBicycle' driver");
        driver_ = new DriveBicycleDriver( display_, context_ );
    }
    else {
        string errorStr = "Unsupported interface ID="+ifaceId;
        context_.tracer()->error( errorStr); 
        context_.tracer()->info( "Valid driver values are {'VelocityControl2d', 'DriveBicycle'}" );
        throw orcaice::Exception( ERROR_INFO, errorStr );
    }    

    // don't forget to enable the driver, but check isActive() to see if we should quit
    while ( driver_->enable() && isActive() ) {
        context_.tracer()->warning("Failed to enable network driver. Will try again in 2 seconds.");
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }

    // check again to make sure we are not being terminated
    if ( !isActive() ) {
        return;
    }
    context_.tracer()->debug("Network driver enabled",2);


    orcaice::EventPtr event;    
    int timeoutMs = (int)floor(1000.0 * orcaice::getPropertyAsDoubleWithDefault(
            context_.properties(), prefix+"RepeatInterval", 0.2 ) );
    //
    // Main loop
    //
    while ( isActive() )
    {
        if ( !events_->timedGet( event, timeoutMs ) ) {
            driver_->repeatCommand();
            continue;
        }

        switch ( event->type() )
        {
        // approx in order of call frequency
        case NewCommandIncrement : {
            //cout<<"focus changed event"<<endl;
            NewCommandIncrementEventPtr e = NewCommandIncrementEventPtr::dynamicCast( event );
            if ( !e ) break;
            driver_->processNewCommandIncrement( e->longitudinal_, e->transverse_, e->angle_ );
            break;
        }        
        case NewRelativeCommand : {
            //cout<<"focus changed event"<<endl;
            NewRelativeCommandEventPtr e = NewRelativeCommandEventPtr::dynamicCast( event );
            if ( !e ) break;
            driver_->processNewRelativeCommand( e->longitudinal_, e->transverse_, e->angle_ );
            break;
        }        
        default : {
            cout<<"unknown network event "<<event->type()<<". Ignoring..."<<endl;
            break;
        }
        } // switch
    } // end of main loop

    //
    // unexpected exceptions
    //
    } // try
    catch ( const orca::OrcaException & e )
    {
        stringstream ss;
        ss << "unexpected (remote?) orca exception: " << e << ": " << e.what;
        context_.tracer()->error( ss.str() );
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const orcaice::Exception & e )
    {
        stringstream ss;
        ss << "unexpected (local?) orcaice exception: " << e.what();
        context_.tracer()->error( ss.str() );
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const Ice::Exception & e )
    {
        stringstream ss;
        ss << "unexpected Ice exception: " << e;
        context_.tracer()->error( ss.str() );
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const std::exception & e )
    {
        // once caught this beast in here, don't know who threw it 'St9bad_alloc'
        stringstream ss;
        ss << "unexpected std exception: " << e.what();
        context_.tracer()->error( ss.str() );
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( ... )
    {
        context_.tracer()->error( "unexpected exception from somewhere.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    
    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    context_.tracer()->debug( "NetworkHandler: stopped.",5 );
}
