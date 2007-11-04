#include "rmpdefs.h"
#include <iostream>
#include <sstream>

using namespace std;

namespace segwayrmp {

std::string toString( RmpModel model )
{
    switch ( model )
    {
    case RmpModel_50:
        return "RmpModel_50";
    case RmpModel_100:
        return "RmpModel_100";
    case RmpModel_200:
        return "RmpModel_200";
    case RmpModel_400:
        return "RmpModel_400";
    default:
        stringstream ss; ss << " ?? Unknown RmpModel: " << model << " ??";
        return ss.str();
    }
}

std::string toString(ConfigurationCommand c)
{
    switch (c)
    {
    case ConfigutationCommandNone:
        return "ConfigutationCommandNone";
    case ConfigurationCommandSetMaxVelocityScale:
        return "ConfigurationCommandSetMaxVelocityScale";
    case ConfigurationCommandSetMaxAccelerationScale:
        return "ConfigurationCommandSetMaxAccelerationScale";
    case ConfigurationCommandSetMaxTurnrateScale:
        return "ConfigurationCommandSetMaxTurnrateScale";
    case ConfigurationCommandSetGainSchedule:
        return "ConfigurationCommandSetGainSchedule";
    case ConfigurationCommandSetCurrentLimitScale:
        return "ConfigurationCommandSetCurrentLimitScale";
    case ConfigurationCommandSetBalanceModeLockout:
        return "ConfigurationCommandSetBalanceModeLockout";
    case ConfigurationCommandSetOperationalMode:
        return "ConfigurationCommandSetOperationalMode";
    case ConfigurationCommandResetIntegrators:
        return "ConfigurationCommandResetIntegrators";
    default:
        stringstream ss; ss << " ?? Unknown ConfigurationCommand: " << c << " ??";
        return ss.str();
    }
}

std::string toString(StatusFlag s)
{
    switch (s )
    {
    case StatusFlagSafetyShutdownType1:
        return "StatusFlagSafetyShutdownType1";
    case StatusFlagSafetyShutdownType2:
        return "StatusFlagSafetyShutdownType2";
    case StatusFlagDisableResponse:
        return "StatusFlagDisableResponse";
    case StatusFlagSpeedLimitTo4Mph:
        return "StatusFlagSpeedLimitTo4Mph";
    case StatusFlagSpeedLimitTo0Mph:
        return "StatusFlagSpeedLimitTo0Mph";
    case StatusFlagSystemIntegrityFault:
        return "StatusFlagSystemIntegrityFault";
    case StatusFlagMotorsEnabled:
        return "StatusFlagMotorsEnabled";
    case StatusFlagBalanceMode:
        return "StatusFlagBalanceMode";
    case StatusFlagYawTransient:
        return "StatusFlagYawTransient";
    case StatusFlagEmptyBatteryHazard:
        return "StatusFlagEmptyBatteryHazard";
    case StatusFlagWheelSlip:
        return "StatusFlagWheelSlip";
    case StatusFlagMotorFeedbackFault:
        return "StatusFlagMotorFeedbackFault";
    case StatusFlagLowBatteryHazard:
        return "StatusFlagLowBatteryHazard";
    case StatusFlagForeAftPitchAngleLimitExceededA:
        return "StatusFlagForeAftPitchAngleLimitExceededA";
    case StatusFlagEStopSwitchOpenA:
        return "StatusFlagEStopSwitchOpenA";
    case StatusFlagPositionErrorLimitExceededA:
        return "StatusFlagPositionErrorLimitExceededA";
    case StatusFlagMaxVelocityExceededA:
        return "StatusFlagMaxVelocityExceededA";
    case StatusFlagBatteryEmptyA:
        return "StatusFlagBatteryEmptyA";
    case StatusFlagBatteryTemperatureOutOfRangeA:
        return "StatusFlagBatteryTemperatureOutOfRangeA";
    case StatusFlagRollAngleLimitExceededA:
        return "StatusFlagRollAngleLimitExceededA";
    case StatusFlagLowBatteryBusVoltageA:
        return "StatusFlagLowBatteryBusVoltageA";
    case StatusFlagForeAftPitchAngleLimitExceededB:
        return "StatusFlagForeAftPitchAngleLimitExceededB";
    case StatusFlagEStopSwitchOpenB:
        return "StatusFlagEStopSwitchOpenB";
    case StatusFlagPositionErrorLimitExceededB:
        return "StatusFlagPositionErrorLimitExceededB";
    case StatusFlagMaxVelocityExceededB:
        return "StatusFlagMaxVelocityExceededB";
    case StatusFlagBatteryEmptyB:
        return "StatusFlagBatteryEmptyB";
    case StatusFlagBatteryTemperatureOutOfRangeB:
        return "StatusFlagBatteryTemperatureOutOfRangeB";
    case StatusFlagRollAngleLimitExceededB:
        return "StatusFlagRollAngleLimitExceededB";
    case StatusFlagLowBatteryBusVoltageB:
        return "StatusFlagLowBatteryBusVoltageB";
    default:
        stringstream ss; ss << " ?? Unknown StatusFlag: " << s << " ??";
        return ss.str();
    }
}

}
