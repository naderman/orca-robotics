#include "setpoint.h"
#include <iostream>
#include <cmath>

using namespace std;

namespace segwayrmp {

SetPoint::SetPoint( double maxForwardAcceleration, 
                    double maxReverseAcceleration )
    : dt_(0.0),
      maxForwardAcceleration_(fabs(maxForwardAcceleration)),
      maxReverseAcceleration_(fabs(maxReverseAcceleration)),
      setPoint_(0.0),
      currentCmdSpeed_(0.0)
{
}

void
SetPoint::evaluateDt()
{
    dt_ = timer_.elapsedSec();
    timer_.restart();
}

void
SetPoint::set( double speed )
{
    setPoint_ = speed;
}

double
SetPoint::currentCmdSpeed( bool &setPointReached )
{
    const double diff = setPoint_ - currentCmdSpeed_;
    const bool speedIncreasing = (diff > 0);

    double maxDeltaSpeed; // This is a positive quantity.
    if ( speedIncreasing )
    {
        maxDeltaSpeed = maxForwardAcceleration_ * dt_;
    }
    else
    {
        maxDeltaSpeed = maxReverseAcceleration_ * dt_;
    }

    setPointReached = ( fabs(diff) < maxDeltaSpeed );
    if ( setPointReached )
    {
        currentCmdSpeed_ = setPoint_;
    }
    else
    {
        if ( speedIncreasing )
            currentCmdSpeed_ += maxDeltaSpeed;
        else
            currentCmdSpeed_ -= maxDeltaSpeed;
    }
    return currentCmdSpeed_;
}


}

