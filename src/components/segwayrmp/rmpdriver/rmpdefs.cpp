#include "rmpdefs.h"
#include <iostream>
#include <sstream>

using namespace std;

namespace segwayrmp {

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

}
