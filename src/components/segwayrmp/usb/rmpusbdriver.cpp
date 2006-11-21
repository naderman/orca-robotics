/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <orcaice/orcaice.h>    // for time functions

#include "rmpusbdriver.h"

// USB IO implementation
#include "rmpusbioftdi.h"

// rmp/usb data structure
#include "rmpusbdataframe.h"
#include "rmpdefs.h"
#include "canpacket.h"

using namespace std;
using namespace orca;
using namespace segwayrmp;

namespace {
    const int DEBUG_LEVEL=5;
}

RmpUsbDriver::RmpUsbDriver( const orcaice::Context & context )
    : rmpusbio_(NULL),
      context_(context),
      lastRawYaw_(0),
      lastRawForeaft_(0),
      odomX_(0),
      odomY_(0),
      odomYaw_(0),
      lastStatusWord1_(0),
      lastStatusWord2_(0),
      firstread_(true),
      repairCounter_(0)
{
    // parse configuration parameters
    readFromProperties( context, config_ );
    cout<<config_<<endl;
}

RmpUsbDriver::~RmpUsbDriver()
{
    cout<<"TRACE(rmpusbdriver.cpp): destructor()" << endl;
}

int
RmpUsbDriver::enable()
{
    // init device
    try {
        rmpusbio_ = new RmpUsbIoFtdi( DEBUG_LEVEL );
    }
    catch ( std::exception &e )
    {
        stringstream ss;
        ss << "Looks like the Segway is not connected.  Symptom is: "<<endl<<e.what();
        context_.tracer()->error(ss.str());
        return -1;
    }

    try {
        context_.tracer()->debug("RmpUsbDriver::enable(): connected to USB device.");
    
        // segway is physically connected; try to configure

        // first, tell it to stand still.
        orca::Velocity2dCommandPtr zero = new orca::Velocity2dCommand;
        zero->motion.v.x = 0.0;
        zero->motion.v.y = 0.0;
        zero->motion.w = 0.0;
        write( zero  );
        
        // try reading from it
        try {
            readFrame();
        }
        catch ( Exception &e )
        {
            stringstream ss;
            ss << "Looks like the Segway is powered off.  Symptom is: " << endl << e.what();
            throw Exception( ss.str() );
        }
        stringstream ssread;
        ssread << "Initial exploratory read says:"<<endl<<toString();
        context_.tracer()->debug( ssread.str() );

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
        ss << "RmpUsbDriver::enable() failed: " << e.what();
        context_.tracer()->warning( ss.str() );
        disable();
        return 2;
    }
    return 0;
}

int
RmpUsbDriver::repair()
{
    context_.tracer()->debug( "Repairing..." );
    repairCounter_++;

//     try {
//         // try a quick reset
//         rmpusbio_->reset();
//         return 0;
//     }
//     catch ( std::exception &e )
//     {
//         stringstream ss;
//         ss << "RmpUsbDriver::repair(): Quick reset failed: " << e.what();
//         context_.tracer()->debug( ss.str() );
//     }

    // Try to shutdown and init again.
    disable();
    return enable();
}

int
RmpUsbDriver::disable()
{
    cout<<"RmpUsbDriver::disabling... ("<<repairCounter_<<" repairs so far)"<<endl;
    assert( rmpusbio_ );
    delete rmpusbio_;
    rmpusbio_ = NULL;

    return 0;
}

int
RmpUsbDriver::read( orca::Position2dDataPtr &position2d, orca::Position3dDataPtr &position3d, 
                    orca::PowerDataPtr &power, std::string & status )
{
    try {
        //
        // Read a full data frame
        //
        readFrame();
    
        RmpUsbDriver::Status rmpStatus;
        updateData( position2d, position3d, power, rmpStatus );

        // do a status check (before resetting the frame)
        if ( frame_.status_word1!=lastStatusWord1_ && frame_.status_word1!=lastStatusWord2_ ) {
            cout<<"RmpUsbDriver: internal state change : "<<IceUtil::Time::now().toString()<<endl;
            cout<<toString()<<endl;
            lastStatusWord1_ = frame_.status_word1;
            lastStatusWord2_ = frame_.status_word2;
        }

        frame_.reset();

        // update status (only change it when internal state changes?)
        std::ostringstream os;
        os << "State1="<<frame_.CuStatus1ToString()<<" State2="<<frame_.CuStatus2ToString();
        status = os.str();

        return 0;
    }
    catch ( std::exception &e )
    {
        stringstream ss; 
        ss << "RmpUsbDriver::read(): Error: " << e.what();
        context_.tracer()->error( ss.str() );
        return -1;
    }
}

int
RmpUsbDriver::write( const orca::Velocity2dCommandPtr & command )
{
    try {
        makeMotionCommandPacket( &pkt_, command );

        rmpusbio_->writePacket(&pkt_);

        return 0;
    }
    catch ( std::exception &e )
    {
        stringstream ss;
        ss << "RmpUsbDriver::write(): Error: " << e.what();
        context_.tracer()->error( ss.str() );
        return -1;
    }
}

std::string
RmpUsbDriver::toString()
{
    return frame_.toString();
}

void
RmpUsbDriver::readFrame()
{
    RmpUsbIoFtdi::RmpUsbStatus status;
    int canPacketsProcessed = 0;
    int dataFramesReopened = 0;
    int timeoutCount = 0;

    // arbitrary parameters
    const int maxCanPacketsProcessed = 100;
    const int maxTimeoutCount = 2;

    // get next packet from the packet buffer, will block until new packet arrives
    while( canPacketsProcessed < maxCanPacketsProcessed && timeoutCount < maxTimeoutCount )
    {
        status = rmpusbio_->readPacket( &pkt_ );

        if ( status == RmpUsbIoFtdi::NO_DATA ) {
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
                //cout<<"RmpUsbDriver::readFrame: pkts:"<<canPacketsProcessed<<" re-opened: "<<dataFramesReopened<<endl;
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
    throw Exception( "RmpUsbDriver::readFrame(): either processed too many packets or got too many timeouts without getting a complete frame" );
}

void
RmpUsbDriver::integrateMotion()
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
RmpUsbDriver::updateData( orca::Position2dDataPtr &position2d, orca::Position3dDataPtr &position3d,
                          orca::PowerDataPtr &power, Status & status )
{
    // set all time stamps right away
    orcaice::setToNow( position2d->timeStamp );
    orcaice::setToNow( position3d->timeStamp );
    orcaice::setToNow( power->timeStamp );

    // POSITION2D
    //
    // for odometry, use integrated values
    position2d->pose.p.x    = odomX_;
    position2d->pose.p.y    = odomY_;
    position2d->pose.o      = odomYaw_;

    // for current rates, use instanteneous values
    // combine left and right wheel velocity to get forward velocity
    // change from counts/sec into meters/sec
    position2d->motion.v.x =
            ((double)frame_.left_dot+(double)frame_.right_dot) /
             RMP_COUNT_PER_M_PER_S / 2.0;

    // no side speeds for this bot
    position2d->motion.v.y = 0.0;

    // from counts/sec into deg/sec.  also, take the additive
    // inverse, since the RMP reports clockwise angular velocity as positive.
    position2d->motion.w = -(double)frame_.yaw_dot / RMP_COUNT_PER_RAD_PER_S;

    // @todo stall from currents?
    position2d->stalled = false;

    // POSITION3D
    //
    // for odometry, use integrated values
    position3d->pose.p.x    = odomX_;
    position3d->pose.p.y    = odomY_;
    // Player got it right: "this robot doesn't fly"
    position3d->pose.p.z    = 0.0;

    position3d->pose.o.r    = frame_.roll;
    position3d->pose.o.p    = frame_.pitch;
    position3d->pose.o.y    = odomYaw_;

    // forward speed is the same as for the 2D interface
    position3d->motion.v.x = position2d->motion.v.x;
    // no side speeds for this 'bot
    position3d->motion.v.y = 0.0;
    // no jumps for this 'bot
    position3d->motion.v.z = 0.0;

    // note the correspondence between pitch and roll rate and the 
    // axes around which the rotation happens.
    position3d->motion.w.x = frame_.roll_dot;
    position3d->motion.w.y = frame_.pitch_dot;
    // from counts/sec into deg/sec.  also, take the additive
    // inverse, since the RMP reports clockwise angular velocity as positive.
    position3d->motion.w.z = -(double)frame_.yaw_dot / RMP_COUNT_PER_RAD_PER_S;


    // POWER
    //
    // Convert battery voltage from counts to Volts
    // we only know the minimum of the two main batteries
    power->batteries[0].voltage = frame_.base_battery_voltage / RMP_BASE_COUNT_PER_VOLT;
    power->batteries[0].percent = 99.0;
    power->batteries[0].secRemaining = 8*60*60;
    power->batteries[1].voltage = frame_.base_battery_voltage / RMP_BASE_COUNT_PER_VOLT;
    power->batteries[1].percent = 99.0;
    power->batteries[1].secRemaining = 8*60*60;
    power->batteries[2].voltage = RMP_UI_OFFSET + frame_.ui_battery_voltage*RMP_UI_COEFF;
    power->batteries[2].percent = 99.0;
    power->batteries[2].secRemaining = 8*60*60;

    // INTERNAL STATUS
    status.buildId = frame_.build_id;
    status.cuState = frame_.status_word1;
    status.opMode = frame_.operational_mode;
    status.gainSchedule = frame_.controller_gain_schedule;

    //debug
    //cout<<"cu battery voltage (CU): "<<power->batteries[0].voltage<<" ("<<frame_.base_battery_voltage<<")"<<endl;
    // info from CU in msg 406
    //cout<<"ui battery voltage (CU): "<<power->batteries[2].voltage<<" ("<<frame_.ui_battery_voltage<<")"<<endl;
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
RmpUsbDriver::resetAllIntegrators()
{
    makeStatusCommandPacket( &pkt_, RMP_CMD_RESET_INTEGRATORS, RMP_CAN_RESET_ALL );

    try {
        rmpusbio_->writePacket(&pkt_);
    }
    catch ( std::exception &e )
    {
        stringstream ss; ss << "RmpUsbDriver::resetAllIntegrators(): " << e.what();
        throw Exception( ss.str() );
    }
}

void
RmpUsbDriver::setMaxVelocityScaleFactor( double scale )
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
        rmpusbio_->writePacket(&pkt_);
    }
    catch ( std::exception &e )
    {
        stringstream ss; ss << "RmpUsbDriver::setMaxVelocityScaleFactor(): " << e.what();
        throw Exception( ss.str() );
    }
}

void
RmpUsbDriver::setMaxTurnrateScaleFactor( double scale )
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
        rmpusbio_->writePacket(&pkt_);
    }
    catch ( std::exception &e )
    {
        stringstream ss; ss << "RmpUsbDriver::setMaxTurnrateScaleFactor(): " << e.what();
        throw Exception( ss.str() );
    }
}

void
RmpUsbDriver::setMaxAccelerationScaleFactor( double scale )
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
        rmpusbio_->writePacket(&pkt_);
    }
    catch ( std::exception &e )
    {
        stringstream ss; ss << "RmpUsbDriver::setMaxAccelerationScaleFactor(): " << e.what();
        throw Exception( ss.str() );
    }
}

void
RmpUsbDriver::setMaxCurrentLimitScaleFactor( double scale )
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
        rmpusbio_->writePacket(&pkt_);
    }
    catch ( std::exception &e )
    {
        stringstream ss; ss << "RmpUsbDriver::setMaxCurrentLimitScaleFactor(): " << e.what();
        throw Exception( ss.str() );
    }

}

void
RmpUsbDriver::setOperationalMode( OperationalMode mode )
{
    makeStatusCommandPacket( &pkt_, RMP_CMD_SET_OPERATIONAL_MODE, mode );

    try {
        rmpusbio_->writePacket(&pkt_);
    }
    catch ( std::exception &e )
    {
        stringstream ss; ss << "RmpUsbDriver::setOperationalMode(): " << e.what();
        throw Exception( ss.str() );
    }
}

void
RmpUsbDriver::setGainSchedule( int sched )
{
    makeStatusCommandPacket( &pkt_, RMP_CMD_SET_GAIN_SCHEDULE, sched );

    try {
        rmpusbio_->writePacket(&pkt_);
    }
    catch ( std::exception &e )
    {
        stringstream ss; ss << "RmpUsbDriver::setGainSchedule(): " << e.what();
        throw Exception( ss.str() );
    }

}

void
RmpUsbDriver::enableBalanceMode( bool enable )
{
    if ( enable ) {
        makeStatusCommandPacket( &pkt_, RMP_CMD_SET_BALANCE_MODE_LOCKOUT, BalanceAllowed );
    }
    else {
        makeStatusCommandPacket( &pkt_, RMP_CMD_SET_BALANCE_MODE_LOCKOUT, BalanceNotAllowed );
    }
    
    try {
        rmpusbio_->writePacket(&pkt_);
    }
    catch ( std::exception &e )
    {
        stringstream ss; ss << "RmpUsbDriver::enableBalanceMode(): " << e.what();
        throw Exception( ss.str() );
    }
}

/*
 *  Takes an Orca command object and turns it into CAN packets for the RMP
 */
void
RmpUsbDriver::makeMotionCommandPacket( CanPacket* pkt, const Velocity2dCommandPtr & command )
{
    pkt->id = RMP_CAN_ID_COMMAND;
    // velocity command does not change any other values
    pkt->PutSlot(2, (uint16_t)RMP_CMD_NONE);

    // translational RMP command
    int16_t trans = (int16_t) rint(command->motion.v.x * RMP_COUNT_PER_M_PER_S);
    // check for command limits
    if(trans > RMP_MAX_TRANS_VEL_COUNT) {
        trans = RMP_MAX_TRANS_VEL_COUNT;
    }
    else if(trans < -RMP_MAX_TRANS_VEL_COUNT) {
        trans = -RMP_MAX_TRANS_VEL_COUNT;
    }

    // rotational RMP command
    int16_t rot = (int16_t) rint(command->motion.w * RMP_COUNT_PER_RAD_PER_S);
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
RmpUsbDriver::makeStatusCommandPacket( CanPacket* pkt, uint16_t commandId, uint16_t value )
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
RmpUsbDriver::makeShutdownCommandPacket( CanPacket* pkt )
{
    pkt->id = RMP_CAN_ID_SHUTDOWN;

    //printf("SEGWAYIO: SHUTDOWN: pkt: %s\n", pkt->toString());
}

// Calculate the difference between two raw counter values, taking care
// of rollover.
int
RmpUsbDriver::diff( uint32_t from, uint32_t to, bool first )
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
RmpUsbDriver::watchPacket( CanPacket* pkt, short int pktID )
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
RmpUsbDriver::watchDataStream( CanPacket* pkt )
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
