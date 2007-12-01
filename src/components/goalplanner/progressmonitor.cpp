#include "progressmonitor.h"
#include <iostream>
#include <orcaice/orcaice.h>

using namespace std;

namespace goalplanner {

ProgressMonitor::ProgressMonitor()
    : gotData_(false),
      isEnabled_(true),
      wpIndex_(-1)
{
}

ProgressMonitor::~ProgressMonitor()
{
}

void 
ProgressMonitor::setWaypointIndex( int index, const ::Ice::Current& )
{
    IceUtil::Mutex::Lock lock(mutex_);
    wpIndex_ = index;
}

void 
ProgressMonitor::setActivationTime( const orca::Time &absoluteTime,
                                    double relativeTime,
                                    const ::Ice::Current& )
{
    // IceUtil::Mutex::Lock lock(mutex_);
    // Nothing to do here.
}

void 
ProgressMonitor::setData( const orca::PathFollower2dData &data,
                          const ::Ice::Current& )
{
    IceUtil::Mutex::Lock lock(mutex_);
    pathData_ = data;
    gotData_ = true;
}

void 
ProgressMonitor::setEnabledState( bool enabledState,
                                  const ::Ice::Current& )
{
    IceUtil::Mutex::Lock lock(mutex_);
    isEnabled_ = enabledState;
}

bool
ProgressMonitor::followingPath() const
{
    if ( !gotData_ ) return false;
    if ( !isEnabled_ ) return false;
    if ( wpIndex_ == -1 ) return false;
    if ( wpIndex_ < 0 || wpIndex_ >= (int)(pathData_.path.size()) )
    {
        stringstream ss;
        ss << "Bad wpIndex_: " << wpIndex_ << " for path: " << orcaice::toString(pathData_);
        throw hydroutil::Exception( ERROR_INFO, ss.str() );
    }    

    return true;
}

bool
ProgressMonitor::getCurrentWp( orca::Waypoint2d &currentWp )
{
    IceUtil::Mutex::Lock lock(mutex_);
    if ( !followingPath() ) return false;

    currentWp = pathData_.path[wpIndex_];
    return true;
}

bool
ProgressMonitor::getCurrentPath( orca::PathFollower2dData &pathData, int &wpIndex )
{
    IceUtil::Mutex::Lock lock(mutex_);
    if ( !followingPath() ) return false;

    pathData = pathData_;
    wpIndex = wpIndex_;
    return true;
}

}

