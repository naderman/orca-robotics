#include "speedsetpoint.h"
#include <iostream>
#include <cmath>

using namespace std;

namespace segwayrmpdriverthread {

namespace {

    bool isBetween( double q, double a, double b )
    {
        double hi, lo;
        if ( a > b )
        {
            hi = a; lo = b;
        }
        else
        {
            hi = b; lo = a;
        }
        return ( hi >= q && q >= lo );
    }

}

//////////////////////////////////////////////////////////////////////

SpeedSetPoint::SpeedSetPoint( double maxForwardAcceleration, 
                              double maxReverseAcceleration )
    : dt_(0.0),
      maxForwardAcceleration_(fabs(maxForwardAcceleration)),
      maxReverseAcceleration_(fabs(maxReverseAcceleration)),
      setPoint_(0.0),
      prevSetPoint_(0.0),
      currentCmdSpeed_(0.0)
{
}

void
SpeedSetPoint::evaluateDt()
{
    dt_ = timer_.elapsedSec();
    if ( dt_ > 0.5 )
    {
        cout<<"TRACE(setpoint.cpp): Huh? Why is dt_ so large?? (dt_="<<dt_<<"s)" << endl;
    }
    else if ( dt_ < 0.001 )
    {
        cout<<"TRACE(setpoint.cpp): Huh? Why is dt_ so small?? (dt_="<<dt_<<"s)" << endl;
    }
    timer_.restart();
}

void
SpeedSetPoint::set( double speed )
{
    prevSetPoint_ = setPoint_;
    setPoint_ = speed;
}

double
SpeedSetPoint::currentCmdSpeed( bool &setPointReached )
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
        {
            currentCmdSpeed_ += maxDeltaSpeed;
            assert( currentCmdSpeed_ <= setPoint_ );
        }
        else
        {
            currentCmdSpeed_ -= maxDeltaSpeed;
            assert( currentCmdSpeed_ >= setPoint_ );
        }
    }

    // Hard-coded safety check...
    assert( fabs(currentCmdSpeed_) < 3.6 );
    // Another safety check...
    assert( isBetween( currentCmdSpeed_, setPoint_, prevSetPoint_ ) );

    return currentCmdSpeed_;
}


}

