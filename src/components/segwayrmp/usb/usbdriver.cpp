/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <iostream>
#include <sstream>
#include <orcaice/orcaice.h>    // for time functions

#include "usbdriver.h"

// USB IO implementation
#include "usbftdi.h"

// rmp/usb data structure
#include "rmpusbdataframe.h"
#include "config.h"
#include "canpacket.h"


using namespace std;
using namespace orca;
using namespace segwayrmp;
using orcaice::operator<<;

UsbDriver::UsbDriver()
{
    // Hardware
    usbio_ = new UsbIoFtdi;
    frame_ = new RmpUsbDataFrame;
    pkt_ = new CanPacket;

    // Initialize odometry
    odomX_ = 0.0;
    odomY_ = 0.0;
    odomYaw_ = 0.0;
    lastRawYaw_ = 0;
    lastRawForeaft_ = 0;

    firstread_ = true;
    repairCounter_ = 0;

    lastStatusWord1_ = 0;
    lastStatusWord2_ = 0;
}

UsbDriver::~UsbDriver()
{
    delete usbio_;
    delete frame_;
    delete pkt_;
}

int
UsbDriver::enable()
{
    // init device
    if ( usbio_->init() != UsbIo::OK ) {
        return 1;
    }
    
    // segway is physically connected; try to configure

    // first, tell it to stand still.
    orca::Velocity2dCommandPtr zero = new orca::Velocity2dCommand;
    zero->motion.v.x = 0.0;
    zero->motion.v.y = 0.0;
    zero->motion.w = 0.0;
    if ( write( zero  ) ) {
        cerr<<"warning: error in writing the initial zero-velocity command";
        return 2;
    }
    
    if ( resetAllIntegrators() ) {
        cerr<<"warning: error in resetIntegrators()"<<endl;
        return 2;
    }

    if ( setMaxTurnrateScaleFactor( 1.0 ) ) {
        cerr<<"warning: 1 error in setMaxTurnrateScaleFactor()"<<endl;
        return 2;
    }

    if ( setMaxVelocityScaleFactor( 1.0 ) ) {
        cerr<<"warning: error in setMaxVelocitySpeedFactor()"<<endl;
        return 2;
    }
    
    if ( setMaxTurnrateScaleFactor( 1.0 ) ) {
        cerr<<"warning: 2 error in setMaxTurnrateScaleFactor()"<<endl;
        return 2;
    }

    if ( setMaxAccelerationScaleFactor( 1.0 ) ) {
        cerr<<"warning: error in setMaxAccelerationScaleFactor()"<<endl;
        return 2;
    }

    return 0;
}

int
UsbDriver::repair()
{
    // try a quick reset
    if ( usbio_->reset() == UsbIo::OK ) {
        return 0;
    }

    // if didn't work, try to shutdown and init again.
    disable();
    return enable();
}

int
UsbDriver::disable()
{
    cout<<"UsbDriver::disabling... ("<<repairCounter_<<" repairs so far)"<<endl;
    usbio_->shutdown();

    return 0;
}

int
UsbDriver::read( orca::Position2dDataPtr &position2d, orca::PowerDataPtr &power, std::string & status )
{
    // Try to read a full data frame
    if( readFrame() ) {
        // IO error while reading a packet, or several timeouts, or can't finish a frame
        return 1;
    }
    
    UsbDriver::Status rmpStatus;
    updateData( position2d, power, rmpStatus );

    // do a status check (before resetting the frame)
    if ( frame_->status_word1!=lastStatusWord1_ && frame_->status_word1!=lastStatusWord2_ ) {
        cout<<"UsbDriver internal state change : "<<IceUtil::Time::now().toString()<<endl;
        cout<<toString()<<endl;
        lastStatusWord1_ = frame_->status_word1;
        lastStatusWord2_ = frame_->status_word2;
    }

    frame_->reset();

    // update status (only change it when internal state changes?)
    std::ostringstream os;
    os << "State1="<<frame_->CuStatus1ToString()<<" State2="<<frame_->CuStatus2ToString();
    status = os.str();

    return 0;
}

int
UsbDriver::write( const orca::Velocity2dCommandPtr & command )
{
    makeMotionCommandPacket( pkt_, command );

    int ret = usbio_->writePacket(pkt_);

    return ret;
}

std::string UsbDriver::toString()
{
    return frame_->toString();
}

// Returns 0 if a full frame was read properly.
// Returns 1 if a single packet read failed.
int UsbDriver::readFrame()
{
    UsbIo::UsbIoStatus status;
    int canPacketsProcessed = 0;
    int dataFramesReopened = 0;
    int timeoutCount = 0;

    // arbitrary parameters
    const int maxCanPacketsProcessed = 100;
    const int maxTimeoutCount = 2;

    // get next packet from the packet buffer, will block until new packet arrives
    while( canPacketsProcessed < maxCanPacketsProcessed && timeoutCount < maxTimeoutCount )
    {
        status = usbio_->readPacket( pkt_ );

        if ( status == UsbIo::IO_ERROR ) {
            return -1;
        }
        else if ( status == UsbIo::NO_DATA ) {
            // not sure what to do here. treat as an error? try again?
            ++timeoutCount;
            continue;
        }
        
        ++canPacketsProcessed;
        // debug
        //watchDataStream( pkt_ );
    
        //
        // Add packet to data frame
        // This is where we interprete all message contents!
        //
        frame_->AddPacket(pkt_);
   
        // special case
        if ( pkt_->id == RMP_CAN_ID_MSG4 ) {
            integrateMotion();
        }

        // If frame is closed and complete, transfer data and reset frame
        if( frame_->isClosed() )
        {
            if ( frame_->isComplete() )
            {
                //cout<<"UsbDriver::readFrame: pkts:"<<canPacketsProcessed<<" re-opened: "<<dataFramesReopened<<endl;
                return 0;
            }
            else {
                // debug: the frame is closed but not complete: some packets were lost
                /* int sec = IceUtil::Time::now().toSeconds();
                cout<<sec<<" re-opening frame. Missing [";
                for ( int i=1; i<8; ++i ) {
                    if ( !frame_->msgCheckList_[i] ) { cout<<i<<" "; }
                }
                cout<<"]"<<endl; */
                // @todo or should we reset?
                frame_->reopen();
                ++dataFramesReopened;
                
                // remain in the while loop, read another frame
            }
        }

    }   // while

    // either processed too many packets or got too many timeouts without
    // getting a complete frame.
    return -1;
}

void
UsbDriver::integrateMotion()
{
    // Get the new linear and angular encoder values and compute odometry.
    int deltaForeaftRaw = diff(lastRawForeaft_, frame_->foreaft, firstread_);
    int deltaYawRaw = diff(lastRawYaw_, frame_->yaw, firstread_);

    // store current values
    lastRawForeaft_ = frame_->foreaft;
    lastRawYaw_ = frame_->yaw;

    // convert to SI
    double deltaForeaft = (double)deltaForeaftRaw / RMP_COUNT_PER_M;
    double deltaYaw = (double)deltaYawRaw / RMP_COUNT_PER_REV * M_PI;

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
UsbDriver::updateData( orca::Position2dDataPtr &position2d, orca::PowerDataPtr &power,
                            Status & status )
{
    // POSITION2D
    
    // set time stamps righ away
    orcaice::setToNow( position2d->timeStamp );
    // set time stamp righ away
    orcaice::setToNow( power->timeStamp );
    
    // for odometry, use integrated values
    position2d->pose.p.x    = odomX_;
    position2d->pose.p.y    = odomY_;
    position2d->pose.o      = odomYaw_;

    // for current rates, use instanteneous values
    // combine left and right wheel velocity to get forward velocity
    // change from counts/sec into meters/sec
    position2d->motion.v.x =
            ((double)frame_->left_dot+(double)frame_->right_dot) /
             RMP_COUNT_PER_M_PER_S / 2.0;

    // no side speeds for this bot
    position2d->motion.v.y = 0.0;

    // from counts/sec into deg/sec.  also, take the additive
    // inverse, since the RMP reports clockwise angular velocity as positive.
    position2d->motion.w = -(double)frame_->yaw_dot / RMP_COUNT_PER_RAD_PER_S;

    // @todo stall from currents?
    position2d->stalled = false;

    // POWER
    //
    // Convert battery voltage from counts to Volts
    // we only know the minimum of the two main batteries
    power->batteries[0].voltage = frame_->base_battery_voltage / RMP_BASE_COUNT_PER_VOLT;
    power->batteries[0].percent = 99.0;
    power->batteries[1].voltage = frame_->base_battery_voltage / RMP_BASE_COUNT_PER_VOLT;
    power->batteries[1].percent = 99.0;
    power->batteries[2].voltage = RMP_UI_OFFSET + frame_->ui_battery_voltage*RMP_UI_COEFF;
    power->batteries[2].percent = 99.0;

    // INTERNAL STATUS
    status.buildId = frame_->build_id;
    status.cuState = frame_->status_word1;
    status.opMode = frame_->operational_mode;
    status.gainSchedule = frame_->controller_gain_schedule;

    //debug
    //cout<<"cu battery voltage (CU): "<<power->batteries[0].voltage<<" ("<<frame_->base_battery_voltage<<")"<<endl;
    // info from CU in msg 406
    //cout<<"ui battery voltage (CU): "<<power->batteries[2].voltage<<" ("<<frame_->ui_battery_voltage<<")"<<endl;
    // info from UI in heartbeat msg
    //cout<<"ui battery voltage (UI): "<<frame_->ui_heartbeat_voltage<<endl;
    //if ( frame_->ui_heartbeat_status == RMP_UI_LOW_WARNING ) {
    //    cout<<"ui battery status (UI) : low "<<endl;
    //}
    //else if ( frame_->ui_heartbeat_status == RMP_UI_EMPTY_SHUTDOWN ) {
    //    cout<<"ui battery status (UI) : shutdown "<<endl;
    //}

/*        
        // now, do 3D info.
        this->position3d_data.xpos = htonl((int32_t)(odomX_ * 1000.0));
        this->position3d_data.ypos = htonl((int32_t)(odomY_ * 1000.0));
        // this robot doesn't fly
        this->position3d_data.zpos = 0;
        
        // normalize angles to [0,360]
        tmp = NORMALIZE(DTOR((double)frame_->roll /
                (double)RMP_COUNT_PER_DEG));
        if(tmp < 0) {
            tmp += 2*M_PI;
        }
        this->position3d_data.roll = htonl((int32_t)rint(tmp * 1000.0));
        
        // normalize angles to [0,360]
        tmp = NORMALIZE(DTOR((double)frame_->pitch /
                (double)RMP_COUNT_PER_DEG));
        if(tmp < 0) {
            tmp += 2*M_PI;
        }
        this->position3d_data.pitch = htonl((int32_t)rint(tmp * 1000.0));
        
        this->position3d_data.yaw = htonl(((int32_t)(odomYaw_ * 1000.0)));
        
        // combine left and right wheel velocity to get foreward velocity
        // change from counts/s into mm/s
        this->position3d_data.xspeed =
                htonl((uint32_t)rint(((double)frame_->left_dot +
                (double)frame_->right_dot) /
                (double)RMP_COUNT_PER_M_PER_S
                * 1000.0 / 2.0));
        // no side or vertical speeds for this bot
        this->position3d_data.yspeed = 0;
        this->position3d_data.zspeed = 0;
        
        this->position3d_data.rollspeed =
                htonl((int32_t)rint((double)frame_->roll_dot /
                (double)RMP_COUNT_PER_DEG_PER_S * M_PI / 180 * 1000.0));
        this->position3d_data.pitchspeed =
                htonl((int32_t)rint((double)frame_->pitch_dot /
                (double)RMP_COUNT_PER_DEG_PER_S * M_PI / 180 * 1000.00));
        // from counts/sec into millirad/sec.  also, take the additive
        // inverse, since the RMP reports clockwise angular velocity as
        // positive.
        
        // This one uses left_dot and right_dot, which comes from odometry
        this->position3d_data.yawspeed =
                htonl((int32_t)(rint((double)(frame_->right_dot - frame_->left_dot) /
                (RMP_COUNT_PER_M_PER_S * RMP_GEOM_WHEEL_SEP * M_PI) * 1000)));
        // This one uses yaw_dot, which comes from the IMU
        //data.position3d_data.yawspeed =
        //  htonl((int32_t)(-rint((double)frame_->yaw_dot /
        //                        (double)RMP_COUNT_PER_DEG_PER_S * M_PI / 180 * 1000)));
        
        this->position3d_data.stall = 0;
*/

    // Chip
    //frame_->dump();
    //dump();

    firstread_ = false;
}

int
UsbDriver::resetAllIntegrators()
{
    makeStatusCommandPacket( pkt_, RMP_CMD_RESET_INTEGRATORS, RMP_CAN_RESET_ALL );

    return usbio_->writePacket(pkt_);
}

int
UsbDriver::setMaxVelocityScaleFactor( double scale )
{
    // limit input to [0.0, 1.0]
    if ( scale>1.0 ) {
        scale=1.0;
    } else if ( scale<0.0 ) {
        scale=0.0;
    }
    makeStatusCommandPacket( pkt_, RMP_CMD_SET_MAXIMUM_VELOCITY, (uint16_t)ceil(scale*16.0) );

    return usbio_->writePacket(pkt_);
}

int
UsbDriver::setMaxTurnrateScaleFactor( double scale )
{
    // limit input to [0.0, 1.0]
    if ( scale>1.0 ) {
        scale=1.0;
    } else if ( scale<0.0 ) {
        scale=0.0;
    }
    makeStatusCommandPacket( pkt_, RMP_CMD_SET_MAXIMUM_TURN_RATE, (uint16_t)ceil(scale*16.0) );

    return usbio_->writePacket(pkt_);
}

int
UsbDriver::setMaxAccelerationScaleFactor( double scale )
{
    // limit input to [0.0, 1.0]
    if ( scale>1.0 ) {
        scale=1.0;
    } else if ( scale<0.0 ) {
        scale=0.0;
    }
    makeStatusCommandPacket( pkt_, RMP_CMD_SET_MAXIMUM_ACCELERATION, (uint16_t)ceil(scale*16.0) );

    return usbio_->writePacket(pkt_);
}

int
UsbDriver::setMaxCurrentLimitScaleFactor( double scale )
{
    // limit input to [0.0, 1.0]
    if ( scale>1.0 ) {
        scale=1.0;
    } else if ( scale<0.0 ) {
        scale=0.0;
    }
    // note: the scale of this command is [0,256]
    makeStatusCommandPacket( pkt_, RMP_CMD_SET_CURRENT_LIMIT, (uint16_t)ceil(scale*256.0) );

    return usbio_->writePacket(pkt_);
}

/*
 *  Takes an Orca command object and turns it into CAN packets for the RMP
 */
void
UsbDriver::makeMotionCommandPacket( CanPacket* pkt, const Velocity2dCommandPtr & command )
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
UsbDriver::makeStatusCommandPacket( CanPacket* pkt, uint16_t commandId, uint16_t value )
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
UsbDriver::makeShutdownCommandPacket( CanPacket* pkt )
{
    pkt->id = RMP_CAN_ID_SHUTDOWN;

    //printf("SEGWAYIO: SHUTDOWN: pkt: %s\n", pkt->toString());
}

// Calculate the difference between two raw counter values, taking care
// of rollover.
int
UsbDriver::diff( uint32_t from, uint32_t to, bool first )
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
UsbDriver::watchPacket( CanPacket* pkt, short int pktID )
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
UsbDriver::watchDataStream( CanPacket* pkt )
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
