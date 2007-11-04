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
#include <sstream>
#include <orcaice/orcaice.h>    // for time functions

#include "rmpdriver.h"

// rmp data structure
#include "rmpdefs.h"
#include "canpacket.h"
#include <rmpexception.h>

using namespace std;
using namespace segwayrmp;

namespace {
    const int DEBUG_LEVEL=5;

    // Calculate the difference between two raw counter values, taking care of rollover.
    int32_t
    diffInclRollover( uint32_t from, uint32_t to )
    {
        int diff1, diff2;
        const uint32_t max = 0xFFFFFFFF;

        diff1 = to - from;

        // find difference in two directions and pick shortest
        if( to > from ) {
            diff2 = -(from + max - to);
        }
        else {
            diff2 = max - from + to;
        }

        if( abs(diff1) < abs(diff2) ) {
            return diff1;
        }
        else {
            return diff2;
        }
    }
}

RmpDriver::RmpDriver( const RmpDriverConfig  &driverConfig,
                      RmpIo                  &rmpIo,
                      const orcaice::Context &context )
    : config_(driverConfig),
      rmpIo_(rmpIo),
      odomX_(0),
      odomY_(0),
      odomYaw_(0),
      converter_(config_.model),
      rxData_(config_.model,context),
      context_(context)
{
    stringstream ss;
    ss << "RmpDriver: Using config: " << config_;
    context_.tracer()->info( ss.str() );
}

void
RmpDriver::enable()
{
    // todo: check build id

    rmpIo_.disable();

    // init device
    try {
        rmpIo_.enable( DEBUG_LEVEL );
    }
    catch ( std::exception &e )
    {
        stringstream ss;
        ss << "RmpDriver: failed to enable.  Looks like the Segway is not connected.  Symptom is: "<<endl<<e.what();
        context_.tracer()->error( ss.str() );
        throw RmpException( ss.str() );
    }

    context_.tracer()->debug("RmpDriver::enable(): connected to RmpIo device.");
    
    // segway is physically connected; try to configure

    // first, tell it to stand still.
    Command zero;
    zero.vx = 0.0;
    zero.w = 0.0;
    try {
        write( zero  );
    }
    catch ( std::exception &e )
    {
        stringstream ss; ss << "RmpDriver::enable(): write failed: " << e.what();
        throw RmpException( ss.str() );
    }

    // try reading from it (and fill out initial values in rxData_)
    try {
        context_.tracer()->debug( "RmpDriver::enable(): Trying initial read" );
        rxData_ = readData();
    }
    catch ( RmpException &e )
    {
        stringstream ss;
        ss << "RmpDriver: Looks like the Segway is powered off.  Symptom is: " << endl << e.what();
        throw RmpException( ss.str() );
    }

    stringstream ssread;
    ssread << "Initial rxData: "<<endl<<rxData_.toString();
    context_.tracer()->info( ssread.str() );

    if ( config_.requireSpecificBuildId )
    {
        // Check Build ID (ie firmware version)
        if ( rxData_.buildId() != config_.requiredBuildId )
        {
            stringstream ss;
            ss << "RmpDriver::enable(): RMP firmware version mismatch!" << endl
               << "  Required version: " << config_.requiredBuildId << endl
               << "  Found version:    " << rxData_.buildId();
            throw RmpException( ss.str() );
        }
    }
         
    try {
        // Initialise everything
        resetAllIntegrators();
        setMaxVelocityScaleFactor( config_.maxVelocityScale );
        setMaxTurnrateScaleFactor( config_.maxTurnrateScale );
        setMaxAccelerationScaleFactor( config_.maxAccelerationScale );
        setMaxCurrentLimitScaleFactor( config_.maxCurrentLimitScale );
        setGainSchedule( config_.gainSchedule );
    }
    catch ( std::exception &e )
    {
        stringstream ss;
        ss << "RmpDriver::enable() failed: " << e.what();
        rmpIo_.disable();
        throw RmpException( ss.str() );
    }
}

bool
RmpDriver::read( Data &data )
{
    bool stateChanged = false;

    try {
        //
        // Read a new RxData from the CAN bus
        //
        RxData newRxData = readData();

        // Check that the RMP read our last command correctly
        if ( newRxData.rawData().received_velocity_command != lastTrans_ ||
             newRxData.rawData().received_turnrate_command != lastRot_ )
        {
            stringstream ss;
            ss << "RmpDriver::read(): It looks like the RMP didn't read our last command correctly!"<<endl
               << "  velocity: we last wrote: " << lastTrans_ << " but it last received: " << newRxData.rawData().received_velocity_command << endl
               << "  turnrate: we last wrote: " << lastRot_ << " but it last received: " << newRxData.rawData().received_turnrate_command;
            context_.tracer()->warning( ss.str() );
        }

        // Calculate integrated odometry
        calculateIntegratedOdometry( rxData_, newRxData );

        // Fill out the argument
        data = getData();

        // do a status check
        if ( newRxData.rawData().status_word1 != rxData_.rawData().status_word1 || 
             newRxData.rawData().status_word2 != rxData_.rawData().status_word2 ) 
        {
            stateChanged = true;
        }

        // update our stored rxData
        rxData_ = newRxData;
    }
    catch ( std::exception &e )
    {
        stringstream ss; 
        ss << "RmpDriver::read(): " << e.what();
        throw RmpException( ss.str() );
    }

    return stateChanged;
}

void 
RmpDriver::getStatus( std::string &status, bool &isWarn, bool &isFault )
{
    stringstream ss;
    ss << "RmpDriver: internal state change : "<<IceUtil::Time::now().toDateTime()<<endl;
    ss<<rxData_.toString();
    status = ss.str();
    isWarn = rxData_.isWarn();
    isFault = rxData_.isFault();
}

void
RmpDriver::applyScaling( const Command& originalCommand, Command &scaledCommand )
{
    // todo: clean up and debug scaling

    scaledCommand.vx = originalCommand.vx / config_.maxVelocityScale;

    // alexm: it's not clear if maxTurnrateScale is used by segway at all
//     scaledCommand.w  = originalCommand.w  / config_.maxTurnrateScale;

    // here's our interpretation of RMP Manual Section "Theory of Operation: Turning"
    //   Vo = Pi/2
    //   if V<=Vo, Wout = Win;
    //   if V>Vo,  Wout = Win / (V/Vo);
    // assumption, we use the commanded forward velocity instead of the actual velocity.
    double vxo = 1.57;
    if ( originalCommand.vx <= vxo ) {
        scaledCommand.w = originalCommand.w;
    }
    else {
        scaledCommand.w = originalCommand.w * (originalCommand.vx/vxo);
    }
}

void
RmpDriver::write( const Command& command )
{
    // todo: scale more sensibly

    Command scaledCommand;
    applyScaling( command, scaledCommand );

    try {
        CanPacket packet = makeMotionCommandPacket( scaledCommand );
        rmpIo_.writePacket(packet);
    }
    catch ( std::exception &e )
    {
        stringstream ss;
        ss << "RmpDriver::write(): " << e.what();
        throw RmpException( ss.str() );
    }
}

void 
RmpDriver::applyHardwareLimits( double& forwardSpeed, double& reverseSpeed, 
                                double& turnrate, double& turnrateAtMaxSpeed )
{
    if ( rand() < RAND_MAX/100.0 )
        cout<<"TRACE(rmpdriver.cpp): TODO: applyHardwareLimits" << endl;
#if 0
    double forwardSpeedLimit 
        = config_.maxVelocityScale * (double)RMP_MAX_TRANS_VEL_COUNT / RMP_COUNT_PER_M_PER_S;
    double reverseSpeedLimit = forwardSpeedLimit;
    
    // alexm: this needs to be debugged, it seems that the turnrate is not scaled the same
    // way the forward velocidy is.
//     double turnrateLimit
//             = config_.maxTurnrateScale * (double)RMP_MAX_ROT_VEL_COUNT / RMP_COUNT_PER_RAD_PER_S;
    double turnrateLimit
            = (double)RMP_MAX_ROT_VEL_COUNT / RMP_COUNT_PER_RAD_PER_S;
    // the easiest thing: set turnrate limit at max speed to be the same as at zero speed
    double turnrateAtMaxSpeedLimit = turnrateLimit;

    // limit software limits to physical limits
    forwardSpeed = MIN( forwardSpeed, forwardSpeedLimit );
    reverseSpeed = MIN( reverseSpeed, reverseSpeedLimit );
    turnrate = MIN( turnrate, turnrateLimit );
    turnrate = MIN( turnrateAtMaxSpeed, turnrateAtMaxSpeedLimit );
#endif
}

RxData
RmpDriver::readData()
{
    RxData rxData( config_.model, context_ );

    RmpIo::RmpIoStatus status;
    int canPacketsProcessed = 0;
    int timeoutCount = 0;

    // arbitrary parameters
    const int maxCanPacketsProcessed = 100;
    const int maxTimeoutCount = 2;

    CanPacket pkt;

    while( canPacketsProcessed < maxCanPacketsProcessed && timeoutCount < maxTimeoutCount )
    {
        status = rmpIo_.readPacket( pkt );

        if ( status == RmpIo::NO_DATA ) {
            // not sure what to do here. treat as an error? try again?
            ++timeoutCount;
            continue;
        }
        ++canPacketsProcessed;

        if ( pkt.id() == RMP_CAN_ID_HEARTBEAT )
        {
            // Ignore this, we can get all the info from other messages
            continue;
        }

        //
        // This is where we interprete all message contents!
        //
        rxData.addPacket(pkt);
   
        // If frame is complete, we can stop reading
        if ( rxData.isComplete() )
        {
            return rxData;
        }
    }   // while

    if ( canPacketsProcessed == maxCanPacketsProcessed )
    {
        stringstream ss;
        ss << "RmpDriver::readData(): processed "<<canPacketsProcessed<<" CAN packets without finding all data.";
        throw RmpException( ss.str() );
    }
    else if ( timeoutCount == maxTimeoutCount )
    {
        stringstream ss;
        ss << "RmpDriver::readData(): timed out " << timeoutCount << " times while waiting for CAN data.";
        throw RmpException( ss.str() );
    }
    else
    {
        throw RmpException( "RmpDriver::readData() failed for reason unknown (this shouldn't happen!)" );
    }
}

// AlexB: todo: We're getting yaw and foreAft direct from the Segway, how are these calculated?
//              Could we get them from the wheel positions instead?
void
RmpDriver::calculateIntegratedOdometry( const RxData &prevData, const RxData &thisData )
{
    // Calculate the delta this new info represents
    int32_t deltaForeAftRaw = diffInclRollover( prevData.rawData().foreaft_displacement,
                                                thisData.rawData().foreaft_displacement );
    int32_t deltaYawRaw = diffInclRollover( prevData.rawData().yaw,
                                            thisData.rawData().yaw );

    // convert to SI
    double deltaForeAft = converter_.distanceInM( deltaForeAftRaw );
    double deltaYaw     = converter_.angleInRadiansFromRevCounts(deltaYawRaw);

    // First-order odometry integration
    odomX_   += deltaForeAft * cos(odomYaw_);
    odomY_   += deltaForeAft * sin(odomYaw_);
    odomYaw_ += deltaYaw;

    // Normalize yaw
    NORMALISE_ANGLE( odomYaw_ );
}

Data
RmpDriver::getData()
{
    Data data;

    // set all time stamps right away
    orca::Time t = orcaice::toOrcaTime( IceUtil::Time::now() );
    data.seconds = t.seconds;
    data.useconds = t.useconds;

    // for x,y,yaw use locally-integrated values
    data.x     = odomX_;
    data.y     = odomY_;
    data.yaw   = odomYaw_;

    // Use what we can direct from the RMP
    data.roll   = rxData_.rollAngle();
    data.pitch  = rxData_.pitchAngle();
    data.droll  = rxData_.rollRate();
    data.dpitch = rxData_.pitchRate();
    data.dyaw   = rxData_.yawRate();
    data.mainvolt = rxData_.baseBatteryVoltage();
    data.uivolt   = rxData_.uiBatteryVoltage();

    // combine left and right wheel velocity to get forward velocity
    data.vx = (rxData_.leftWheelVelocity() + rxData_.rightWheelVelocity()) / 2.0;

    return data;
}

void
RmpDriver::resetAllIntegrators()
{
    sendStatusCommandPacket( ConfigurationCommandResetIntegrators, 
                             RMP_CAN_RESET_ALL );
}

void
RmpDriver::setMaxVelocityScaleFactor( double scale )
{
    assert( scale>=0.0);
    assert( scale<=1.0);

    sendStatusCommandPacket( ConfigurationCommandSetMaxVelocityScale,
                             converter_.maxVelocityScaleFactorAsRaw(scale) );
}

void
RmpDriver::setMaxTurnrateScaleFactor( double scale )
{
    assert( scale>=0.0);
    assert( scale<=1.0);

    sendStatusCommandPacket( ConfigurationCommandSetMaxTurnrateScale,
                             converter_.maxTurnrateScaleFactorAsRaw(scale) );
}

void
RmpDriver::setMaxAccelerationScaleFactor( double scale )
{
    assert( scale>=0.0);
    assert( scale<=1.0);

    sendStatusCommandPacket( ConfigurationCommandSetMaxAccelerationScale,
                             converter_.maxAccelerationScaleFactorAsRaw(scale) );
}

void
RmpDriver::setMaxCurrentLimitScaleFactor( double scale )
{
    assert( scale>=0.0);
    assert( scale<=1.0);

    // note: the scale of this command is [0,256]
    sendStatusCommandPacket( ConfigurationCommandSetCurrentLimitScale,
                             converter_.maxCurrentLimitScaleFactorAsRaw(scale) );
}

void
RmpDriver::setGainSchedule( int sched )
{
    sendStatusCommandPacket( ConfigurationCommandSetGainSchedule, sched );
}

void
RmpDriver::enableBalanceMode( bool enable )
{
    if ( enable ) {
        sendStatusCommandPacket( ConfigurationCommandSetBalanceModeLockout, BalanceAllowed );
    }
    else {
        sendStatusCommandPacket( ConfigurationCommandSetBalanceModeLockout, BalanceNotAllowed );
    }
}

CanPacket
RmpDriver::makeMotionCommandPacket( const Command& command )
{
    // translational RMP command
    int16_t trans = converter_.speedAsRaw(command.vx);

    // rotational RMP command
    int16_t rot = converter_.angularRateAsRaw(command.w);

    if ( rand() < RAND_MAX / 100.0 )
        cout<<"TRACE(rmpdriver.cpp): TODO: check command limits" << endl;
#if 0
    // check for command limits
    if(trans > RMP_MAX_TRANS_VEL_COUNT) {
        trans = RMP_MAX_TRANS_VEL_COUNT;
    }
    else if(trans < -RMP_MAX_TRANS_VEL_COUNT) {
        trans = -RMP_MAX_TRANS_VEL_COUNT;
    }
    // check for command limits
    if(rot > RMP_MAX_ROT_VEL_COUNT) {
        rot = RMP_MAX_ROT_VEL_COUNT;
    }
    else if(rot < -RMP_MAX_ROT_VEL_COUNT) {
        rot = -RMP_MAX_ROT_VEL_COUNT;
    }
#endif

    // save this last command
    lastTrans_ = trans;
    lastRot_ = rot;

    return motionCommandPacket( trans, rot );
}

/*
    Creates a status CAN packet from the given arguments
 */  
void
RmpDriver::sendStatusCommandPacket( ConfigurationCommand command, uint16_t param )
{
    CanPacket pkt = statusCommandPacket( converter_.configurationCommandAsRaw( command ),
                                         param,
                                         (uint16_t)lastTrans_,
                                         (uint16_t)lastRot_ );

    {
        stringstream ss;
        ss << "RmpDriver: sending status command " << toString(command) << " with param " << param;
        context_.tracer()->info( ss.str() );
    }

    try {
        rmpIo_.writePacket(pkt);
    }
    catch ( std::exception &e )
    {
        stringstream ss; ss << "RmpDriver::sendStatusCommandPacket("<<toString(command)<<"): " << e.what();
        throw RmpException( ss.str() );
    }
}

