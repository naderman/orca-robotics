/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <iomanip>    // for setw

#include "rmpdataframe.h"

#include "rmpdefs.h"
#include "canpacket.h"

using namespace std;
using namespace segwayrmp;

// this holds all the RMP data it gives us
RmpDataFrame::RmpDataFrame()
{
    reset();

    kill_flag = false;
}

std::string RmpDataFrame::toString()
{
    std::ostringstream os;
    statusCheck_ ? os<<setw(17)<<"status 1"<<dec<<setw(10)<<(int)status_word1<<hex<<setw(10)<<"0x"<<status_word1<<"\t\t"<< CuStatus1ToString()<<endl :os<<"status 1   \tDROPPED"<<endl;
    statusCheck_ ? os<<setw(17)<<"status 2"<<dec<<setw(10)<<(int)status_word2<<hex<<setw(10)<<"0x"<<status_word2<<"\t\t"<< CuStatus2ToString()<<endl :os<<"status 2   \tDROPPED"<<endl;
    
    msgCheckList_[1] ? os<<setw(17)<<"pitch"<<dec<<setw(10)<<(int)pitch<<hex<<setw(10)<<pitch<<"\t\t"<<dec<<(float)pitch/RMP_COUNT_PER_DEG<<" deg"<<endl : os<<"pitch      \tDROPPED"<<endl;
    msgCheckList_[1] ? os<<setw(17)<<"pitch dot"<<dec<<setw(10)<<(int)pitch_dot<<hex<<setw(10)<<pitch_dot<<"\t\t"<<dec<<(float)pitch_dot/RMP_COUNT_PER_DEG_PER_S<<" deg/s"<<endl : os<<"pitch dot  \tDROPPED"<<endl;
    msgCheckList_[1] ? os<<setw(17)<<"roll"<<dec<<setw(10)<<(int)roll<<hex<<setw(10)<<roll<<"\t\t"<<dec<<(float)roll/RMP_COUNT_PER_DEG<<" deg"<<endl : os<<"roll       \tDROPPED"<<endl;
    msgCheckList_[1] ? os<<setw(17)<<"roll dot"<<dec<<setw(10)<<(int)roll_dot<<hex<<setw(10)<<roll_dot<<"\t\t"<<dec<<(float)roll_dot/RMP_COUNT_PER_DEG_PER_S<<" deg/s"<<endl : os<<"roll dot   \tDROPPED"<<endl;
    msgCheckList_[4] ? os<<setw(17)<<"yaw"<<dec<<setw(10)<<(int)yaw<<hex<<setw(10)<<yaw<<"\t\t"<<dec<<(float)yaw/RMP_COUNT_PER_DEG<<" deg"<<endl : os<<"yaw        \tDROPPED"<<endl;
    msgCheckList_[2] ? os<<setw(17)<<"yaw dot"<<dec<<setw(10)<<(int)yaw_dot<<hex<<setw(10)<<yaw_dot<<"\t\t"<<dec<<(float)yaw_dot/RMP_COUNT_PER_DEG_PER_S<<" deg/s"<<endl : os<<"yaw dot    \tDROPPED"<<endl;

    msgCheckList_[3] ? os<<setw(17)<<"left"<<dec<<setw(10)<<(int)left<<hex<<setw(10)<<left<<"\t\t"<<dec<<(float)left/RMP_COUNT_PER_M<<" m"<<endl : os<<"left       \tDROPPED"<<endl;
    msgCheckList_[2] ? os<<setw(17)<<"left dot"<<dec<<setw(10)<<(int)left_dot<<hex<<setw(10)<<left_dot<<"\t\t"<<dec<<(float)left_dot/RMP_COUNT_PER_M_PER_S<<" m/s"<<endl : os<<"left dot   \tDROPPED"<<endl;
    msgCheckList_[3] ? os<<setw(17)<<"right"<<dec<<setw(10)<<(int)right<<hex<<setw(10)<<right<<"\t\t"<<dec<<(float)right/RMP_COUNT_PER_M<<" m"<<endl : os<<"right      \tDROPPED"<<endl;
    msgCheckList_[2] ? os<<setw(17)<<"right dot"<<dec<<setw(10)<<(int)right_dot<<hex<<setw(10)<<right_dot<<"\t\t"<<dec<<(float)right_dot/RMP_COUNT_PER_M_PER_S<<" m/s"<<endl : os<<"right dot  \tDROPPED"<<endl;
    msgCheckList_[4] ? os<<setw(17)<<"fore-aft"<<dec<<setw(10)<<(int)foreaft<<hex<<setw(10)<<foreaft<<"\t\t"<<dec<<(float)foreaft/RMP_COUNT_PER_M<<" m"<<endl : os<<"fore-aft   \tDROPPED"<<endl;

    msgCheckList_[5] ? os<<setw(17)<<"left torque"<<dec<<setw(10)<<(int)left_torque<<hex<<setw(10)<<left_torque<<"\t\t"<<dec<<(float)left_torque/RMP_COUNT_PER_NM<<" Nm"<<endl : os<<"left torque\tDROPPED"<<endl;
    msgCheckList_[5] ? os<<setw(17)<<"right torque"<<dec<<setw(10)<<(int)right_torque<<hex<<setw(10)<<right_torque<<"\t\t"<<dec<<(float)right_torque/RMP_COUNT_PER_NM<<" Nm"<<endl : os<<"right torqu\tDROPPED"<<endl;

    msgCheckList_[6] ? os<<setw(17)<<"ui battery"<<dec<<setw(10)<<(int)ui_battery_voltage<<hex<<setw(10)<<ui_battery_voltage<<"\t\t"<<dec<<(float)RMP_UI_OFFSET + ui_battery_voltage*RMP_UI_COEFF<<" V"<<endl : os<<"ui battery \tDROPPED"<<endl;
    msgCheckList_[6] ? os<<setw(17)<<"cu battery"<<dec<<setw(10)<<(int)base_battery_voltage<<hex<<setw(10)<<base_battery_voltage<<"\t\t"<<dec<<(float)base_battery_voltage/RMP_BASE_COUNT_PER_VOLT<<" V"<<endl : os<<"cu battery \tDROPPED"<<endl;

    msgCheckList_[7] ? os<<setw(17)<<"vel command"<<dec<<setw(10)<<(int)velocity_command<<hex<<setw(10)<<velocity_command<<"\t\t"<<dec<<(float)velocity_command/RMP_COUNT_PER_M_PER_S<<" m/s"<<endl : os<<"vel command\tDROPPED"<<endl;
    msgCheckList_[7] ? os<<setw(17)<<"turn command"<<dec<<setw(10)<<(int)rate_command<<hex<<setw(10)<<rate_command<<"\t\t"<<dec<<(float)rate_command/RMP_COUNT_PER_DEG_PER_S<<" deg/s"<<endl : os<<"turn comman\tDROPPED"<<endl;

    msgCheckList_[6] ? os<<setw(17)<<"op mode"<<dec<<setw(10)<<(int)operational_mode<<hex<<setw(10)<<operational_mode<<dec<<endl : os<<"op mode    \tDROPPED"<<endl;
    msgCheckList_[6] ? os<<setw(17)<<"gain schedule"<<dec<<setw(10)<<(int)controller_gain_schedule<<hex<<setw(10)<<controller_gain_schedule<<dec<<endl : os<<"gain sched \tDROPPED"<<endl;
    msgCheckList_[2] ? os<<setw(17)<<"frames"<<dec<<setw(10)<<(int)frames<<hex<<setw(10)<<frames<<dec<<endl : os<<"frames     \tDROPPED"<<endl;
    msgCheckList_[0] ? os<<setw(17)<<"firmware build id"<<dec<<setw(10)<<(int)build_id<<hex<<setw(10)<<build_id<<dec<<endl : os<<"build id   \tDROPPED"<<endl;
    return os.str();
}

std::string RmpDataFrame::CuStatus1ToString()
{
    std::string s;
    if ( status_word1 & WORD1_SAFETY_SHUTDOWN_1 ) s += std::string(WORD1_SAFETY_SHUTDOWN_1_STRING) + "; ";
    //if ( status_word1 & WORD1_NOT_USED_0002 ) s += std::string(WORD1_NOT_USED_0002_STRING) + "; ";
    if ( status_word1 & WORD1_SAFETY_SHUTDOWN_2 ) s += std::string(WORD1_SAFETY_SHUTDOWN_2_STRING) + "; ";
    if ( status_word1 & WORD1_DISABLE_RESPONSE ) s += std::string(WORD1_DISABLE_RESPONSE_STRING) + "; ";
    if ( status_word1 & WORD1_SPEED_LIMIT_4MPH ) s += std::string(WORD1_SPEED_LIMIT_4MPH_STRING) + "; ";
    if ( status_word1 & WORD1_SPEED_LIMIT_0MPH ) s += std::string(WORD1_SPEED_LIMIT_0MPH_STRING) + "; ";
    if ( status_word1 & WORD1_SYSTEM_INTEGRITY_FAULT ) s += std::string(WORD1_SYSTEM_INTEGRITY_FAULT_STRING) + "; ";
    if ( status_word1 & WORD1_MOTORS_ENABLED ) s += std::string(WORD1_MOTORS_ENABLED_STRING) + "; ";
    if ( status_word1 & WORD1_BALANCE_MODE ) s += std::string(WORD1_BALANCE_MODE_STRING) + "; ";
    if ( status_word1 & WORD1_YAW_TRANSIENT_FLAG ) s += std::string(WORD1_YAW_TRANSIENT_FLAG_STRING) + "; ";
    if ( status_word1 & WORD1_EMPTY_BATTERY_HAZARD ) s += std::string(WORD1_EMPTY_BATTERY_HAZARD_STRING) + "; ";
    //if ( status_word1 & WORD1_NOT_USED_0800 ) s += std::string(WORD1_NOT_USED_0800_STRING) + "; ";
    if ( status_word1 & WORD1_WHEEL_SLIP_FLAG ) s += std::string(WORD1_WHEEL_SLIP_FLAG_STRING) + "; ";
    if ( status_word1 & WORD1_MOTOR_FEEDBACK_FAULT ) s += std::string(WORD1_MOTOR_FEEDBACK_FAULT_STRING) + "; ";
    if ( status_word1 & WORD1_LOW_BATTRY_HAZARD ) s += std::string(WORD1_LOW_BATTRY_HAZARD_STRING) + "; ";
    //if ( status_word1 & WORD1_NOT_USED_8000 ) s += std::string(WORD1_NOT_USED_8000_STRING) + "; ";

    return s;
}

std::string RmpDataFrame::CuStatus2ToString()
{
    std::string s;
    if ( status_word2 & WORD2_FOREAFT_PITCH_ANGLE_LIMIT_A ) s += std::string(WORD2_FOREAFT_PITCH_ANGLE_LIMIT_A_STRING) + "; ";
    if ( status_word2 & WORD2_ESTOP_SWITCH_OPEN_A ) s += std::string(WORD2_ESTOP_SWITCH_OPEN_A_STRING) + "; ";
    if ( status_word2 & WORD2_POSITION_ERROR_LIMIT_A ) s += std::string(WORD2_POSITION_ERROR_LIMIT_A_STRING) + "; ";
    if ( status_word2 & WORD2_MAX_VELOCITY_A ) s += std::string(WORD2_MAX_VELOCITY_A_STRING) + "; ";
    if ( status_word2 & WORD2_BATTERY_EMPTY_A ) s += std::string(WORD2_BATTERY_EMPTY_A_STRING) + "; ";
    if ( status_word2 & WORD2_BATTERY_TEMP_LIMIT_A ) s += std::string(WORD2_BATTERY_TEMP_LIMIT_A_STRING) + "; ";
    if ( status_word2 & WORD2_ROLL_ANGLE_LIMIT_A ) s += std::string(WORD2_ROLL_ANGLE_LIMIT_A_STRING) + "; ";
    if ( status_word2 & WORD2_LOW_BATTERY_BUS_VOLTAGE_A ) s += std::string(WORD2_LOW_BATTERY_BUS_VOLTAGE_A_STRING) + "; ";
    if ( status_word2 & WORD2_FOREAFT_PITCH_ANGLE_LIMIT_B ) s += std::string(WORD2_FOREAFT_PITCH_ANGLE_LIMIT_B_STRING) + "; ";
    if ( status_word2 & WORD2_ESTOP_SWITCH_OPEN_B ) s += std::string(WORD2_ESTOP_SWITCH_OPEN_B_STRING) + "; ";
    if ( status_word2 & WORD2_POSITION_ERROR_LIMIT_B ) s += std::string(WORD2_POSITION_ERROR_LIMIT_B_STRING) + "; ";
    if ( status_word2 & WORD2_MAX_VELOCITY_B ) s += std::string(WORD2_MAX_VELOCITY_B_STRING) + "; ";
    if ( status_word2 & WORD2_BATTERY_EMPTY_B ) s += std::string(WORD2_BATTERY_EMPTY_B_STRING) + "; ";
    if ( status_word2 & WORD2_BATTERY_TEMP_LIMIT_B ) s += std::string(WORD2_BATTERY_TEMP_LIMIT_B_STRING) + "; ";
    if ( status_word2 & WORD2_ROLL_ANGLE_LIMIT_B ) s += std::string(WORD2_ROLL_ANGLE_LIMIT_B_STRING) + "; ";
    if ( status_word2 & WORD2_LOW_BATTERY_BUS_VOLTAGE_B ) s += std::string(WORD2_LOW_BATTERY_BUS_VOLTAGE_B_STRING) + "; ";

    return s;
}

bool
RmpDataFrame::isWarn()
{
    return ( status_word1 & WORD1_DISABLE_RESPONSE          || // Not sure what this one is...
             status_word1 & WORD1_YAW_TRANSIENT_FLAG        || //Not sure what this one is...
             status_word1 & WORD1_WHEEL_SLIP_FLAG           ||
             status_word1 & WORD1_MOTOR_FEEDBACK_FAULT      ||
             status_word1 & WORD1_LOW_BATTRY_HAZARD         ||
             status_word2 & WORD2_MAX_VELOCITY_A            || // Not sure if this is significant or not...
             status_word2 & WORD2_LOW_BATTERY_BUS_VOLTAGE_A ||
             status_word2 & WORD2_MAX_VELOCITY_B            ||
             status_word2 & WORD2_LOW_BATTERY_BUS_VOLTAGE_B );
}

bool 
RmpDataFrame::isFault()
{
    return ( status_word1 & WORD1_SAFETY_SHUTDOWN_1           ||
             status_word1 & WORD1_SAFETY_SHUTDOWN_2           ||
             status_word1 & WORD1_SYSTEM_INTEGRITY_FAULT      ||
             status_word1 & WORD1_EMPTY_BATTERY_HAZARD        ||
             status_word2 & WORD2_FOREAFT_PITCH_ANGLE_LIMIT_A ||
             status_word2 & WORD2_ESTOP_SWITCH_OPEN_A         ||
             status_word2 & WORD2_POSITION_ERROR_LIMIT_A      ||
             status_word2 & WORD2_BATTERY_EMPTY_A             ||
             status_word2 & WORD2_BATTERY_TEMP_LIMIT_A        ||
             status_word2 & WORD2_ROLL_ANGLE_LIMIT_A          ||
             status_word2 & WORD2_FOREAFT_PITCH_ANGLE_LIMIT_B ||
             status_word2 & WORD2_ESTOP_SWITCH_OPEN_B         ||
             status_word2 & WORD2_POSITION_ERROR_LIMIT_B      ||
             status_word2 & WORD2_BATTERY_EMPTY_B             ||
             status_word2 & WORD2_BATTERY_TEMP_LIMIT_B        ||
             status_word2 & WORD2_ROLL_ANGLE_LIMIT_B          );
}

void RmpDataFrame::reopen()
{
    msgCheckList_[0] = false;
}

bool RmpDataFrame::isClosed() const
{
    // The only thing that matters: did we receive MSG0?
    return msgCheckList_[0];
}

void RmpDataFrame::reset()
{
    for ( int i=0; i<8; ++i ) {
        msgCheckList_[i] = false;
    }
    statusCheck_ = false;
}

bool RmpDataFrame::isComplete() const
{
    bool complete = true;
    for ( int i=0; i<8; ++i ) {
        complete = (complete && msgCheckList_[i]);
    }
    return complete && statusCheck_;
}

/*
 * Takes a CAN packet from the RMP and parses it into a RmpDataFrame struct.
 */
void RmpDataFrame::AddPacket(const CanPacket* pkt)
{
    switch(pkt->id)
    {
    case RMP_CAN_ID_STATUS:
    {
        // slot0 : controller mode and wake control
        status_word1 = pkt->GetSlot(1);
        status_word2 =  pkt->GetSlot(2);
        // slot3 : undefined
        
        statusCheck_ = true;

        //debug
        //std::cout<<"got status: "<<status_word1<<" "<<status_word2<<std::endl;

        //debug
        /*
        if ( kill_flag ) {
            std::cout<<"exiting because of CU status"<<std::endl;
            std::cout<<"CU status: "<<std::hex<<status_word1<<" "<<status_word2<<std::endl;
            exit(0);
        }
        */
        //if ( status_word1==16512 ) {
/*
        if ( status_word1!=128 && status_word1!=384 ) {  // 384=0x0180 128=0x0080
            std::cout<<"CU status: "<<std::hex<<status_word1<<" "<<status_word2<<std::endl;
            kill_flag = true;
        }
*/
        break;
    }
/*    
    // we don't get this message!
    case RMP_CAN_ID_HEARTBEAT:
    {
        // slot0 (byte 2) : CAN channel identifier
        // slot1 : UI battery voltage (we also get in MSG6 from CU)
        // slot2 : UI battery status
        // @todo have to apply mask here
        uint16_t slot2 = (uint16_t)pkt->GetSlot(2);
        ui_heartbeat_voltage =  slot2 & RMP_CAN_MASK_HEARTBEAT_UIBAT_VOLTAGE;
        ui_heartbeat_status =  slot2 & RMP_CAN_MASK_HEARTBEAT_UIBAT_STATUS;
        std::cout<<"heartbeat:slot2 : "<<slot2<<std::endl;
        // slot3 : undefined
        break;
    }
*/    
    case RMP_CAN_ID_MSG0:
    {
        build_id = pkt->GetSlot(0);
        // slot1 undefined
        // slot2 undefined
        // slot3 undefined

        // mark current frame finished, closed, or 'ready'
        msgCheckList_[0] = true;
        break;
    }
    case RMP_CAN_ID_MSG1:
    {
        pitch = pkt->GetSlot(0);
        pitch_dot = pkt->GetSlot(1);
        roll =  pkt->GetSlot(2);
        roll_dot =  pkt->GetSlot(3);

        msgCheckList_[1] = true;
        break;
    }
    case RMP_CAN_ID_MSG2:
    {
        left_dot = (int16_t) pkt->GetSlot(0);
        right_dot = (int16_t) pkt->GetSlot(1);
        yaw_dot = (int16_t) pkt->GetSlot(2);
        frames = pkt->GetSlot(3);

        msgCheckList_[2] = true;
        break;
    }
    case RMP_CAN_ID_MSG3:
    {
        left = (uint32_t)(((uint32_t)pkt->GetSlot(1) << 16) |
                    (uint32_t)pkt->GetSlot(0));
        right = (uint32_t)(((uint32_t)pkt->GetSlot(3) << 16) |
                (uint32_t)pkt->GetSlot(2));

        msgCheckList_[3] = true;
        break;
    }
    case RMP_CAN_ID_MSG4:
    {
        foreaft = (uint32_t)(((uint32_t)pkt->GetSlot(1) << 16) |
                        (uint32_t)pkt->GetSlot(0));
        yaw = (uint32_t)(((uint32_t)pkt->GetSlot(3) << 16) |
                (uint32_t)pkt->GetSlot(2));

        msgCheckList_[4] = true;
        break;
    }
    case RMP_CAN_ID_MSG5:
    {
        left_torque = (int16_t) pkt->GetSlot(0);
        right_torque = (int16_t) pkt->GetSlot(1);
        // slot2 undefined
        // slot3 undefined

        msgCheckList_[5] = true;
        break;
    }
    case RMP_CAN_ID_MSG6:
    {
        operational_mode = (uint8_t) pkt->GetSlot(0);
        controller_gain_schedule = (uint8_t) pkt->GetSlot(1);
        ui_battery_voltage = (uint16_t) pkt->GetSlot(2);
        base_battery_voltage = (uint16_t)pkt->GetSlot(3);

        msgCheckList_[6] = true;
        break;
    }
    case RMP_CAN_ID_MSG7:
    {
        velocity_command = (int16_t) pkt->GetSlot(0);
        rate_command = (int16_t) pkt->GetSlot(1);
        // slot2 undefined
        // slot3 undefined

        msgCheckList_[7] = true;
        break;
    }
    default:
    {
        // did not recognize the message
        break;
    }
  }

}
