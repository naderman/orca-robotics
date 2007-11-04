#include "rxdata.h"
#include <iostream>

using namespace std;

namespace segwayrmp {

RxData::RxData( RmpModel rmpModel,
                const orcaice::Context &context )
    : rawData_(context),
      converter_(rmpModel)
{
}

std::vector<StatusFlag> 
RxData::statusFlags() const
{
    std::vector<StatusFlag> f;

    int16_t w1 = rawData_.status_word1;
    int16_t w2 = rawData_.status_word2;

    if ( w1 & WORD1_SAFETY_SHUTDOWN_1 )       f.push_back( StatusFlagSafetyShutdownType1 );
    if ( w1 & WORD1_SAFETY_SHUTDOWN_2 )       f.push_back( StatusFlagSafetyShutdownType2 );
    if ( w1 & WORD1_DISABLE_RESPONSE )        f.push_back( StatusFlagDisableResponse );
    if ( w1 & WORD1_SPEED_LIMIT_4MPH )        f.push_back( StatusFlagSpeedLimitTo4Mph );
    if ( w1 & WORD1_SPEED_LIMIT_0MPH )        f.push_back( StatusFlagSpeedLimitTo0Mph );
    if ( w1 & WORD1_SYSTEM_INTEGRITY_FAULT )  f.push_back( StatusFlagSystemIntegrityFault );
    if ( w1 & WORD1_MOTORS_ENABLED )          f.push_back( StatusFlagMotorsEnabled );
    if ( w1 & WORD1_BALANCE_MODE )            f.push_back( StatusFlagBalanceMode );
    if ( w1 & WORD1_YAW_TRANSIENT_FLAG )      f.push_back( StatusFlagYawTransient );
    if ( w1 & WORD1_EMPTY_BATTERY_HAZARD )    f.push_back( StatusFlagEmptyBatteryHazard );
    if ( w1 & WORD1_WHEEL_SLIP_FLAG )         f.push_back( StatusFlagWheelSlip );
    if ( w1 & WORD1_MOTOR_FEEDBACK_FAULT )    f.push_back( StatusFlagMotorFeedbackFault );
    if ( w1 & WORD1_LOW_BATTRY_HAZARD )       f.push_back( StatusFlagLowBatteryHazard );

    if ( w2 & WORD2_FOREAFT_PITCH_ANGLE_LIMIT_A )  f.push_back( StatusFlagForeAftPitchAngleLimitExceededA );
    if ( w2 & WORD2_ESTOP_SWITCH_OPEN_A )          f.push_back( StatusFlagEStopSwitchOpenA );
    if ( w2 & WORD2_POSITION_ERROR_LIMIT_A )       f.push_back( StatusFlagPositionErrorLimitExceededA );
    if ( w2 & WORD2_MAX_VELOCITY_A )               f.push_back( StatusFlagMaxVelocityExceededA );
    if ( w2 & WORD2_BATTERY_EMPTY_A )              f.push_back( StatusFlagBatteryEmptyA );
    if ( w2 & WORD2_BATTERY_TEMP_LIMIT_A )         f.push_back( StatusFlagBatteryTemperatureOutOfRangeA );
    if ( w2 & WORD2_ROLL_ANGLE_LIMIT_A )           f.push_back( StatusFlagRollAngleLimitExceededA );
    if ( w2 & WORD2_LOW_BATTERY_BUS_VOLTAGE_A )    f.push_back( StatusFlagLowBatteryBusVoltageA );
    if ( w2 & WORD2_FOREAFT_PITCH_ANGLE_LIMIT_B )  f.push_back( StatusFlagForeAftPitchAngleLimitExceededB );
    if ( w2 & WORD2_ESTOP_SWITCH_OPEN_B )          f.push_back( StatusFlagEStopSwitchOpenB );
    if ( w2 & WORD2_POSITION_ERROR_LIMIT_B )       f.push_back( StatusFlagPositionErrorLimitExceededB );
    if ( w2 & WORD2_MAX_VELOCITY_B )               f.push_back( StatusFlagMaxVelocityExceededB );
    if ( w2 & WORD2_BATTERY_EMPTY_B )              f.push_back( StatusFlagBatteryEmptyB );
    if ( w2 & WORD2_BATTERY_TEMP_LIMIT_B )         f.push_back( StatusFlagBatteryTemperatureOutOfRangeB );
    if ( w2 & WORD2_ROLL_ANGLE_LIMIT_B )           f.push_back( StatusFlagRollAngleLimitExceededB );
    if ( w2 & WORD2_LOW_BATTERY_BUS_VOLTAGE_B )    f.push_back( StatusFlagLowBatteryBusVoltageB );

    return f;
}

bool
RxData::isWarn() const
{
    int16_t w1 = rawData_.status_word1;
    int16_t w2 = rawData_.status_word2;

    return ( w1 & WORD1_DISABLE_RESPONSE          || // Not sure what this one is...
             w1 & WORD1_YAW_TRANSIENT_FLAG        || // Not sure what this one is...
             w1 & WORD1_WHEEL_SLIP_FLAG           ||
             w1 & WORD1_MOTOR_FEEDBACK_FAULT      ||
             w1 & WORD1_LOW_BATTRY_HAZARD         ||
             w2 & WORD2_MAX_VELOCITY_A            || // Not sure if this is significant or not...
             w2 & WORD2_LOW_BATTERY_BUS_VOLTAGE_A ||
             w2 & WORD2_MAX_VELOCITY_B            || // Not sure if this is significant or not...
             w2 & WORD2_LOW_BATTERY_BUS_VOLTAGE_B );
}

bool 
RxData::isFault() const
{
    int16_t w1 = rawData_.status_word1;
    int16_t w2 = rawData_.status_word2;

    return ( w1 & WORD1_SAFETY_SHUTDOWN_1           ||
             w1 & WORD1_SAFETY_SHUTDOWN_2           ||
             w1 & WORD1_SYSTEM_INTEGRITY_FAULT      ||
             w1 & WORD1_EMPTY_BATTERY_HAZARD        ||
             w2 & WORD2_FOREAFT_PITCH_ANGLE_LIMIT_A ||
             w2 & WORD2_ESTOP_SWITCH_OPEN_A         ||
             w2 & WORD2_POSITION_ERROR_LIMIT_A      ||
             w2 & WORD2_BATTERY_EMPTY_A             ||
             w2 & WORD2_BATTERY_TEMP_LIMIT_A        ||
             w2 & WORD2_ROLL_ANGLE_LIMIT_A          ||
             w2 & WORD2_FOREAFT_PITCH_ANGLE_LIMIT_B ||
             w2 & WORD2_ESTOP_SWITCH_OPEN_B         ||
             w2 & WORD2_POSITION_ERROR_LIMIT_B      ||
             w2 & WORD2_BATTERY_EMPTY_B             ||
             w2 & WORD2_BATTERY_TEMP_LIMIT_B        ||
             w2 & WORD2_ROLL_ANGLE_LIMIT_B          );
}

}

