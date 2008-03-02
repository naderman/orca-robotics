#include "estop.h"
#include <iostream>

using namespace std;

namespace segwayrmp {

EStop::EStop( double                  keepAlivePeriodSec,
              const orcaice::Context &context )
    : isEStopTriggered_(true),
      keepAlivePeriodSec_(keepAlivePeriodSec),
      eStopImpl_( new orcaifaceimpl::EStopImpl( *this, "EStop", context ) ),
      context_(context)
{
}

bool
EStop::isEStopTriggered()
{
    IceUtil::Mutex::Lock lock(mutex_);
    if ( !isEStopTriggered_ && 
         keepAliveTimer_.elapsedSec() > keepAlivePeriodSec_ )
    {
        context_.tracer().info( "EStop: keep-alive timed out!" );
        isEStopTriggered_ = true;
    }
    return isEStopTriggered_;
}

void
EStop::activateEStop()
{
    IceUtil::Mutex::Lock lock(mutex_);
    isEStopTriggered_ = true;

    // Ideally we'd do something else here, rather than simply waiting
    // for the next call to isEStopTriggered().  
    //
    // But it's OK in this case, because the Segway times out if it
    // doesn't keep getting commands, and it can only get commands if
    // isEStopTriggered() is called.
}
void
EStop::keepAlive()
{
    IceUtil::Mutex::Lock lock(mutex_);
    if ( isEStopTriggered_ )
    {
        orca::EStopAlreadyTriggeredException e;
        e.what = "The platform is disabled";
        throw e;
    }
    keepAliveTimer_.restart();
}
double
EStop::getRequiredKeepAlivePeriodSec()
{
    IceUtil::Mutex::Lock lock(mutex_);
    return keepAlivePeriodSec_;
}
void
EStop::setToOperatingMode()
{
    IceUtil::Mutex::Lock lock(mutex_);
    if ( isEStopTriggered_ )
    {
        isEStopTriggered_ = false;
        keepAliveTimer_.restart();
    }
}

}

