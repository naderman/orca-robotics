#include "driverthread.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include "speedsetpoint.h"

using namespace std;

namespace segwayrmpdriverthread {

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

//////////////////////////////////////////////////////////////////////

void
DriverThread::work()
{
    while ( !isStopping() )
    {
        try {
            setMaxHeartbeatInterval( 5.0 );    
            enableHardware();

            setMaxHeartbeatInterval( 1.0 );
            operateHardware();
        }
        catch ( ... ) {
            string problem = orcaice::catchExceptionsWithStatusAndSleep( "work loop", health(), gbxutilacfr::SubsystemFault, 1000 );

            stateMachine_.setFault( problem );
        }
    }    
}

void
DriverThread::enableHardware()
{
    while ( !isStopping() )
    {
        stringstream exceptionSS;
        try {
            segwayRmp_.enable();
            tracer_.info( "DriverThread: Enable succeeded." );
            stateMachine_.setOK();
            callback_.hardwareInitialised();
            return;
        }
        catch ( ... ) {
            string problem = orcaice::catchExceptionsWithStatusAndSleep( "enabling", health(), gbxutilacfr::SubsystemFault, 2000 );

            stateMachine_.setFault( problem );
        }
    }
    cout<<"TRACE(driverthread.cpp): dropping out of enableHardware.  isStopping(): " << isStopping() << endl;
}

void
DriverThread::operateHardware()
{
    // temp data structures.
    hydrointerfaces::SegwayRmp::Data data;
    hydrointerfaces::SegwayRmp::Command command(0,0);

    // monitor the speed set-point, don't allow it to change faster than the acceleration limits
    // (initialises to zero speed)
    SpeedSetPoint speedSetPoint( config_.maxForwardAcceleration,
                                 config_.maxReverseAcceleration );

    // initialise stall sensor
    StallSensor stallSensor( config_.stallSensorConfig );
    StallSensor::StallType stallType;

    // clear any pending commands
    commandStore_.set( command );

    gbxiceutilacfr::Timer newCommandTimer;

    //
    // The big loop (we exit immediately on detecting a fault)
    //
    while ( !isStopping() )
    {
        speedSetPoint.evaluateDt();

        if ( stateMachine_.isFault() )
        {
            // Can't operate in a fault condition.
            break;
        }

        //
        // Read from the hardware
        //

        try {
            segwayRmp_.read( data );
            if ( config_.driveInReverse ) reverseDirection( data );
            stallType = stallSensor.isStalled(data);
            if ( stallType != StallSensor::NoStall )
                reportStall(data,stallType);

            // Let the higher-ups know
            callback_.receiveData( data );

            // Check for warnings/faults
            if ( data.hasFaults )
            {
                stateMachine_.setFault( data.warnFaultReason );
                health().fault( data.warnFaultReason );
                break;
            }
            else if ( data.hasWarnings )
            {
                stateMachine_.setWarning( data.warnFaultReason );
                health().warning( data.warnFaultReason );
            }
            else
            {
                stateMachine_.setOK();
                health().ok();
            }
        }
        catch ( ... ) {
            string problem = orcaice::catchExceptionsWithStatus( "reading from hardware", health() );
            stateMachine_.setFault( problem ); 
            break;
        }

        //
        // Write pending commands to the hardware
        //

        // Have we got a new command?
        bool gotNewCommand = false;
        if ( commandStore_.isNewData() )
        {
            gotNewCommand = true;
            commandStore_.get( command );
            speedSetPoint.set( command.vx );
            newCommandTimer.restart();
        }

        // Are we still trying to hit our set-point?
        bool setPointAlreadyReached = false;
        command.vx = speedSetPoint.currentCmdSpeed( setPointAlreadyReached );

        // Write if we're supposed to
        if ( gotNewCommand || !setPointAlreadyReached )
        {
            if ( config_.driveInReverse ) reverseDirection( command );
            try {
                // Probably better not to have this (mutex-locking)
                // statement in the middle of this very tight loop...
                //
                //stringstream ss;
                //ss << "DriverThread::"<<__func__<<"(): writing command: " << command.toString();
                //tracer_.debug( ss.str(), 2 );

                segwayRmp_.write( command );
            }
            catch ( ... ) {
                string problem = orcaice::catchExceptionsWithStatus( "writing to hardware", health() );
                stateMachine_.setFault( problem ); 
                break;
            }
        }

        // Check for timeouts in receiving commands
        if ( newCommandTimer.elapsedSec() > 0.2 )
        {
            if ( command.vx != 0 || command.w != 0 )
            {
                tracer_.info( "newCommandTimer timed out, resetting desired speed to zero" );
                command = hydrointerfaces::SegwayRmp::Command(0,0);
                commandStore_.set(command);
                newCommandTimer.restart();
            }
        }
    }

    std::string faultReason;
    if ( stateMachine_.isFault(faultReason) )
    {
        health().fault( faultReason );
        // pause in case of persistent fault
        sleep(1);
    }
}

void
DriverThread::setDesiredSpeed( const hydrointerfaces::SegwayRmp::Command &command )
{ 
    if ( !config_.isMotionEnabled )
    {
        throw gbxutilacfr::Exception( ERROR_INFO, "Motion disabled in configuration" );
    }
    commandStore_.set( command ); 
}

void
DriverThread::reportStall( const hydrointerfaces::SegwayRmp::Data &data,
                           const StallSensor::StallType           &stallType )
{
    stringstream ss;
    ss << "Stall condition: " << stallType << ", data: " << data.toString();
    health().warning( ss.str() );
}

}

