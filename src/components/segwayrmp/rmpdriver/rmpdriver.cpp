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
using namespace orca;
using namespace segwayrmp;

namespace {
    const int DEBUG_LEVEL=5;
}

RmpDriver::RmpDriver( const orcaice::Context & context,
                      RmpIo &rmpIo )
    : model_(RmpModel_200),
      rmpIo_(rmpIo),
      rxData_(model_,context),
      context_(context),
      lastRawYaw_(0),
      lastRawForeaft_(0),
      odomX_(0),
      odomY_(0),
      odomYaw_(0),
      lastStatusWord1_(0),
      lastStatusWord2_(0),
      firstread_(true),
      converter_(RmpModel_200)
{
    // parse configuration parameters
    readFromProperties( context, config_ );
    cout<<config_<<endl;
}

void
RmpDriver::enable()
{
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
        
    // try reading from it
    try {
        context_.tracer()->debug( "RmpDriver::enable(): Trying initial read" );
        readData( rxData_ );
    }
    catch ( RmpException &e )
    {
        stringstream ss;
        ss << "RmpDriver: Looks like the Segway is powered off.  Symptom is: " << endl << e.what();
        throw RmpException( ss.str() );
    }

    stringstream ssread;
    ssread << "Initial exploratory read says:"<<endl<<toString();
    context_.tracer()->debug( ssread.str() );

//     cout<<"DEBUG: SETTING MAX VELOCITY SCALE = "<<config_.maxVelocityScale<<endl;
    
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
        // Read a new RxData from scratch
        //
        readData( rxData_ );    
        updateData( data );

        // do a status check (before resetting the frame)
        if ( rxData_.rawData().status_word1 != lastStatusWord1_ || 
             rxData_.rawData().status_word2 != lastStatusWord2_ ) 
        {
            lastStatusWord1_ = rxData_.rawData().status_word1;
            lastStatusWord2_ = rxData_.rawData().status_word2;
            stateChanged = true;
        }

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
    ss<<toString();
    status = ss.str();
    isWarn = rxData_.rawData().isWarn();
    isFault = rxData_.rawData().isFault();
}

void
RmpDriver::applyScaling( const Command& originalCommand, Command &scaledCommand )
{
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

std::string
RmpDriver::toString()
{
    return rxData_.rawData().toString();
}

void
RmpDriver::readData( RxData &rxData )
{
    // Start from scratch
    rxData = RxData( model_, context_ );    

    RmpIo::RmpIoStatus status;
    int canPacketsProcessed = 0;
    int timeoutCount = 0;

    // arbitrary parameters
    const int maxCanPacketsProcessed = 100;
    const int maxTimeoutCount = 2;

    CanPacket pkt;

    // get next packet from the packet buffer, will block until new packet arrives
    while( canPacketsProcessed < maxCanPacketsProcessed && timeoutCount < maxTimeoutCount )
    {
        status = rmpIo_.readPacket( pkt );

        if ( status == RmpIo::NO_DATA ) {
            // not sure what to do here. treat as an error? try again?
            ++timeoutCount;
            continue;
        }
        
        ++canPacketsProcessed;
        if ( ++canPacketsProcessed > 9 )
        {
            stringstream ss;
            ss << "RmpDriver::readData: processed " << canPacketsProcessed << " without completing rxData!";
            context_.tracer()->warning( ss.str() );
        }

        //
        // This is where we interprete all message contents!
        //
        rxData_.addPacket(pkt);
   
        //
        // special case: integrate robot motion
        //
        if ( pkt.id() == RMP_CAN_ID_MSG4 ) {
            integrateMotion();
        }

        // If frame is complete, we can stop reading
        if ( rxData_.isComplete() )
        {
            return;
        }
    }   // while

    // either processed too many packets or got too many timeouts without
    // getting a complete frame.
    throw RmpException( "RmpDriver::readFrame(): either processed too many packets or got too many timeouts without getting a complete frame" );
}


//**??** djlm... Do we know anything about changes in wheel diameters here?
//**??** djlm what happens if the segway integrated values are reset or rollover?
void
RmpDriver::integrateMotion()
{
    // Get the new linear and angular encoder values and compute odometry.
    int deltaForeaftRaw = diff(lastRawForeaft_, rxData_.rawData().foreaft_displacement, firstread_);
    int deltaYawRaw = diff(lastRawYaw_, rxData_.rawData().yaw, firstread_);

    // store current values
    lastRawForeaft_ = rxData_.rawData().foreaft_displacement;
    lastRawYaw_ = rxData_.rawData().yaw;

    // convert to SI
    double deltaForeaft = converter_.distanceInM( deltaForeaftRaw );
    double deltaYaw = converter_.angleInRadiansFromRevCounts(deltaYawRaw);

    // First-order odometry integration
    odomX_ += deltaForeaft * cos(odomYaw_);
    odomY_ += deltaForeaft * sin(odomYaw_);
    odomYaw_ += deltaYaw;

    // Normalize yaw in [0, 360]
    odomYaw_ = atan2(sin(odomYaw_), cos(odomYaw_));
    if (odomYaw_ < 0) {
        odomYaw_ += 2 * M_PI;
    }
}

void
RmpDriver::updateData( Data& data )
{
    // set all time stamps right away
    orca::Time t = orcaice::toOrcaTime( IceUtil::Time::now() );
    data.seconds = t.seconds;
    data.useconds = t.useconds;

    // for odometry, use integrated values
    data.x    = odomX_;
    data.y    = odomY_;
    // Player got it right: "this robot doesn't fly"
//     data.z    = 0.0;

    data.roll    = rxData_.rawData().roll;
    data.pitch   = rxData_.rawData().pitch;
    data.yaw     = odomYaw_;

    // for current rates, use instanteneous values
    // combine left and right wheel velocity to get forward velocity
    // change from counts/sec into meters/sec
    data.vx = (converter_.speedInMperS(rxData_.rawData().left_wheel_velocity)+converter_.speedInMperS(rxData_.rawData().right_wheel_velocity)) / 2.0;

    // no side speeds for this 'bot
//     data.vy = 0.0;
    // no jumps for this 'bot
//     data.vz = 0.0;

    // note the correspondence between pitch and roll rate and the 
    // axes around which the rotation happens.
    data.droll  = rxData_.rawData().roll_dot;
    data.dpitch = rxData_.rawData().pitch_dot;
    // from counts/sec into deg/sec.  also, take the additive
    // inverse, since the RMP reports clockwise angular velocity as positive.
    data.dyaw   = -(double)rxData_.rawData().yaw_dot / RMP_COUNT_PER_RAD_PER_S;

    // Convert battery voltage from counts to Volts
    // we only know the minimum of the two main batteries
    data.mainvolt = rxData_.rawData().base_battery_voltage / RMP_BASE_COUNT_PER_VOLT;
//     power.batteries[0].percent = 99.0;
//     power.batteries[0].secRemaining = 8*60*60;
//     data.mainvolt = rxData_.rawData().base_battery_voltage / RMP_BASE_COUNT_PER_VOLT;
//     power.batteries[1].percent = 99.0;
//     power.batteries[1].secRemaining = 8*60*60;
    data.uivolt = RMP_UI_OFFSET + rxData_.rawData().ui_battery_voltage*RMP_UI_COEFF;
//     power.batteries[2].percent = 99.0;
//     power.batteries[2].secRemaining = 8*60*60;


    firstread_ = false;
}

void
RmpDriver::resetAllIntegrators()
{
    CanPacket pkt = makeStatusCommandPacket( RMP_CMD_RESET_INTEGRATORS, RMP_CAN_RESET_ALL );

    try {
        rmpIo_.writePacket(pkt);
    }
    catch ( std::exception &e )
    {
        stringstream ss; ss << "RmpDriver::resetAllIntegrators(): " << e.what();
        throw RmpException( ss.str() );
    }
}

void
RmpDriver::setMaxVelocityScaleFactor( double scale )
{
    assert( scale>=0.0);
    assert( scale<=1.0);
    // limit input to [0.0, 1.0]
    if ( scale>1.0 ) {
        scale=1.0;
    } else if ( scale<0.0 ) {
        scale=0.0;
    }
    CanPacket pkt = makeStatusCommandPacket( RMP_CMD_SET_MAX_VELOCITY_SCALE, (uint16_t)ceil(scale*16.0) );

    try {
        rmpIo_.writePacket(pkt);
    }
    catch ( std::exception &e )
    {
        stringstream ss; ss << "RmpDriver::setMaxVelocityScaleFactor(): " << e.what();
        throw RmpException( ss.str() );
    }
}

void
RmpDriver::setMaxTurnrateScaleFactor( double scale )
{
    assert( scale>=0.0);
    assert( scale<=1.0);
    // limit input to [0.0, 1.0]
    if ( scale>1.0 ) {
        scale=1.0;
    } else if ( scale<0.0 ) {
        scale=0.0;
    }
    CanPacket pkt = makeStatusCommandPacket( RMP_CMD_SET_MAX_TURNRATE_SCALE, (uint16_t)ceil(scale*16.0) );

    try {
        rmpIo_.writePacket(pkt);
    }
    catch ( std::exception &e )
    {
        stringstream ss; ss << "RmpDriver::setMaxTurnrateScaleFactor(): " << e.what();
        throw RmpException( ss.str() );
    }
}

void
RmpDriver::setMaxAccelerationScaleFactor( double scale )
{
    assert( scale>=0.0);
    assert( scale<=1.0);
    // limit input to [0.0, 1.0]
    if ( scale>1.0 ) {
        scale=1.0;
    } else if ( scale<0.0 ) {
        scale=0.0;
    }
    CanPacket pkt = makeStatusCommandPacket( RMP_CMD_SET_MAX_ACCELERATION_SCALE, (uint16_t)ceil(scale*16.0) );

    try {
        rmpIo_.writePacket(pkt);
    }
    catch ( std::exception &e )
    {
        stringstream ss; ss << "RmpDriver::setMaxAccelerationScaleFactor(): " << e.what();
        throw RmpException( ss.str() );
    }
}

void
RmpDriver::setMaxCurrentLimitScaleFactor( double scale )
{
    assert( scale>=0.0);
    assert( scale<=1.0);
    // limit input to [0.0, 1.0]
    if ( scale>1.0 ) {
        scale=1.0;
    } else if ( scale<0.0 ) {
        scale=0.0;
    }
    // note: the scale of this command is [0,256]
    CanPacket pkt = makeStatusCommandPacket( RMP_CMD_SET_CURRENT_LIMIT_SCALE, (uint16_t)ceil(scale*256.0) );

    try {
        rmpIo_.writePacket(pkt);
    }
    catch ( std::exception &e )
    {
        stringstream ss; ss << "RmpDriver::setMaxCurrentLimitScaleFactor(): " << e.what();
        throw RmpException( ss.str() );
    }

}

void
RmpDriver::setGainSchedule( int sched )
{
    CanPacket pkt = makeStatusCommandPacket( RMP_CMD_SET_GAIN_SCHEDULE, sched );

    try {
        rmpIo_.writePacket(pkt);
    }
    catch ( std::exception &e )
    {
        stringstream ss; ss << "RmpDriver::setGainSchedule(): " << e.what();
        throw RmpException( ss.str() );
    }

}

void
RmpDriver::enableBalanceMode( bool enable )
{
    CanPacket pkt;
    if ( enable ) {
        pkt = makeStatusCommandPacket( RMP_CMD_SET_BALANCE_MODE_LOCKOUT, BalanceAllowed );
    }
    else {
        pkt = makeStatusCommandPacket( RMP_CMD_SET_BALANCE_MODE_LOCKOUT, BalanceNotAllowed );
    }
    
    try {
        rmpIo_.writePacket(pkt);
    }
    catch ( std::exception &e )
    {
        stringstream ss; ss << "RmpDriver::enableBalanceMode(): " << e.what();
        throw RmpException( ss.str() );
    }
}

/*
 *  Takes an Orca command object and turns it into CAN packets for the RMP
 */
CanPacket
RmpDriver::makeMotionCommandPacket( const Command& command )
{
    // translational RMP command
    int16_t trans = converter_.speedInCounts(command.vx);

    // rotational RMP command
    int16_t rot = converter_.angularRateInCounts(command.w);

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
CanPacket
RmpDriver::makeStatusCommandPacket( uint16_t commandId, uint16_t value )
{
    return statusCommandPacket( commandId, 
                                value,
                                (uint16_t)lastTrans_,
                                (uint16_t)lastRot_ );
}

// Calculate the difference between two raw counter values, taking care
// of rollover.
int
RmpDriver::diff( uint32_t from, uint32_t to, bool first )
{
    // if this is the first time, report no change
    if(first) {
        return 0;
    }

    int diff1, diff2;
    static uint32_t max = (uint32_t)pow(2.0,32.0)-1;

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

