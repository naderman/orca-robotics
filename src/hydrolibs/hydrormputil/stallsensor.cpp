#include "stallsensor.h"
#include <iostream>
#include <cmath>

using namespace std;

namespace hydrormputil {

namespace {

    StallType currentStallCondition( const hydrointerfaces::SegwayRmp::Data &data,
                                     const StallSensor::Config &config )
    {
        if ( fabs(data.vx) > config.speedThreshold )
            return NoStall;

        const bool leftMotorTorqueHigh = fabs(data.leftTorque)   > config.torqueThreshold;
        const bool rightMotorTorqueHigh = fabs(data.rightTorque) > config.torqueThreshold;
        if ( leftMotorTorqueHigh && rightMotorTorqueHigh )
            return BothMotorsStall;
        else if ( leftMotorTorqueHigh )
            return LeftMotorStall;
        else if ( rightMotorTorqueHigh )
            return RightMotorStall;
        else
            return NoStall;
    }

}

StallType
StallSensor::isStalled( const hydrointerfaces::SegwayRmp::Data &data )
{
    StallType currentStall = currentStallCondition( data, config_ );
    if ( currentStall == NoStall )
    {
        timerIsRunning_ = false;
        return NoStall;
    }
    else
    {
        if ( !timerIsRunning_ )
        {
            timerIsRunning_ = true;
            stalledTimer_.restart();
            return NoStall;
        }
        else
        {
            if ( stalledTimer_.elapsedSec() > config_.timeThreshold )
            {
                return currentStall;
            }
        }
    }
    return NoStall;
}

std::string toString( StallType t )
{
    switch ( t )
    {
    case NoStall: return "NoStall";
    case BothMotorsStall: return "BothMotorsStall";
    case LeftMotorStall: return "LeftMotorStall";
    case RightMotorStall: return "RightMotorStall";
    default:
        stringstream ss;
        ss << "Unknown StallType: " << (int)t;
        return ss.str();
    }
}

}

