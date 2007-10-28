#include "masterfilebreadcrumbs.h"
#include <iostream>
#include "utils.h"

using namespace std;

namespace orcalog {

MasterFileBreadCrumbs::MasterFileBreadCrumbs()
{
}

bool
MasterFileBreadCrumbs::haveCrumbAtTime( const IceUtil::Time &t )
{
    std::map<IceUtil::Time,std::ios::pos_type>::const_iterator it =
        crumbs_.find( t );
    return !(it == crumbs_.end());
}

void
MasterFileBreadCrumbs::placeCrumb( const std::ios::pos_type &pos, int sec, int usec )
{
    IceUtil::Time time = iceUtilTime(sec,usec);

    if ( crumbs_.size() == 0 )
    {
        // First crumb -- add it and make this the latest time
        latestTime_ = time;
        crumbs_[time] = pos;
    }
    else
    {
        // Already have at least one crumb.
        // Add this one if it's new.
        if ( time > latestTime_    ||
             !haveCrumbAtTime(time) )
        {
            crumbs_[time] = pos;
            if ( time > latestTime_ )
                latestTime_ = time;
        }
    }
}

SeekResult
MasterFileBreadCrumbs::getCrumbAtOrAfterTime( int querySec, int queryUsec, std::ios::pos_type &pos )
{
    std::map<IceUtil::Time,std::ios::pos_type>::const_iterator it =
        crumbs_.lower_bound( iceUtilTime(querySec,queryUsec) );
    if ( it == crumbs_.end() )
    {
        return SeekQueryInFuture;
    }
    else
    {
        pos = it->second;
        return SeekOK;
    }
}

}

