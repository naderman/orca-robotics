#include "speedsetpoint.h"
#include <iostream>
#include <cmath>
#include <gbxutilacfr/exceptions.h>

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
      currentCmdSpeed_(0.0)
{
}

void
SpeedSetPoint::evaluateDt()
{
    dt_ = timer_.elapsedSec();
    if ( dt_ > 1.0 )
    {
        stringstream ss;
        ss << "Huh? Why is dt_ so large?? (dt_="<<dt_<<"s)";
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
    else if ( dt_ > 0.015 || dt_ < 0.001 )
    {
        cout<<"TRACE(speedsetpoint.cpp): "<< IceUtil::Time::now().toDateTime() <<": Outlier dt: " << dt_ << endl;
    }
    timer_.restart();
}

void
SpeedSetPoint::set( double speed )
{
    setPoint_ = speed;
}

double
SpeedSetPoint::currentCmdSpeed( bool &setPointAlreadyReached )
{
    // dt_ can actually be negative, e.g. if ntp steps the clock backwards...
    if ( dt_ <= 0.0 )
    {
        cout<<"TRACE(speedsetpoint.cpp): warning: found negative dt_: " << dt_ << ".  Setting dt_ to 0.01." << endl;
        dt_ = 0.01;
    }

    double cmdSpeedPrior = currentCmdSpeed_;

    const double diff = setPoint_ - currentCmdSpeed_;
    const bool speedIncreasing = (diff > 0);

    // cout<<"TRACE(speedsetpoint.cpp): setPoint_: " << setPoint_ << ", currentCmdSpeed_: " << currentCmdSpeed_ << ", diff: " << diff << endl;

    if ( fabs(diff) < 1e-9 )
    {
        setPointAlreadyReached = true;
        return currentCmdSpeed_;
    }

    double maxDeltaSpeed; // This is a positive quantity.
    if ( speedIncreasing )
    {
        maxDeltaSpeed = maxForwardAcceleration_ * dt_;
    }
    else
    {
        maxDeltaSpeed = maxReverseAcceleration_ * dt_;
    }

    bool willReachSetPoint = ( fabs(diff) < maxDeltaSpeed );
    if ( willReachSetPoint )
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
    assert( isBetween( currentCmdSpeed_, cmdSpeedPrior, setPoint_ ) );

    return currentCmdSpeed_;
}


}

