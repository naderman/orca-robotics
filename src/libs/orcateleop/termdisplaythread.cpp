/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>

#include "termdisplaythread.h"
#include "iostreamdisplaydriver.h"
#include "events.h"

using namespace std;
using namespace orcateleop;

TermDisplayThread::TermDisplayThread( const orcaice::Context& context ) : 
    SafeThread(context.tracer()),
    events_(new hydroiceutil::EventQueue),
    context_(context)
{
    //
    // Read settings
    //
    Ice::PropertiesPtr prop = context_.properties();;
    std::string prefix = context_.tag() + ".Config.";

    // based on the config parameter, create the right driver
//     string driverName = orcaice::getPropertyWithDefault( prop, prefix+"Driver", "kbd-termio" );

    driver_ = new IostreamDisplayDriver();
}

TermDisplayThread::~TermDisplayThread()
{
    delete driver_;
}

void 
TermDisplayThread::sentNewVelocityCommand( 
                double vx, double vy, double w, bool vxLimit, bool vyLimit, bool wLimit )
{
    hydroiceutil::EventPtr e = new SentVelocityCommandEvent( vx,vy,w, vxLimit,vyLimit,wLimit );
    events_->add( e );
}

void 
TermDisplayThread::sentNewBicycleCommand( 
                double speed, double steerAngle, bool speedLimit, bool steerAngleLimit )
{
    hydroiceutil::EventPtr e = new SentBicycleCommandEvent( speed, steerAngle, speedLimit, steerAngleLimit );
    events_->add( e );
}

void 
TermDisplayThread::sentRepeatCommand()
{
    cout<<"."<<flush;
}

void 
TermDisplayThread::failedToSendCommand()
{
    cout<<"x"<<flush;
}

// read commands from the keyboard. Launced in a separate thread.
void
TermDisplayThread::walk()
{
    // we are in a different thread now, catch all stray exceptions
    try
    {
    
    // don't forget to enable the driver, but check !isStopping() to see if we should quit
    while ( driver_->enable() && !isStopping() ) {
        context_.tracer().warning("Failed to enable driver. Will try again in 2 seconds.");
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }

    // check again to make sure we are not being terminated
    if ( isStopping() ) {
        return;
    }
    
    context_.tracer().debug("Display driver enabled",2);
    
    hydroiceutil::EventPtr event;
    int timeoutMs = 500;

    //
    // Main loop
    //
    while ( !isStopping() )
    {
        if ( !events_->timedGet( event, timeoutMs ) ) {
            continue;
        }

        switch ( event->type() )
        {
        // approx in order of call frequency
        case SentVelocityCommand : {
            SentVelocityCommandEventPtr e = SentVelocityCommandEventPtr::dynamicCast( event );

            stringstream ss;
            ss<<"\nVelocityControl2d command (vx,vy,w(deg/s)) : ("<<e->vx_<<", "<<e->vy_<<", "<< RAD2DEG(e->w_) << ") ";
            if ( e->vxLimit_ ) ss<<"Vx LIMITED. ";
            if ( e->vyLimit_ ) ss<<"Vy LIMITED. ";
            if ( e->wLimit_ ) ss<<"W LIMITED. ";
            ss<<endl;

            driver_->show( ss.str() );
            break;
        }
        case SentBicycleCommand : {
            SentBicycleCommandEventPtr e = SentBicycleCommandEventPtr::dynamicCast( event );

            stringstream ss;
            ss<<"\nBicycleCommand command (speed[m],ster[deg]) : ("<<e->speed_<<", "<<RAD2DEG(e->steerAngle_) << ") ";
            if ( e->speedLimit_ ) ss<<"V LIMITED. ";
            if ( e->steerAngleLimit_ ) ss<<"Steer LIMITED. ";
            ss<<endl;

            driver_->show( ss.str() );
            break;
        }
        default : {
            stringstream ss; ss<<"LOCAL: TermDisplayThread: unknown event "<<event->type()<<". Ignoring...";
            driver_->show( ss.str() );
        }
        } // switch
    } // end of main loop

    // reset the hardware
    if ( driver_->disable() ) {
        context_.tracer().warning("failed to disable driver");
    }
    context_.tracer().debug("driver disabled",2);

    } // try
    catch ( ... )
    {
        context_.tracer().error( "unexpected exception from somewhere.");
        context_.communicator()->destroy();
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    context_.tracer().debug( "TermDisplayThread: stopped.",2 );
}
