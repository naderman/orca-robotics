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
#include <sstream>
#include <orcaice/orcaice.h>    // for time functions

#include "rmpdriver.h"

// rmp data structure
#include "rmpdataframe.h"
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
    : rmpIo_(rmpIo),
      context_(context),
      lastRawYaw_(0),
      lastRawForeaft_(0),
      odomX_(0),
      odomY_(0),
      odomYaw_(0),
      lastStatusWord1_(0),
      lastStatusWord2_(0),
      firstread_(true)
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
        throw RmpException( ss.str() );
    }

    context_.tracer()->debug("RmpDriver::enable(): connected to RmpIo device.");
    
    // segway is physically connected; try to configure

    // first, tell it to stand still.
    SegwayRmpCommand zero;
    zero.vx = 0.0;
    zero.w = 0.0;
    write( zero  );
        
    // try reading from it
    try {
        readFrame();
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

// int
// RmpDriver::disable()
// {
//     cout<<"RmpDriver::disabling... ("<<repairCounter_<<" repairs so far)"<<endl;
//     assert( rmpusbio_ );
//     delete rmpusbio_;
//     rmpusbio_ = NULL;

//     return 0;
// }

bool
RmpDriver::read( SegwayRmpData &data, std::string &status )
{
    bool stateChanged = false;

    try {
        //
        // Read a full data frame
        //
        readFrame();
    
        RmpDriver::Status rmpStatus;
        updateData( data, rmpStatus );

        // do a status check (before resetting the frame)
        if ( frame_.status_word1 != lastStatusWord1_ || 
             frame_.status_word1 != lastStatusWord2_ ) 
        {
            stringstream ss;
            ss << "RmpDriver: internal state change : "<<IceUtil::Time::now().toDateTime()<<endl;
            ss<<toString()<<endl;
            ss << "StatusWords:     " << frame_.status_word1 << "  " << frame_.status_word2 << endl;
            ss << "lastStatusWords: " << lastStatusWord1_ << "  " << lastStatusWord2_ << endl;
            lastStatusWord1_ = frame_.status_word1;
            lastStatusWord2_ = frame_.status_word2;
            status = ss.str();
            stateChanged = true;
        }

        frame_.reset();

//         // update status (only change it when internal state changes?)
//         std::ostringstream os;
//         os << "State1="<<frame_.CuStatus1ToString()<<" State2="<<frame_.CuStatus2ToString();
//         status = os.str();
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
RmpDriver::applyScaling( const SegwayRmpCommand& original, SegwayRmpCommand &scaledCommand )
{
    scaledCommand.vx = original.vx / config_.maxVelocityScale;

    // alexm: this needs to be debugged, it seems that the turnrate is not scaled the same
    // way the forward velocidy is.
//     scaledCommand.w  = original.w  / config_.maxTurnrateScale;
    scaledCommand.w  = original.w;
}

void
RmpDriver::write( const SegwayRmpCommand& command )
{
    SegwayRmpCommand scaledCommand;
    applyScaling( command, scaledCommand );

    try {
        makeMotionCommandPacket( &pkt_, scaledCommand );

        rmpIo_.writePacket(&pkt_);
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
}

void
RmpDriver::get( SegwayRmpStats& stats )
{
    stats.distanceTravelled = frame_.foreaft;
}

std::string
RmpDriver::toString()
{
    return frame_.toString();
}

void
RmpDriver::readFrame()
{
    RmpIo::RmpIoStatus status;
    int canPacketsProcessed = 0;
    int dataFramesReopened = 0;
    int timeoutCount = 0;

    // arbitrary parameters
    const int maxCanPacketsProcessed = 100;
    const int maxTimeoutCount = 2;

    // get next packet from the packet buffer, will block until new packet arrives
    while( canPacketsProcessed < maxCanPacketsProcessed && timeoutCount < maxTimeoutCount )
    {
        status = rmpIo_.readPacket( &pkt_ );

        if ( status == RmpIo::NO_DATA ) {
            // not sure what to do here. treat as an error? try again?
            ++timeoutCount;
            continue;
        }
        
        ++canPacketsProcessed;
        // debug
        //watchDataStream( &pkt_ );
    
        //
        // Add packet to data frame
        // This is where we interprete all message contents!
        //
        frame_.AddPacket(&pkt_);
   
        //
        // special case: integrate robot motion
        //
        if ( pkt_.id == RMP_CAN_ID_MSG4 ) {
            integrateMotion();
        }

        // If frame is closed and complete, transfer data and reset frame
        if( frame_.isClosed() )
        {
            if ( frame_.isComplete() )
            {
                //cout<<"RmpDriver::readFrame: pkts:"<<canPacketsProcessed<<" re-opened: "<<dataFramesReopened<<endl;
                return;
            }
            else {
                // debug: the frame is closed but not complete: some packets were lost
                /* int sec = IceUtil::Time::now().toSeconds();
                cout<<sec<<" re-opening frame. Missing [";
                for ( int i=1; i<8; ++i ) {
                    if ( !frame_.msgCheckList_[i] ) { cout<<i<<" "; }
                }
                cout<<"]"<<endl; */
                // @todo or should we reset?
                frame_.reopen();
                ++dataFramesReopened;
                
                // remain in the while loop, read another frame
            }
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
    int deltaForeaftRaw = diff(lastRawForeaft_, frame_.foreaft, firstread_);
    int deltaYawRaw = diff(lastRawYaw_, frame_.yaw, firstread_);

    // store current values
    lastRawForeaft_ = frame_.foreaft;
    lastRawYaw_ = frame_.yaw;

    // convert to SI
    double deltaForeaft = (double)deltaForeaftRaw / RMP_COUNT_PER_M;
    double deltaYaw = (double)deltaYawRaw / RMP_COUNT_PER_REV * 2.0 * M_PI;

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
RmpDriver::updateData( SegwayRmpData& data, Status & status )
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

    data.roll    = frame_.roll;
    data.pitch   = frame_.pitch;
    data.yaw     = odomYaw_;

    // for current rates, use instanteneous values
    // combine left and right wheel velocity to get forward velocity
    // change from counts/sec into meters/sec
    data.vx = ((double)frame_.left_dot+(double)frame_.right_dot) /
                    RMP_COUNT_PER_M_PER_S / 2.0;
    // no side speeds for this 'bot
//     data.vy = 0.0;
    // no jumps for this 'bot
//     data.vz = 0.0;

    // note the correspondence between pitch and roll rate and the 
    // axes around which the rotation happens.
    data.droll  = frame_.roll_dot;
    data.dpitch = frame_.pitch_dot;
    // from counts/sec into deg/sec.  also, take the additive
    // inverse, since the RMP reports clockwise angular velocity as positive.
    data.dyaw   = -(double)frame_.yaw_dot / RMP_COUNT_PER_RAD_PER_S;

    // Convert battery voltage from counts to Volts
    // we only know the minimum of the two main batteries
    data.mainvolt = frame_.base_battery_voltage / RMP_BASE_COUNT_PER_VOLT;
//     power.batteries[0].percent = 99.0;
//     power.batteries[0].secRemaining = 8*60*60;
//     data.mainvolt = frame_.base_battery_voltage / RMP_BASE_COUNT_PER_VOLT;
//     power.batteries[1].percent = 99.0;
//     power.batteries[1].secRemaining = 8*60*60;
    data.uivolt = RMP_UI_OFFSET + frame_.ui_battery_voltage*RMP_UI_COEFF;
//     power.batteries[2].percent = 99.0;
//     power.batteries[2].secRemaining = 8*60*60;

    // INTERNAL STATUS
    status.buildId = frame_.build_id;
    status.cuState = frame_.status_word1;
    status.opMode = frame_.operational_mode;
    status.gainSchedule = frame_.controller_gain_schedule;

    //debug
    //cout<<"cu battery voltage (CU): "<<power.batteries[0].voltage<<" ("<<frame_.base_battery_voltage<<")"<<endl;
    // info from CU in msg 406
    //cout<<"ui battery voltage (CU): "<<power.batteries[2].voltage<<" ("<<frame_.ui_battery_voltage<<")"<<endl;
    // info from UI in heartbeat msg
    //cout<<"ui battery voltage (UI): "<<frame_.ui_heartbeat_voltage<<endl;
    //if ( frame_.ui_heartbeat_status == RMP_UI_LOW_WARNING ) {
    //    cout<<"ui battery status (UI) : low "<<endl;
    //}
    //else if ( frame_.ui_heartbeat_status == RMP_UI_EMPTY_SHUTDOWN ) {
    //    cout<<"ui battery status (UI) : shutdown "<<endl;
    //}

    // Chip debug
    //frame_.dump();
    //dump();

    firstread_ = false;
}

void
RmpDriver::resetAllIntegrators()
{
    makeStatusCommandPacket( &pkt_, RMP_CMD_RESET_INTEGRATORS, RMP_CAN_RESET_ALL );

    try {
        rmpIo_.writePacket(&pkt_);
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
    makeStatusCommandPacket( &pkt_, RMP_CMD_SET_MAX_VELOCITY_SCALE, (uint16_t)ceil(scale*16.0) );

    try {
        rmpIo_.writePacket(&pkt_);
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
    makeStatusCommandPacket( &pkt_, RMP_CMD_SET_MAX_TURNRATE_SCALE, (uint16_t)ceil(scale*16.0) );

    try {
        rmpIo_.writePacket(&pkt_);
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
    makeStatusCommandPacket( &pkt_, RMP_CMD_SET_MAX_ACCELERATION_SCALE, (uint16_t)ceil(scale*16.0) );

    try {
        rmpIo_.writePacket(&pkt_);
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
    makeStatusCommandPacket( &pkt_, RMP_CMD_SET_CURRENT_LIMIT_SCALE, (uint16_t)ceil(scale*256.0) );

    try {
        rmpIo_.writePacket(&pkt_);
    }
    catch ( std::exception &e )
    {
        stringstream ss; ss << "RmpDriver::setMaxCurrentLimitScaleFactor(): " << e.what();
        throw RmpException( ss.str() );
    }

}

void
RmpDriver::setOperationalMode( OperationalMode mode )
{
    makeStatusCommandPacket( &pkt_, RMP_CMD_SET_OPERATIONAL_MODE, mode );

    try {
        rmpIo_.writePacket(&pkt_);
    }
    catch ( std::exception &e )
    {
        stringstream ss; ss << "RmpDriver::setOperationalMode(): " << e.what();
        throw RmpException( ss.str() );
    }
}

void
RmpDriver::setGainSchedule( int sched )
{
    makeStatusCommandPacket( &pkt_, RMP_CMD_SET_GAIN_SCHEDULE, sched );

    try {
        rmpIo_.writePacket(&pkt_);
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
    if ( enable ) {
        makeStatusCommandPacket( &pkt_, RMP_CMD_SET_BALANCE_MODE_LOCKOUT, BalanceAllowed );
    }
    else {
        makeStatusCommandPacket( &pkt_, RMP_CMD_SET_BALANCE_MODE_LOCKOUT, BalanceNotAllowed );
    }
    
    try {
        rmpIo_.writePacket(&pkt_);
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
void
RmpDriver::makeMotionCommandPacket( CanPacket* pkt, const SegwayRmpCommand& command )
{
    pkt->id = RMP_CAN_ID_COMMAND;
    // velocity command does not change any other values
    pkt->PutSlot(2, (uint16_t)RMP_CMD_NONE);

    // translational RMP command
    int16_t trans = (int16_t) rint(command.vx * RMP_COUNT_PER_M_PER_S);
    // check for command limits
    if(trans > RMP_MAX_TRANS_VEL_COUNT) {
        trans = RMP_MAX_TRANS_VEL_COUNT;
    }
    else if(trans < -RMP_MAX_TRANS_VEL_COUNT) {
        trans = -RMP_MAX_TRANS_VEL_COUNT;
    }

    // rotational RMP command
    int16_t rot = (int16_t) rint(command.w * RMP_COUNT_PER_RAD_PER_S);
    // check for command limits
    if(rot > RMP_MAX_ROT_VEL_COUNT) {
        rot = RMP_MAX_ROT_VEL_COUNT;
    }
    else if(rot < -RMP_MAX_ROT_VEL_COUNT) {
        rot = -RMP_MAX_ROT_VEL_COUNT;
    }

    // put commands into the packet
    pkt->PutSlot(0, (uint16_t)trans);
    pkt->PutSlot(1, (uint16_t)rot);

    // save this last command
    lastTrans_ = trans;
    lastRot_ = rot;
}

/*
    Creates a status CAN packet from the given arguments
 */  
void
RmpDriver::makeStatusCommandPacket( CanPacket* pkt, uint16_t commandId, uint16_t value )
{
    pkt->id = RMP_CAN_ID_COMMAND;

    // put last motion command into the packet
    pkt->PutSlot(0, (uint16_t)lastTrans_);
    pkt->PutSlot(1, (uint16_t)lastRot_);

    pkt->PutSlot(2, commandId);
    
    // it was noted in the windows demo code that they
    // copied the 8-bit value into both bytes like this
    //pkt->PutByte(6, val);
    //pkt->PutByte(7, val);
    pkt->PutSlot(3, value);
    
    // debug
//     if(cmd) {
//         printf("SEGWAYIO: STATUS: cmd: %02x val: %02x pkt: %s\n", cmd, val, pkt->toString());
//     }
}

void
RmpDriver::makeShutdownCommandPacket( CanPacket* pkt )
{
    pkt->id = RMP_CAN_ID_SHUTDOWN;

    //printf("SEGWAYIO: SHUTDOWN: pkt: %s\n", pkt->toString());
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

void
RmpDriver::watchPacket( CanPacket* pkt, short int pktID )
{
    short slot0 = (short)pkt->GetSlot(0);
    short slot1 = (short)pkt->GetSlot(1);
    short slot2 = (short)pkt->GetSlot(2);
    short slot3 = (short)pkt->GetSlot(3);

    int slot0_lo = (int)pkt->GetSlot(0);
    int slot1_hi = (int)pkt->GetSlot(1) << 16;
    int slot2_lo = (int)pkt->GetSlot(2);
    int slot3_hi = (int)pkt->GetSlot(3) << 16;

    if( pkt->id == pktID )
    {
        //printf("SEGWAYIO: pkt: %s\n", pkt.toString());

        switch( pkt->id )
        {
            case RMP_CAN_ID_MSG1:
                printf("pitch = %6.2f, pitch rate = %6.2f,roll = %6.2f, roll rate = %6.2f\r",
                       (float)(slot0/RMP_COUNT_PER_DEG), (float)(slot1/RMP_COUNT_PER_DEG_PER_S),
                       (float)(slot2/RMP_COUNT_PER_DEG), (float)(slot3/RMP_COUNT_PER_DEG_PER_S));
                break;
            case RMP_CAN_ID_MSG2:
                printf("LW vel = %6.2f, RW vel = %6.2f, yaw rate = %6.2f, frames = %8i\r",
                       (float)(slot0/RMP_COUNT_PER_M_PER_S), float(slot1/RMP_COUNT_PER_M_PER_S),
                       (float)(slot2/RMP_COUNT_PER_DEG_PER_S), (int)(slot3/RMP_SEC_PER_FRAME)  );
                break;
            case RMP_CAN_ID_MSG3:
                printf("Left wheel = %6.2f, right wheel = %6.2f\r",
                       (float)((slot0_lo | slot1_hi)/RMP_COUNT_PER_M),
                       (float)((slot2_lo | slot3_hi)/RMP_COUNT_PER_M)   );
                break;
            case RMP_CAN_ID_MSG4:
                printf("Int f/a disp  = %6.2f, int yaw disp = %6.2f\r",
                       (float)((slot0_lo | slot1_hi)/RMP_COUNT_PER_M),
                       (float)((slot2_lo | slot3_hi)/RMP_COUNT_PER_REV)   );
                break;
            case RMP_CAN_ID_MSG5:
                printf("Left motor torque  = %6.2f, right motor torque = %6.2f\r",
                       (float)(slot0/1094.0), (float)(slot1/RMP_COUNT_PER_NM)   );
                break;
            case RMP_CAN_ID_MSG6:
                printf("Op mode = %1i, gain sch = %1i, UI batt = %6.2f, Base batt = %6.2f\r",
                       slot0, slot1,
                       (float)(1.5 + slot2*RMP_UI_COEFF),
                       (float)(slot3/RMP_BASE_COUNT_PER_VOLT) );
                break;
            case RMP_CAN_ID_MSG7:
                printf("Vel command = %4i, turn command = %4i\n",
                       slot0, slot1 );
                break;
        }
    }
}

void
RmpDriver::watchDataStream( CanPacket* pkt )
{
    static CanPacket priorPkt;

    // Check for breaks in the message sequence
    if( (pkt->id != (priorPkt.id + 1) )  &&
         !((pkt->id == 0x0400) && (priorPkt.id == 0x0407)) )
    {
        printf("=== BREAK IN SEQUENCE ===\n");
    }

        // Update prior packet (for debugging)
    priorPkt = *pkt;

    printf("SEGWAYIO: pkt: %s\n", pkt->toString());
}
