#include "rawrxdata.h"
#include <iostream>
#include "rmpdefs.h"
#include <iomanip>

using namespace std;

namespace segwayrmp {

RawRxData::RawRxData( const orcaice::Context &context )
    : context_(context)
{
    haveStatusMsg_ = false;
    for ( int i=0; i<8; ++i ) {
        haveMonitorMsg_[i] = false;
    }
}

bool 
RawRxData::isComplete() const
{
    if ( !haveStatusMsg_ ) return false;
    for ( int i=0; i<8; ++i ) 
    {
        if ( !haveMonitorMsg_[i] ) return false;
    }
    return true;
}

void
RawRxData::addPacket( const CanPacket &pkt )
{
    switch(pkt.id())
    {
    case RMP_CAN_ID_STATUS:
    {
        // slot0 : controller mode and wake control
        //         alexb: why not read the controller mode here?
        status_word1 = pkt.getSlot(1);
        status_word2 = pkt.getSlot(2);
        // slot3 : not used

        haveStatusMsg_ = true;
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
        uint16_t slot2 = (uint16_t)pkt.getSlot(2);
        ui_heartbeat_voltage =  slot2 & RMP_CAN_MASK_HEARTBEAT_UIBAT_VOLTAGE;
        ui_heartbeat_status =  slot2 & RMP_CAN_MASK_HEARTBEAT_UIBAT_STATUS;
        std::cout<<"heartbeat:slot2 : "<<slot2<<std::endl;
        // slot3 : undefined
        break;
    }
*/    
    case RMP_CAN_ID_MSG0:
    {
        build_id = pkt.getSlot(0);
        // slot1 undefined
        // slot2 undefined
        // slot3 undefined

        // mark current frame finished, closed, or 'ready'
        haveMonitorMsg_[0] = true;
        break;
    }
    case RMP_CAN_ID_MSG1:
    {
        pitch     = pkt.getSlot(0);
        pitch_dot = pkt.getSlot(1);
        roll      = pkt.getSlot(2);
        roll_dot  = pkt.getSlot(3);

        haveMonitorMsg_[1] = true;
        break;
    }
    case RMP_CAN_ID_MSG2:
    {
        left_wheel_velocity  = (int16_t) pkt.getSlot(0);
        right_wheel_velocity = (int16_t) pkt.getSlot(1);
        yaw_dot              = (int16_t) pkt.getSlot(2);
        servo_frames         = pkt.getSlot(3);

        haveMonitorMsg_[2] = true;
        break;
    }
    case RMP_CAN_ID_MSG3:
    {
        left_wheel_displacement = (uint32_t)(((uint32_t)pkt.getSlot(1) << 16) |
                                             (uint32_t)pkt.getSlot(0));
        right_wheel_displacement = (uint32_t)(((uint32_t)pkt.getSlot(3) << 16) |
                                              (uint32_t)pkt.getSlot(2));

        haveMonitorMsg_[3] = true;
        break;
    }
    case RMP_CAN_ID_MSG4:
    {
        foreaft_displacement = (uint32_t)(((uint32_t)pkt.getSlot(1) << 16) |
                                          (uint32_t)pkt.getSlot(0));
        yaw = (uint32_t)(((uint32_t)pkt.getSlot(3) << 16) |
                         (uint32_t)pkt.getSlot(2));

        haveMonitorMsg_[4] = true;
        break;
    }
    case RMP_CAN_ID_MSG5:
    {
        left_torque = (int16_t) pkt.getSlot(0);
        right_torque = (int16_t) pkt.getSlot(1);
        // slot2 undefined
        // slot3 undefined

        haveMonitorMsg_[5] = true;
        break;
    }
    case RMP_CAN_ID_MSG6:
    {
        operational_mode = (uint8_t) pkt.getSlot(0);
        controller_gain_schedule = (uint8_t) pkt.getSlot(1);
        ui_battery_voltage = (uint16_t) pkt.getSlot(2);
        base_battery_voltage = (uint16_t)pkt.getSlot(3);

        haveMonitorMsg_[6] = true;
        break;
    }
    case RMP_CAN_ID_MSG7:
    {
        received_velocity_command = (int16_t) pkt.getSlot(0);
        received_turnrate_command = (int16_t) pkt.getSlot(1);
        // slot2 undefined
        // slot3 undefined

        haveMonitorMsg_[7] = true;
        break;
    }
    default:
    {
        stringstream ss;
        ss << "Received CAN packet with id 0x"<<std::hex<<pkt.id()<<".  Don't know what to do, ignoring.";
         context_.tracer()->warning( ss.str() );
        break;
    }
    }
}

std::string 
RawRxData::toString() const
{
    std::ostringstream os;
    haveStatusMsg_ ? os<<setw(17)<<"status 1"<<dec<<setw(10)<<(int)status_word1<<hex<<setw(10)<<"0x"<<status_word1<<"\t\t"<< CuStatus1ToString()<<endl :os<<"status 1   \tDROPPED"<<endl;
    haveStatusMsg_ ? os<<setw(17)<<"status 2"<<dec<<setw(10)<<(int)status_word2<<hex<<setw(10)<<"0x"<<status_word2<<"\t\t"<< CuStatus2ToString()<<endl :os<<"status 2   \tDROPPED"<<endl;
    
    haveMonitorMsg_[1] ? os<<setw(17)<<"pitch"<<dec<<setw(10)<<(int)pitch<<hex<<setw(10)<<pitch<<"\t\t"<<dec<<(float)pitch/RMP_COUNT_PER_DEG<<" deg"<<endl : os<<"pitch      \tDROPPED"<<endl;
    haveMonitorMsg_[1] ? os<<setw(17)<<"pitch dot"<<dec<<setw(10)<<(int)pitch_dot<<hex<<setw(10)<<pitch_dot<<"\t\t"<<dec<<(float)pitch_dot/RMP_COUNT_PER_DEG_PER_S<<" deg/s"<<endl : os<<"pitch dot  \tDROPPED"<<endl;
    haveMonitorMsg_[1] ? os<<setw(17)<<"roll"<<dec<<setw(10)<<(int)roll<<hex<<setw(10)<<roll<<"\t\t"<<dec<<(float)roll/RMP_COUNT_PER_DEG<<" deg"<<endl : os<<"roll       \tDROPPED"<<endl;
    haveMonitorMsg_[1] ? os<<setw(17)<<"roll dot"<<dec<<setw(10)<<(int)roll_dot<<hex<<setw(10)<<roll_dot<<"\t\t"<<dec<<(float)roll_dot/RMP_COUNT_PER_DEG_PER_S<<" deg/s"<<endl : os<<"roll dot   \tDROPPED"<<endl;
    haveMonitorMsg_[4] ? os<<setw(17)<<"yaw"<<dec<<setw(10)<<(int)yaw<<hex<<setw(10)<<yaw<<"\t\t"<<dec<<(float)yaw/RMP_COUNT_PER_DEG<<" deg"<<endl : os<<"yaw        \tDROPPED"<<endl;
    haveMonitorMsg_[2] ? os<<setw(17)<<"yaw dot"<<dec<<setw(10)<<(int)yaw_dot<<hex<<setw(10)<<yaw_dot<<"\t\t"<<dec<<(float)yaw_dot/RMP_COUNT_PER_DEG_PER_S<<" deg/s"<<endl : os<<"yaw dot    \tDROPPED"<<endl;

    haveMonitorMsg_[3] ? os<<setw(17)<<"left_wheel_displacement"<<dec<<setw(10)<<(int)left_wheel_displacement<<hex<<setw(10)<<left_wheel_displacement<<"\t\t"<<dec<<(float)left_wheel_displacement/RMP_COUNT_PER_M_RMP200<<" m"<<endl : os<<"left_wheel_displacement       \tDROPPED"<<endl;
    haveMonitorMsg_[2] ? os<<setw(17)<<"left_wheel_velocity"<<dec<<setw(10)<<(int)left_wheel_velocity<<hex<<setw(10)<<left_wheel_velocity<<"\t\t"<<dec<<(float)left_wheel_velocity/RMP_COUNT_PER_M_PER_S_RMP200<<" m/s"<<endl : os<<"left_wheel_velocity   \tDROPPED"<<endl;
    haveMonitorMsg_[3] ? os<<setw(17)<<"right_wheel_displacement"<<dec<<setw(10)<<(int)right_wheel_displacement<<hex<<setw(10)<<right_wheel_displacement<<"\t\t"<<dec<<(float)right_wheel_displacement/RMP_COUNT_PER_M_RMP200<<" m"<<endl : os<<"right_wheel_displacement      \tDROPPED"<<endl;
    haveMonitorMsg_[2] ? os<<setw(17)<<"right_wheel_velocity"<<dec<<setw(10)<<(int)right_wheel_velocity<<hex<<setw(10)<<right_wheel_velocity<<"\t\t"<<dec<<(float)right_wheel_velocity/RMP_COUNT_PER_M_PER_S_RMP200<<" m/s"<<endl : os<<"right_wheel_velocity  \tDROPPED"<<endl;
    haveMonitorMsg_[4] ? os<<setw(17)<<"foreaft_displacement"<<dec<<setw(10)<<(int)foreaft_displacement<<hex<<setw(10)<<foreaft_displacement<<"\t\t"<<dec<<(float)foreaft_displacement/RMP_COUNT_PER_M_RMP200<<" m"<<endl : os<<"foreaft_displacement   \tDROPPED"<<endl;

    haveMonitorMsg_[5] ? os<<setw(17)<<"left torque"<<dec<<setw(10)<<(int)left_torque<<hex<<setw(10)<<left_torque<<"\t\t"<<dec<<(float)left_torque/RMP_COUNT_PER_NM_RMP200<<" Nm"<<endl : os<<"left torque\tDROPPED"<<endl;
    haveMonitorMsg_[5] ? os<<setw(17)<<"right torque"<<dec<<setw(10)<<(int)right_torque<<hex<<setw(10)<<right_torque<<"\t\t"<<dec<<(float)right_torque/RMP_COUNT_PER_NM_RMP200<<" Nm"<<endl : os<<"right torqu\tDROPPED"<<endl;

    haveMonitorMsg_[6] ? os<<setw(17)<<"ui battery"<<dec<<setw(10)<<(int)ui_battery_voltage<<hex<<setw(10)<<ui_battery_voltage<<"\t\t"<<dec<<(float)RMP_UI_OFFSET + ui_battery_voltage*RMP_UI_COEFF<<" V"<<endl : os<<"ui battery \tDROPPED"<<endl;
    haveMonitorMsg_[6] ? os<<setw(17)<<"cu battery"<<dec<<setw(10)<<(int)base_battery_voltage<<hex<<setw(10)<<base_battery_voltage<<"\t\t"<<dec<<(float)base_battery_voltage/RMP_BASE_COUNT_PER_VOLT<<" V"<<endl : os<<"cu battery \tDROPPED"<<endl;

    haveMonitorMsg_[7] ? os<<setw(17)<<"received_velocity_command"<<dec<<setw(10)<<(int)received_velocity_command<<hex<<setw(10)<<received_velocity_command<<"\t\t"<<dec<<(float)received_velocity_command/RMP_COUNT_PER_M_PER_S_RMP200<<" m/s"<<endl : os<<"received_velocity_command\tDROPPED"<<endl;
    haveMonitorMsg_[7] ? os<<setw(17)<<"received_turnrate_command"<<dec<<setw(10)<<(int)received_turnrate_command<<hex<<setw(10)<<received_turnrate_command<<"\t\t"<<dec<<(float)received_turnrate_command/RMP_COUNT_PER_DEG_PER_S<<" deg/s"<<endl : os<<"received_turnrate_command\tDROPPED"<<endl;

    haveMonitorMsg_[6] ? os<<setw(17)<<"op mode"<<dec<<setw(10)<<(int)operational_mode<<hex<<setw(10)<<operational_mode<<dec<<endl : os<<"op mode    \tDROPPED"<<endl;
    haveMonitorMsg_[6] ? os<<setw(17)<<"gain schedule"<<dec<<setw(10)<<(int)controller_gain_schedule<<hex<<setw(10)<<controller_gain_schedule<<dec<<endl : os<<"gain sched \tDROPPED"<<endl;
    haveMonitorMsg_[2] ? os<<setw(17)<<"servo_frames"<<dec<<setw(10)<<(int)servo_frames<<hex<<setw(10)<<servo_frames<<dec<<endl : os<<"servo_frames     \tDROPPED"<<endl;
    haveMonitorMsg_[0] ? os<<setw(17)<<"firmware build id"<<dec<<setw(10)<<(int)build_id<<hex<<setw(10)<<build_id<<dec<<endl : os<<"build id   \tDROPPED"<<endl;
    return os.str();
}

std::string
RawRxData::CuStatus1ToString() const
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

std::string
RawRxData::CuStatus2ToString() const
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
RawRxData::isWarn() const
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
RawRxData::isFault() const
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


}

