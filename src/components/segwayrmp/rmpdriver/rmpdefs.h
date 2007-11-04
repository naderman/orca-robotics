/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_SEGWAY_RMP_CONFIGURATION_H
#define ORCA2_SEGWAY_RMP_CONFIGURATION_H

#include <string>

// MESSAGE ID'S

// Segway->CPU messages
// warning: Segway manual refers to them as Message 1-8
// we call them Message 0-7 to synch with 0x0400-407
// 0x0400 = 1024
#define RMP_CAN_ID_MSG0         0x0400
#define RMP_CAN_ID_MSG1         0x0401
#define RMP_CAN_ID_MSG2         0x0402
#define RMP_CAN_ID_MSG3         0x0403
#define RMP_CAN_ID_MSG4         0x0404
#define RMP_CAN_ID_MSG5         0x0405
#define RMP_CAN_ID_MSG6         0x0406
#define RMP_CAN_ID_MSG7         0x0407

// CPU->Segway message
#define RMP_CAN_ID_SHUTDOWN     0x0412

// CPU->Segway message
#define RMP_CAN_ID_COMMAND      0x0413

// CU->UI: CU Status message
#define RMP_CAN_ID_STATUS       0x0680

// UI->CU: RMP Heartbeat message
#define RMP_CAN_ID_HEARTBEAT    0x0688

// DATA MASKS

// Mask for user interface battery voltage in the heartbeat message.
#define RMP_CAN_MASK_HEARTBEAT_UIBAT_VOLTAGE    0x03F0

// Mask for user interface battery status in the heartbeat message.
#define RMP_CAN_MASK_HEARTBEAT_UIBAT_STATUS     0xC000

// from table: Bitfield for Reset Integrators, see sec.2.2.2
#define RMP_CAN_RESET_RIGHT           0x01
#define RMP_CAN_RESET_LEFT            0x02
#define RMP_CAN_RESET_YAW             0x04
#define RMP_CAN_RESET_FOREAFT         0x08
#define RMP_CAN_RESET_ALL             (RMP_CAN_RESET_RIGHT | \
                                      RMP_CAN_RESET_LEFT | \
                                      RMP_CAN_RESET_YAW | \
                                      RMP_CAN_RESET_FOREAFT)

// CU STATUS WORD DEFINITIONS, see sec.2.4.1
#define WORD1_SAFETY_SHUTDOWN_1                 0x0001
#define WORD1_SAFETY_SHUTDOWN_1_STRING          "Safety Shutdown, type 1"
#define WORD1_NOT_USED_0002                     0x0002
#define WORD1_NOT_USED_0002_STRING              "Not Used"
#define WORD1_SAFETY_SHUTDOWN_2                 0x0004
#define WORD1_SAFETY_SHUTDOWN_2_STRING          "Safety Shutdown, type 2"
#define WORD1_DISABLE_RESPONSE                  0x0008
#define WORD1_DISABLE_RESPONSE_STRING           "Disable Response"
#define WORD1_SPEED_LIMIT_4MPH                  0x0010
#define WORD1_SPEED_LIMIT_4MPH_STRING           "Speed Limit to 4 mph"
#define WORD1_SPEED_LIMIT_0MPH                  0x0020
#define WORD1_SPEED_LIMIT_0MPH_STRING           "Speed Limit to 0 mph"
#define WORD1_SYSTEM_INTEGRITY_FAULT            0x0040
#define WORD1_SYSTEM_INTEGRITY_FAULT_STRING     "System Integrity Fault"
#define WORD1_MOTORS_ENABLED                    0x0080
#define WORD1_MOTORS_ENABLED_STRING             "Motors Enabled"
#define WORD1_BALANCE_MODE                      0x0100
#define WORD1_BALANCE_MODE_STRING               "Balance Mode"
#define WORD1_YAW_TRANSIENT_FLAG                0x0200
#define WORD1_YAW_TRANSIENT_FLAG_STRING         "Yaw Transient Flag"
#define WORD1_EMPTY_BATTERY_HAZARD              0x0400
#define WORD1_EMPTY_BATTERY_HAZARD_STRING       "Empty Battery Hazard"
#define WORD1_NOT_USED_0800                     0x0800
#define WORD1_NOT_USED_0800_STRING              "Not Used"
#define WORD1_WHEEL_SLIP_FLAG                   0x1000
#define WORD1_WHEEL_SLIP_FLAG_STRING            "Wheel Slip Flag"
#define WORD1_MOTOR_FEEDBACK_FAULT              0x2000
#define WORD1_MOTOR_FEEDBACK_FAULT_STRING       "Motor Feedback Fault"
#define WORD1_LOW_BATTRY_HAZARD                 0x4000
#define WORD1_LOW_BATTRY_HAZARD_STRING          "Low Battery Hazard"
#define WORD1_NOT_USED_8000                     0x8000
#define WORD1_NOT_USED_8000_STRING              "Not Used"


#define WORD2_FOREAFT_PITCH_ANGLE_LIMIT_A       0x0001
#define WORD2_FOREAFT_PITCH_ANGLE_LIMIT_A_STRING "Fore/Aft Pitch Angle Limit Exceeded (A)"
#define WORD2_ESTOP_SWITCH_OPEN_A               0x0002
#define WORD2_ESTOP_SWITCH_OPEN_A_STRING        "E-Stop Switch Open (A)"
#define WORD2_POSITION_ERROR_LIMIT_A            0x0004
#define WORD2_POSITION_ERROR_LIMIT_A_STRING     "Position Error Limit Exceeded (A)"
#define WORD2_MAX_VELOCITY_A                    0x0008
#define WORD2_MAX_VELOCITY_A_STRING             "Maximum Velocity Exceeded (A)"
#define WORD2_BATTERY_EMPTY_A                   0x0010
#define WORD2_BATTERY_EMPTY_A_STRING            "Battery Empty (A)"
#define WORD2_BATTERY_TEMP_LIMIT_A              0x0020
#define WORD2_BATTERY_TEMP_LIMIT_A_STRING       "Battery Temperature Out of Range (A)"
#define WORD2_ROLL_ANGLE_LIMIT_A                0x0040
#define WORD2_ROLL_ANGLE_LIMIT_A_STRING         "Roll Angle Limit Exceeded (A)"
#define WORD2_LOW_BATTERY_BUS_VOLTAGE_A         0x0080
#define WORD2_LOW_BATTERY_BUS_VOLTAGE_A_STRING  "Low Battery Bus Voltage (A)"
#define WORD2_FOREAFT_PITCH_ANGLE_LIMIT_B       0x0100
#define WORD2_FOREAFT_PITCH_ANGLE_LIMIT_B_STRING "Fore/Aft Pitch Angle Limit Exceeded (B)"
#define WORD2_ESTOP_SWITCH_OPEN_B               0x0200
#define WORD2_ESTOP_SWITCH_OPEN_B_STRING        "E-Stop Switch Open (B)"
#define WORD2_POSITION_ERROR_LIMIT_B            0x0400
#define WORD2_POSITION_ERROR_LIMIT_B_STRING     "Position Error Limit Exceeded (B)"
#define WORD2_MAX_VELOCITY_B                    0x0800
#define WORD2_MAX_VELOCITY_B_STRING             "Maximum Velocity Exceeded (B)"
#define WORD2_BATTERY_EMPTY_B                   0x1000
#define WORD2_BATTERY_EMPTY_B_STRING            "Battery Empty (B)"
#define WORD2_BATTERY_TEMP_LIMIT_B              0x2000
#define WORD2_BATTERY_TEMP_LIMIT_B_STRING       "Battery Temperature Out of Range (B)"
#define WORD2_ROLL_ANGLE_LIMIT_B                0x4000
#define WORD2_ROLL_ANGLE_LIMIT_B_STRING         "Roll Angle Limit Exceeded (B)"
#define WORD2_LOW_BATTERY_BUS_VOLTAGE_B         0x8000
#define WORD2_LOW_BATTERY_BUS_VOLTAGE_B_STRING  "Low Battery Bus Voltage (B)"

// gain schedule for the balance controller
#define RMP_GAIN_SCHEDULE_NORMAL    0
#define RMP_GAIN_SCHEDULE_TALL      1
#define RMP_GAIN_SCHEDULE_HEAVY     2

// unit conversions from Data Reference, see sec.4

// different models
#define RMP_COUNT_PER_M_RMP50      40181.0
#define RMP_COUNT_PER_M_RMP100     40181.0
#define RMP_COUNT_PER_M_RMP200     33215.0
#define RMP_COUNT_PER_M_RMP400     33215.0

#define RMP_COUNT_PER_M_PER_S_RMP50  401.0
#define RMP_COUNT_PER_M_PER_S_RMP100 401.0
#define RMP_COUNT_PER_M_PER_S_RMP200 332.0
#define RMP_COUNT_PER_M_PER_S_RMP400 332.0

#define RMP_COUNT_PER_DEG          7.8
#define RMP_COUNT_PER_DEG_PER_S    7.8
#define RMP_COUNT_PER_DEG_PER_SS   7.8
#define RMP_COUNT_PER_RAD          446.9071
#define RMP_COUNT_PER_RAD_PER_S    446.9071
#define RMP_COUNT_PER_RAD_PER_SS   446.9071
// This is different from rotation and depends on geometry
#define RMP_COUNT_PER_REV_RMP50    117031.0
#define RMP_COUNT_PER_REV_RMP100   117031.0
#define RMP_COUNT_PER_REV_RMP200   112644.0
#define RMP_COUNT_PER_REV_RMP400   112644.0

// main (CU) battery voltage
#define RMP_BASE_COUNT_PER_VOLT      4.0

// user interface battery voltage: volts=offset + n*counts
#define RMP_UI_OFFSET               1.4
#define RMP_UI_COEFF                0.0125
// user interface battery status: low battery warning
#define RMP_UI_LOW_WARNING          0x80
// user interface battery status: empty battery shutdown
#define RMP_UI_EMPTY_SHUTDOWN       0x40

// motor torque
#define RMP_COUNT_PER_NM_RMP50     1463.0
#define RMP_COUNT_PER_NM_RMP100    1463.0
#define RMP_COUNT_PER_NM_RMP200    1094.0
#define RMP_COUNT_PER_NM_RMP400    1094.0
#define RMP_SEC_PER_FRAME          0.01

// alexm: where is this from?
#define RMP_MAX_TRANS_VEL_MM_S      3576
// from rmi_demo: 1300*0.013805056
#define RMP_MAX_ROT_VEL_DEG_S       18

// limits

// from page 7 of the manual  
#define RMP_MAX_TRANS_VEL_COUNT     1176
#define RMP_MAX_ROT_VEL_COUNT       1024

// minimum powerbase (CU) battery voltage (V)
#define RMP_BASE_MIN_VOLTAGE          66.0
// minimum user interface (UI) battery voltage (V)
#define RMP_UI_MIN_VOLTAGE          6.0

// maximum time without incoming commands [s]
#define RMP_COMMAND_TIMEOUT         0.4

// misc
#define RMP_GEOM_WHEEL_SEP          0.54

#define RMP_CAN_DROPPED_PACKET      0x5A5A

enum RmpModel {
    RmpModel_50,
    RmpModel_100,
    RmpModel_200,
    RmpModel_400
};

enum OperationalMode {
    OperationalModeDisabled=0,
    OperationalModeTractor=1,
    OperationalModeBalance=2
};

enum GainSchedule {
    GainScheduleNormal=0,
    GainScheduleTall=1,
    GainScheduleHeavy=2
};

enum BalanceLockout
{
    BalanceAllowed=0,
    BalanceNotAllowed=1
};

enum ConfigurationCommand {
    ConfigutationCommandNone=0,
    ConfigurationCommandSetMaxVelocityScale=10,
    ConfigurationCommandSetMaxAccelerationScale=11,
    ConfigurationCommandSetMaxTurnrateScale=12,
    ConfigurationCommandSetGainSchedule=13,
    ConfigurationCommandSetCurrentLimitScale=14,
    ConfigurationCommandSetBalanceModeLockout=15,
    ConfigurationCommandSetOperationalMode=16,
    ConfigurationCommandResetIntegrators=50
};
std::string toString(ConfigurationCommand c);

#endif
