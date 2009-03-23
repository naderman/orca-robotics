#include "estopmonitor.h"
#include <iostream>
#include <orcaice/orcaice.h>

using namespace std;

namespace segwayrmp {

EStopMonitor::EStopMonitor( const orca::EStopDescription &description )
    : description_(description)
{
    state_.timeStamp        = orcaice::getNow();
    state_.isEStopActivated = true;
    state_.info             = "e-stop un-initialised";
}

bool
EStopMonitor::isEStopTriggered( std::string &reason )
{
    IceUtil::Mutex::Lock lock(mutex_);

    if ( state_.isEStopActivated )
    {
        reason = state_.info;
        return true;
    }

    if ( keepAliveTimer_.elapsedSec() > description_.keepAlivePeriodSec )
    {
        stringstream reasonSS;
        reasonSS << "keep-alive timed out on "<<description_.info<<" (last heard from it "<<keepAliveTimer_.elapsedSec()<<"sec ago";
        reason = reasonSS.str();
        return true;
    }

    return false;
}

void
EStopMonitor::addData( const orca::EStopData &data )
{
    IceUtil::Mutex::Lock lock(mutex_);
    keepAliveTimer_.restart();
    state_ = data;
}

}

