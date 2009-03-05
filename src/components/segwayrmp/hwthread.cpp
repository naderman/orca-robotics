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
#include <cmath>
#include <orcaice/orcaice.h>

// we need these to throw orca exceptions from the functions executed in the network thread
#include <orca/exceptions.h>
#include "hwthread.h"
#include "stats.h"

using namespace std;
using namespace segwayrmp;

namespace {

    void swap( double &a, double &b )
    {
        double temp = a;
        a = b;
        b = temp;
    }

    void reverseDirection( hydrointerfaces::SegwayRmp::Data &data )
    {
        data.x     = -data.x;
        data.y     = -data.y;
        data.roll  = -data.roll;
        data.pitch = -data.pitch;

        data.vx     = -data.vx;
        data.droll  = -data.droll;
        data.dpitch = -data.dpitch;
        
        swap( data.leftTorque, data.rightTorque );
    }

    void reverseDirection( hydrointerfaces::SegwayRmp::Command &cmd )
    {
        cmd.vx = -cmd.vx;
    }

}

HwThread::HwThread( Config& config, const orcaice::Context &context )
    : SubsystemThread( context.tracer(), context.status(), "HwThread" ),
      context_(context)
{
    subStatus().initialising();
    subStatus().setMaxHeartbeatInterval( 10.0 );

    //
    // Read settings
    //
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    isMotionEnabled_ = (bool)orcaice::getPropertyAsIntWithDefault( prop, prefix+"EnableMotion", 1 );
    driveInReverse_  = (bool)orcaice::getPropertyAsIntWithDefault( prop, prefix+"DriveInReverse", 0 );

    bool isEStopEnabled = (bool)orcaice::getPropertyAsIntWithDefault( prop, prefix+"EnableEStopInterface", 0 );
    if ( isEStopEnabled )
    {
        double keepAlivePeriodSec = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"EStop.KeepAlivePeriodSec", 3 );
        estopInterface_.reset( new EStopInterface( keepAlivePeriodSec, context) );
    }

    // Dynamically load the library and find the factory
    std::string driverLibName = 
        orcaice::getPropertyWithDefault( prop, prefix+"DriverLib", "libHydroSegwayRmpAcfrCan.so" );
    context_.tracer().debug( "HwThread: Loading driver library "+driverLibName, 4 );
    // The factory which creates the driver
    std::auto_ptr<hydrointerfaces::SegwayRmpFactory> driverFactory;
    try {
        driverLib_.reset( new hydrodll::DynamicallyLoadedLibrary(driverLibName) );
        driverFactory.reset( 
            hydrodll::dynamicallyLoadClass<hydrointerfaces::SegwayRmpFactory,DriverFactoryMakerFunc>
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
        ss << "HwThread: Caught exception while creating driver.";
        context_.tracer().error( ss.str() );
        throw;
    }  

    // let the driver apply limits and save the param for future use
    driver_->applyHardwareLimits( config.maxForwardSpeed, 
                                  config.maxReverseSpeed,
                                  config.maxTurnrate, 
                                  config.maxLateralAcceleration );

    // Initialise the bit that controls the speed set point.
    speedSetPoint_.reset( new SetPoint( config.maxForwardAcceleration,
                                        config.maxReverseAcceleration ) );
}

void
HwThread::work()
{
    // This call catches its own exceptions
    if ( estopInterface_.get() ) estopInterface_->initInterface( this );

    stringstream exceptionSS;
    std::string reason;
    // temp data structures.
    hydrointerfaces::SegwayRmp::Data data;
    hydrointerfaces::SegwayRmp::Command command;

    // a simple class which summarizes motion information
    Stats stats;
    std::stringstream historySS;

    subStatus().working();

    //
    // Main loop
    //
    while ( !isStopping() )
    {
        //
        // Make sure we're OK
        //
        if ( stateMachine_.isFault( reason ) )
        {
            // Try to (re-)enable
            subStatus().setMaxHeartbeatInterval( 5.0 );    

            enableDriver();

            // tell stats object that the odometry stream is (possibly) reset.
            stats.resetRawData();

            checkStatus();
            subStatus().setMaxHeartbeatInterval( 2.0 );

            // but make sure we're not shutting down.
            if ( isStopping() )
                break;
        }

        //
        // Read data from the hardware
        //
        try {
            bool stateChanged = driver_->read( data );
            if ( driveInReverse_ ) reverseDirection( data );

            // stick it in the store, so that NetThread can distribute it                
            dataStore_.set( data );

            // Update status
            if ( stateChanged ) 
            {
                checkStatus();
            }

            // keep track of our approximate motion for history
            stats.addData( data );
            historySS.str(" ");
            historySS << stats.distance()<<" "<<stats.timeInMotion()<<" "<<stats.maxSpeed();
            context_.history().setWithFinishSequence( historySS.str() );
        }
        catch ( ... ) {
            string problem = orcaice::catchExceptionsWithStatus( "reading from hardware", subStatus() );
            stateMachine_.setFault( problem ); 
        }

        //
        // Write pending commands to the hardware
        //
        bool needToWriteCommand = false;
        speedSetPoint_->evaluateDt();
        if ( !stateMachine_.isFault() )
        {
            if ( commandStore_.isNewData() )
            {
                commandStore_.get( command );
                speedSetPoint_->set( command.vx );
                needToWriteCommand = true;
            }
            
            bool setPointReached;
            command.vx = speedSetPoint_->currentCmdSpeed( setPointReached );
            if ( !setPointReached )
                needToWriteCommand = true;

            if ( needToWriteCommand )
            {
                if ( driveInReverse_ ) reverseDirection( command );

                try {
                    writeCommand( command );

                    stringstream ss;
                    ss << "HwThread: wrote command: " << command.toString();
                    context_.tracer().debug( ss.str(), 2 );
                }
                catch ( ... ) {
                    string problem = orcaice::catchExceptionsWithStatus( "writing to hardware", subStatus() );
                    stateMachine_.setFault( problem ); 
                }
            }
        }

        // Tell the 'status' engine what our local state machine knows.
        stringstream ssWarn, ssFault;
        if ( estopInterface_.get() && estopInterface_->isEStopTriggered() )
        {
            ssWarn << "Software E-Stop triggered";
        }
        if ( stateMachine_.isFault(reason) )
        {
            ssFault << reason;
        }
        if ( stateMachine_.isWarning(reason) )
        {
            ssWarn << reason;
        }

        if ( !ssFault.str().empty() )
        {
            if ( !ssWarn.str().empty() )
                ssFault << ", " << ssWarn;
            subStatus().fault( ssFault.str() );
        }
        else if ( !ssWarn.str().empty() )
        {
            subStatus().warning( ssWarn.str() );
        }
        else
        {
            subStatus().ok();
        }

    } // while
}

////////////////////////////

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
            string problem = orcaice::catchExceptionsWithStatusAndSleep( "initialising", subStatus(), gbxutilacfr::SubsystemFault, 2000 );

            stateMachine_.setFault( problem );
        }
    }
}

void
HwThread::writeCommand( hydrointerfaces::SegwayRmp::Command &command )
{
    if ( estopInterface_.get() && estopInterface_->isEStopTriggered() )
    {
        command.vx = 0;
        command.w  = 0;
    }
    driver_->write( command );
}

void
HwThread::checkStatus()
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

void
HwThread::setCommand( const hydrointerfaces::SegwayRmp::Command &command )
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

    //
    // Don't bother checking that the command is feasible here:
    // netthread should have checked already.
    //

    double msecs=writeTimer_.elapsed().toMilliSecondsDouble();
    writeTimer_.restart();
    // Check how long since we received our last command
    if ( msecs>300 ) 
    {
        std::stringstream ss;
        ss << "HwDriverMainThread: It's been " << msecs << "ms since we last received a command."<<endl
           << "This will cause the Segway to time out.";
        context_.tracer().debug( ss.str(), 2 );
    }

    commandStore_.set( command );

    stringstream ss;
    ss << "HwThread::setCommand( "<<command.toString()<<" )";
    context_.tracer().debug( ss.str(), 2 );
}
