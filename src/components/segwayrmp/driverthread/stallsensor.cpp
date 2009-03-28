#include "stallsensor.h"
#include <iostream>
#include <cmath>

using namespace std;

namespace segwayrmpdriverthread {

namespace {

    StallSensor::StallType currentStallCondition( const hydrointerfaces::SegwayRmp::Data &data,
                                                  const StallSensor::Config &config )
    {
        if ( fabs(data.vx) > config.speedThreshold )
            return StallSensor::NoStall;

        const bool leftMotorTorqueHigh = fabs(data.leftTorque)   > config.torqueThreshold;
        const bool rightMotorTorqueHigh = fabs(data.rightTorque) > config.torqueThreshold;
        if ( leftMotorTorqueHigh && rightMotorTorqueHigh )
            return StallSensor::BothMotorsStall;
        else if ( leftMotorTorqueHigh )
            return StallSensor::LeftMotorStall;
        else if ( rightMotorTorqueHigh )
            return StallSensor::RightMotorStall;
        else
            return StallSensor::NoStall;
    }

}

StallSensor::StallType
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
}

std::string toString( StallSensor::StallType t )
{
    switch ( t )
    {
    case StallSensor::NoStall: return "NoStall";
    case StallSensor::BothMotorsStall: return "BothMotorsStall";
    case StallSensor::LeftMotorStall: return "LeftMotorStall";
    case StallSensor::RightMotorStall: return "RightMotorStall";
    default:
        stringstream ss;
        ss << "Unknown StallType: " << (int)t;
        return ss.str();
    }
}

}

