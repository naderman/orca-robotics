/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ROBOTDRIVERUTIL_HARDWARE_HANDLER_H
#define ORCA2_ROBOTDRIVERUTIL_HARDWARE_HANDLER_H

#include <orcaice/safethread.h>
#include <orcaice/context.h>
#include <orcaice/proxy.h>
#include <orcaice/timer.h>
#include <IceUtil/Time.h>
#include <orcaice/orcaice.h>
#include <sstream>
#include <iostream>
#include <orcarobotdriverutil/ihwdriver.h>

namespace orcarobotdriverutil
{

//!
//! @brief A thread that looks after the robot hardware driver (ie IHwDriver).
//! Needs to be templated over:
//!   Command (what we tell the driver to do)
//!   Data    (what the driver tells us)
//!
template<typename Command, typename Data>
class HwDriverHandler : public orcaice::SafeThread
{
public:

    HwDriverHandler( IHwDriver<Command,Data>  &driver,
                     bool                      isMotionEnabled,
                     const orcaice::Context   &context );

    // from SafeThread
    virtual void walk();

    void setCommand( const Command &command );

    orcaice::Proxy<Data> &dataProxy() { return dataProxy_; }

private:

    // Keeps trying until success or !isActive()
    void enableDriver();

    // This structs stores fault information.  
    class FaultInfo {
    public:
        FaultInfo( bool isError=false )
            : isError_(isError) {}
        FaultInfo( bool isError, std::string diagnostics )
            : isError_(isError),
              diagnostics_(diagnostics)
            {}

        bool isError() const { return isError_; }
        const std::string &diagnostics() { return diagnostics_; }
    private:
        bool isError_;
        std::string diagnostics_;
    };
    //
    // Faults can be detected in either read or write threads: have to be careful.
    //
    orcaice::Proxy<FaultInfo> faultProxy_;

    // network/hardware interface
    orcaice::Proxy<Data> dataProxy_;

    IHwDriver<Command,Data> &driver_;

    bool isMotionEnabled_;

    // debug
    orcaice::Timer writeTimer_;

    orcaice::Context context_;
};

//////////////////////////////////////////////////////////////////////

template<typename Command, typename Data>
HwDriverHandler<Command,Data>::HwDriverHandler( IHwDriver<Command,Data> &driver,
                                                bool                     isMotionEnabled,
                                                const orcaice::Context  &context )
    : driver_(driver),
      isMotionEnabled_(isMotionEnabled),
      context_(context)
{
    context_.status()->setMaxHeartbeatInterval( "HwDriverHandler", 10.0 );
    context_.status()->initialising( "HwDriverHandler" );

    // unsure about faults until we enable the driver
    faultProxy_.set( FaultInfo( true, "Initialising" ) );
}

template<typename Command, typename Data>
void
HwDriverHandler<Command,Data>::enableDriver()
{
    while ( isActive() ) 
    {
        try {
            context_.tracer()->info("(Re-)Enabling driver...");
            driver_.enable();
            context_.tracer()->info( "Enable succeeded." );
            return;
        }
        catch ( std::exception &e )
        {
            std::stringstream ss;
            ss << "HwDriverHandler::enableDriver(): enable failed: " << e.what();
            context_.tracer()->error( ss.str() );
            context_.status()->fault( "HwDriverHandler", ss.str() );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }
}

template<typename Command, typename Data>
void
HwDriverHandler<Command,Data>::walk()
{
    IceUtil::Time startTime = IceUtil::Time::now();
    
    // temp data object
    Data data;

    //
    // Main loop
    //
    while( isActive() )
    {
        //
        // Make sure we're OK
        //
        FaultInfo faultInfo;
        faultProxy_.get( faultInfo );
        if ( faultInfo.isError() )
        {
            context_.status()->setMaxHeartbeatInterval( "HwDriverHandler", 5.0 );    
            enableDriver();

            // we enabled, so presume we're OK.
            faultProxy_.set( FaultInfo(false) );
            context_.status()->setMaxHeartbeatInterval( "HwDriverHandler", 2.0 );

            // but make sure we're not shutting down.
            if ( !isActive() )
                break;
        }

        //
        // Read data from the hardware
        //
        try {
            bool stateChanged = driver_.read( data );

            // stick it in the proxy, so that NetHandler can distribute it                
            dataProxy_.set( data );

            // Update status
            if ( stateChanged ) 
            {
                std::string status;
                bool isWarn, isFault;
                driver_.getStatus( status, isWarn, isFault );
                std::stringstream ss;
                ss << "Saw state change: " << status;
                if ( isFault )
                {
                    context_.tracer()->error( ss.str() );
                    context_.status()->fault( "HwDriverHandler", status );
                }
                else if ( isWarn )
                {
                    context_.tracer()->warning( ss.str() );
                    context_.status()->warning( "HwDriverHandler", status );
                }
                else
                {
                    context_.tracer()->info( ss.str() );
                    context_.status()->ok( "HwDriverHandler", status );
                }
            }
            else
                context_.status()->ok( "HwDriverHandler" );
        }
        catch ( std::exception &e )
        {
            std::stringstream ss;
            ss << "HwDriverHandler: Failed to read: " << e.what();
            context_.tracer()->error( ss.str() );
            context_.status()->fault( "HwDriverHandler", ss.str() );
            faultProxy_.set( FaultInfo( true, ss.str() ) );
        }
    } // while
}

template<typename Command, typename Data>
void
HwDriverHandler<Command,Data>::setCommand( const Command &command )
{
    // if we know we can't write, don't try: inform remote component of problem
    FaultInfo faultInfo;
    faultProxy_.get( faultInfo );
    if ( faultInfo.isError() )
    {
        throw orca::HardwareFailedException( faultInfo.diagnostics() );
    }

    if ( !isMotionEnabled_ )
    {
        context_.tracer()->debug( "FaultInfo: Ignoring command because !isMotionEnabled" );
        return;
    }

    // debug
    double msecs=writeTimer_.elapsed().toMilliSecondsDouble();
    writeTimer_.restart();
    // Check how long since we received our last command
    if ( msecs>300 ) 
    {
        std::stringstream ss;
        ss << "HwDriverHandler: It's been " << msecs << "ms since we last received a command!";
        context_.tracer()->debug( ss.str() );
    }

    //
    // write to hardware
    //
    try {
        driver_.write( command );
    }
    catch ( std::exception &e )
    {
        std::stringstream ss;
        ss << "HwDriverHandler: Failed to write command to hardware: " << e.what();
        context_.tracer()->error( ss.str() );
        // set local state to failure
        faultProxy_.set( FaultInfo( true, ss.str() ) );

        // inform remote client of hardware failure
        throw orca::HardwareFailedException( ss.str() );
    }

    std::stringstream ssDebug;
    ssDebug << "HwDriverHandler: Received and wrote command: " << command.toString();
    context_.tracer()->debug( ssDebug.str() );
}

} // namespace

#endif
