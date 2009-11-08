/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "hwthread.h"
#include <iostream>

// we need these to throw orca exceptions from the functions executed in the network thread
#include <orca/exceptions.h>
#include <orcaice/orcaice.h>

using namespace std;
using namespace robot2d;

HwThread::HwThread( const orcaice::Context &context ) :
    SubsystemThread( context.tracer(), context.status(), "HwThread" ),
    context_(context)
{
    setMaxHeartbeatInterval( 10.0 );

    //
    // Read settings
    //
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    isMotionEnabled_ = (bool)orcaice::getPropertyAsIntWithDefault( prop, prefix+"EnableMotion", 1 );

    // Dynamically load the library and find the factory
    std::string driverLibName = 
        orcaice::getPropertyWithDefault( prop, prefix+"DriverLib", "libHydroRobot2dPlayerClient.so" );
    context_.tracer().debug( "HwThread: Loading driver library "+driverLibName, 4 );
    // The factory which creates the driver
    std::auto_ptr<hydrointerfaces::Robot2dFactory> driverFactory;
    try {
        driverLib_.reset( new hydrodll::DynamicallyLoadedLibrary(driverLibName) ); 
        driverFactory.reset(  
            hydrodll::dynamicallyLoadClass<hydrointerfaces::Robot2dFactory,Robot2dDriverFactoryMakerFunc>
            ( *driverLib_, "createDriverFactory" ) );
    }
    catch (hydrodll::DynamicLoadException &e)
    {
        context_.tracer().error( e.what() );
        throw;
    }

    // create the driver
    try {
        context_.tracer().info( "HwThread: Creating driver..." );
        driver_.reset( driverFactory->createDriver( context_.toHydroContext() ) );
    }
    catch ( ... )
    {
        stringstream ss;
        ss << "HwThread: Caught unknown exception while creating driver";
        context_.tracer().error( ss.str() );
        throw;
    }  
}

void
HwThread::work()
{
    stringstream exceptionSS;
    std::string reason;

    //
    // Main loop
    //
    while( !isStopping() )
    {
        //
        // Make sure we're OK
        //
        if ( stateMachine_.isFault( reason ) )
        {
            // Try to (re-)enable
            setMaxHeartbeatInterval( 5.0 );    
            enableDriver();

            // we enabled, so presume we're OK.
            stateMachine_.setOK();
            setMaxHeartbeatInterval( 2.0 );

            // but make sure we're not shutting down.
            if ( isStopping() )
                break;
        }

        //
        // Read data from the hardware
        //
        try {
            hydrointerfaces::Robot2d::Data data;
            bool stateChanged = driver_->read( data );

            // stick it in the store, so that NetThread can distribute it                
            dataStore_.set( data );

            // Update status
            if ( stateChanged ) 
            {
                std::string status;
                bool isWarn, isFault;
                driver_->getStatus( status, isWarn, isFault );
                std::stringstream ss;
                ss << "Saw state change: " << status;
                if ( isFault )
                {
                    stateMachine_.setFault( ss.str() );
                    context_.tracer().error( ss.str() );
                }
                else if ( isWarn )
                {
                    stateMachine_.setWarning( ss.str() );
                    context_.tracer().warning( ss.str() );
                }
                else
                {
                    stateMachine_.setOK();
                    context_.tracer().info( ss.str() );
                }
            }
        }
        catch ( ... ) {
            string problem = orcaice::catchExceptionsWithStatus( "reading from hardware", health() );
            stateMachine_.setFault( problem ); 
        }

        //
        // Write pending commands to the hardware
        //
        if ( !commandStore_.isEmpty() && !stateMachine_.isFault() )
        {
            hydrointerfaces::Robot2d::Command command;
            commandStore_.get( command );

            try {
                driver_->write( command );

                stringstream ss;
                ss << "HwThread: wrote command: " << command.toString();
                context_.tracer().debug( ss.str() );
            }
            catch ( ... ) {
                string problem = orcaice::catchExceptionsWithStatus( "writing to hardware", health() );
                stateMachine_.setFault( problem ); 
            }
        }

        // Tell the 'status' engine what our local state machine knows.
        if ( stateMachine_.isFault(reason) )
        {
            health().fault( reason );
        }
        else if ( stateMachine_.isWarning(reason) )
        {
            health().warning( reason );
        }
        else
        {
            health().ok();
        }

    } // while
}

//////////////////////////

void
HwThread::enableDriver()
{
    while ( !isStopping() ) 
    {
        stringstream exceptionSS;
        try {
            context_.tracer().info("HwThread: (Re-)Enabling driver...");
            driver_->enable();
            context_.tracer().info( "HwThread: Enable succeeded." );
            return;
        }
        catch ( ... ) {
            string problem = orcaice::catchExceptionsWithStatusAndSleep( "enabling hardware driver", health(), gbxutilacfr::SubsystemFault, 2000 );

            stateMachine_.setFault( problem );
        }
    }
}

void
HwThread::setCommand( const hydrointerfaces::Robot2d::Command &command )
{
    // if we know we can't write, don't try: inform remote component of problem
    std::string reason;
    if ( stateMachine_.isFault(reason) )
    {
        throw orca::HardwareFailedException( reason );
    }

    if ( !isMotionEnabled_ )
    {
        throw orca::HardwareFailedException( "Motion is disabled" );
    }

    commandStore_.set( command );

    stringstream ss;
    ss << "HwThread::setCommand( "<<command.toString()<<" )";
    context_.tracer().debug( ss.str() );
}
