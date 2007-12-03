
/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <cmath>

// we need these to throw orca exceptions from the functions executed in the network thread
#include <orca/exceptions.h>
// alexm: this is a network object? should it be here?
#include "estopconsumerI.h"
#include "hwthread.h"

using namespace std;

namespace {
    const char *SUBSYSTEM = "HwThread";
}

namespace segwayrmp {

HwThread::HwThread( Config& config, const orcaice::Context &context ) :
    driver_(0),
    driverFactory_(0),
    driverLib_(0),
    context_(context)
{
    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 10.0 );
    context_.status()->initialising( SUBSYSTEM );

    //
    // Read settings
    //
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    // By default the estop interface is not enabled...
    // alexm: this param is read-in twice: here and in NetThread, doesn't seem right.
    isEStopEnabled_ = (bool)orcaice::getPropertyAsIntWithDefault( prop, prefix+"EnableEStopInterface", 0 );
    stringstream ss; ss <<"HwThread: isEStopInterfaceEnabled is set to "<< isEStopEnabled_<<endl;
    context_.tracer()->info( ss.str() );
 
    isMotionEnabled_ = (bool)orcaice::getPropertyAsIntWithDefault( prop, prefix+"EnableMotion", 1 );

    // Dynamically load the library and find the factory
    std::string driverLibName = 
        orcaice::getPropertyWithDefault( prop, prefix+"DriverLib", "libHydroSegwayRmpAcfrCan.so" );
    context_.tracer()->debug( "HwThread: Loading driver library "+driverLibName, 4 );
    try {
        driverLib_ = new hydrodll::DynamicallyLoadedLibrary(driverLibName);
        driverFactory_ = 
            hydrodll::dynamicallyLoadClass<hydrointerfaces::SegwayRmpFactory,DriverFactoryMakerFunc>
            ( *driverLib_, "createDriverFactory" );
    }
    catch (hydrodll::DynamicLoadException &e)
    {
        context_.tracer()->error( e.what() );
        throw;
    }

    // create the driver
    hydroutil::Properties props( prop->getPropertiesForPrefix(prefix), prefix );
    hydrointerfaces::Context driverContext( props, context_.tracer(), context_.status() );
    try {
        context_.tracer()->info( "HwThread: creating driver..." );
        driver_ = driverFactory_->createDriver( driverContext );
    }
    catch ( ... )
    {
        stringstream ss;
        ss << "HwThread: Caught exception while creating driver.";
        context_.tracer()->error( ss.str() );
        throw;
    }  

    // let the driver apply limits and save the param for future use
    driver_->applyHardwareLimits( config.maxForwardSpeed, config.maxReverseSpeed,
                        config.maxTurnrate, config.maxTurnrateAtMaxSpeed );
    maxForwardSpeed_ = config.maxForwardSpeed;
    maxReverseSpeed_ = config.maxReverseSpeed;
    maxTurnrate_ = config.maxTurnrate;
}

HwThread::~HwThread()
{
    delete driver_;
    delete driverFactory_;
    delete driverLib_;
}

void
HwThread::enableDriver()
{
    while ( !isStopping() ) 
    {
        try {
            context_.tracer()->info("HwThread: (Re-)Enabling driver...");
            driver_->enable();
            context_.tracer()->info( "HwThread: Enable succeeded." );
            return;
        }
        catch ( std::exception &e )
        {
            std::stringstream ss;
            ss << "HwThread::enableDriver(): enable failed: " << e.what();
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
            stateMachine_.setFault( ss.str() );
        }
        catch ( ... )
        {
            std::stringstream ss;
            ss << "HwThread::enableDriver(): enable failed due to unknown exception.";
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
            stateMachine_.setFault( ss.str() );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }
}

void
HwThread::walk()
{
    std::string reason;
    const int eStopTimeoutMs = 1200;

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
            context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 5.0 );    

            // Is the Estop correctly enabled?
            if( isEStopEnabled_ && ( !isEStopConnected(eStopTimeoutMs) ))
               {continue;}
            
            
            enableDriver();

            // we enabled, so presume we're OK.
            stateMachine_.setOK();
            context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 2.0 );

            // but make sure we're not shutting down.
            if ( isStopping() )
                break;
        }

        //
        // Read data from the hardware
        //
        try {
            hydrointerfaces::SegwayRmp::Data data;
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
                    context_.tracer()->error( ss.str() );
                }
                else if ( isWarn )
                {
                    stateMachine_.setWarning( ss.str() );
                    context_.tracer()->warning( ss.str() );
                }
                else
                {
                    stateMachine_.setOK();
                    context_.tracer()->info( ss.str() );
                }
            }
        }
        catch ( std::exception &e )
        {
            std::stringstream ss;
            ss << "HwThread: Failed to read: " << e.what();
            context_.tracer()->error( ss.str() );

            stateMachine_.setFault( ss.str() );
        }
        catch ( ... )
        {
            std::stringstream ss;
            ss << "HwThread: Failed to read due to unknown exception.";
            context_.tracer()->error( ss.str() );

            stateMachine_.setFault( ss.str() );            
        }

        //
        // Write pending commands to the hardware
        //
        if ( commandStore_.isNewData() && !stateMachine_.isFault() )
        {
            hydrointerfaces::SegwayRmp::Command command;
            commandStore_.get( command );

            try {
                driver_->write( command );

                stringstream ss;
                ss << "HwThread: wrote command: " << command.toString();
                context_.tracer()->debug( ss.str() );
            }
            catch ( std::exception &e )
            {
                std::stringstream ss;
                ss << "HwThread: Failed to write command to hardware: " << e.what();
                context_.tracer()->error( ss.str() );

                // set local state to failure
                stateMachine_.setFault( ss.str() );
            }
            catch ( ... )
            {
                std::stringstream ss;
                ss << "HwThread: Failed to write command to hardware due to unknown exception.";
                context_.tracer()->error( ss.str() );

                // set local state to failure
                stateMachine_.setFault( ss.str() );                
            }
        }

        // TODO should this be optional?

        // Does the estop interface indicate a stop now fault!
        if (isEStopEnabled_) 
        {
            if(eStopFaultStatus_.isNewData())
            {
                EStopStatus eStopStatus;
                eStopFaultStatus_.get(eStopStatus);
                if( eStopStatus == segwayrmp::ESS_FAULT )
                {
                    std::stringstream ss; ss << "HwThread: EstopInterface shows error state, disabling motion.";
                    context_.tracer()->error( ss.str() );
                    // set local state to failure
                    stateMachine_.setFault( ss.str() );                                
                }        
                /*
                     else if(){
                     //TODO need to implement timeout
                   }
                 */
            }

        }

        // Tell the 'status' engine what our local state machine knows.
        if ( stateMachine_.isFault(reason) )
        {
            context_.status()->fault( SUBSYSTEM, reason );
        }
        else if ( stateMachine_.isWarning(reason) )
        {
            context_.status()->warning( SUBSYSTEM, reason );
        }
        else
        {
            context_.status()->ok( SUBSYSTEM );
        }


    } // while
}

bool
HwThread::commandImpossible( const hydrointerfaces::SegwayRmp::Command &command )
{
    if ( command.vx > maxForwardSpeed_ )
        return true;

    //Note that maxReverseSpeed is a positive number.
    if ( command.vx < -maxReverseSpeed_ )
        return true;

    if ( command.w > maxTurnrate_ )
        return true;
    if ( command.w < -maxTurnrate_ )
        return true;

    return false;
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

    if ( commandImpossible( command ) )
    {
        stringstream ss;
        ss << "Requested speed ("<<command.toString()<<") can not be achieved.  "
           << "maxForwardSpeed: " << maxForwardSpeed_ << ", maxReverseSpeed: " << maxReverseSpeed_ << ", maxTurnrate: " << maxTurnrate_*M_PI/180.0;
        cout << ss.str() <<endl;
        throw orca::MalformedParametersException( ss.str() );
    }

    double msecs=writeTimer_.elapsed().toMilliSecondsDouble();
    writeTimer_.restart();
    // Check how long since we received our last command
    if ( msecs>300 ) 
    {
        std::stringstream ss;
        ss << "HwDriverHandler: It's been " << msecs << "ms since we last received a command."<<endl
           << "This will cause the Segway to time out.";
        context_.tracer()->debug( ss.str() );
    }

    commandStore_.set( command );

    stringstream ss;
    ss << "HwThread::setCommand( "<<command.toString()<<" )";
    context_.tracer()->debug( ss.str() );
}

bool
HwThread::isEStopConnected(int timeoutMs)
{
    
    //Get the estop status or timeout.
    EStopStatus eStopStatus;
    if(eStopFaultStatus_.getNext(eStopStatus, timeoutMs) != 0)
    {
        string Msg("HwThread: No EStop data available. Will try again");
        context_.tracer()->error( Msg );
        stateMachine_.setFault( Msg );
        return false;
    }                

    //Are we showing a fault on the estop
    if (eStopStatus != segwayrmp::ESS_NO_FAULT)
    {
        string Msg( "HwThread: EStop data indicating fault. Trying again" );
        context_.tracer()->error( Msg );
        stateMachine_.setFault( Msg );
        return false;
    }

    return true;
}
    
} // namespace
