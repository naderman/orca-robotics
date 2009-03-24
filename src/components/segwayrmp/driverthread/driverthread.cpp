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
        subStatus().setMaxHeartbeatInterval( 5.0 );    
        enableHardware();

        subStatus().setMaxHeartbeatInterval( 1.0 );
        operateHardware();
    }    
}

void
DriverThread::enableHardware()
{
    while ( !isStopping() )
    {
        stringstream exceptionSS;
        try {
            tracer_.info("DriverThread: (Re-)Enabling driverthread...");
            segwayRmp_.enable();
            tracer_.info( "DriverThread: Enable succeeded." );
            stateMachine_.setOK();
            callback_.hardwareInitialised();
            return;
        }
        catch ( ... ) {
            string problem = orcaice::catchExceptionsWithStatusAndSleep( "enabling", subStatus(), gbxutilacfr::SubsystemFault, 2000 );

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

    // clear any pending commands
    if ( commandStore_.isNewData() )
        commandStore_.get( command );

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

            // Let the higher-ups know
            callback_.receiveData( data );

            // Check for warnings/faults
            if ( data.hasFaults )
            {
                stateMachine_.setFault( data.warnFaultReason );
                break;
            }
            else if ( data.hasWarnings )
            {
                stateMachine_.setWarning( data.warnFaultReason );
            }
            else
            {
                stateMachine_.setOK();
            }
        }
        catch ( ... ) {
            string problem = orcaice::catchExceptionsWithStatus( "reading from hardware", subStatus() );
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
        }

        // Are we still trying to hit our set-point?
        bool setPointReached = true;
        command.vx = speedSetPoint.currentCmdSpeed( setPointReached );

        // Finally, write if we're supposed to
        if ( gotNewCommand || !setPointReached )
        {
            if ( config_.driveInReverse ) reverseDirection( command );
            try {
                stringstream ss;
                ss << "DriverThread::"<<__func__<<"(): writing command: " << command.toString();
                tracer_.debug( ss.str(), 2 );

                writeToHardware( command );
            }
            catch ( ... ) {
                string problem = orcaice::catchExceptionsWithStatus( "writing to hardware", subStatus() );
                stateMachine_.setFault( problem ); 
                break;
            }
        }
    }

    std::string faultReason;
    if ( stateMachine_.isFault(faultReason) )
    {
        subStatus().fault( faultReason );
    }
}

void
DriverThread::writeToHardware( const hydrointerfaces::SegwayRmp::Command &command )
{
    segwayRmp_.write( command );
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

}

