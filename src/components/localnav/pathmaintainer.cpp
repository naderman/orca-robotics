#include <iostream>
#include <orcaice/orcaice.h>

#include "pathmaintainer.h"

using namespace std;
using namespace orcaice;

namespace localnav {

PathMaintainer::PathMaintainer( orcaice::PtrProxy<orca::PathFollower2dDataPtr> &pathPipe,
                                orcaice::Proxy<bool>                           &newPathArrivedPipe,
                                orcaice::Proxy<orca::Time>                     &activationPipe,
                                orcaice::Proxy<int>                            &wpIndexPipe,
                                orcaice::Context                                context)
    : path_(new orca::PathFollower2dData),
      wpIndex_(-1),
      wpIndexChanged_(false),
      pathPipe_(pathPipe),
      newPathArrivedPipe_(newPathArrivedPipe),
      activationPipe_(activationPipe),
      wpIndexPipe_(wpIndexPipe),
      context_(context)
{
}

const orca::Waypoint2d &
PathMaintainer::currentWaypoint() const
{
    assert( wpIndex_ >= 0 && wpIndex_ <= (int) (path_->path.size()) );
    cout<<"TRACE(pathmaintainer.cpp): currentWaypoint: " << orcaice::toString(path_->path[wpIndex_]) << endl;
    return path_->path[wpIndex_];
}

void
PathMaintainer::checkForNewPath( orca::PathFollower2dConsumerPrx &pathConsumer )
{
    bool dummy;
    if ( newPathArrivedPipe_.isNewData() )
    {
        context_.tracer()->debug( "PathMaintainer: received new path", 1 );
        newPathArrivedPipe_.get(dummy);
        pathPipe_.get( path_ );
        informWorldOfNewPath( pathConsumer, path_ );

        // Have we also been told to start?
        if ( activationPipe_.isNewData() )
        {
            context_.tracer()->debug( "Activating immediately" );
            activationPipe_.get(pathStartTime_);
            wpIndex_ = 0;
        }
        else
        {
            context_.tracer()->debug( "Not activating yet..." );
            wpIndex_ = -1;
        }
        wpIndexChanged_ = true;
    }
    else
    {
        // No new path received.
        if ( activationPipe_.isNewData() )
        {
            activationPipe_.get(pathStartTime_);
            context_.tracer()->warning( "What the hell??  Received activation when no new path was loaded.  Ignoring it." );
        }
    }
}

void 
PathMaintainer::checkForWpIndexChange( orca::PathFollower2dConsumerPrx &pathConsumer )
{
    if ( wpIndexChanged_ )
    {
        wpIndexPipe_.set( true );
    }
    informWorldOfNewWpIndex( pathConsumer, wpIndex_ );
}

void  
PathMaintainer::incrementWpIndex()
{
    wpIndexChanged_ = true;
    wpIndex_++;
    if ( wpIndex_ >= (int) path_->path.size() )
    {
        wpIndex_ = -1;
    }
}

void
PathMaintainer::informWorldOfNewPath( orca::PathFollower2dConsumerPrx pathConsumer, orca::PathFollower2dDataPtr &path )
{
    pathConsumer->setData( path );
}

void
PathMaintainer::informWorldOfNewWpIndex( orca::PathFollower2dConsumerPrx pathConsumer, int newIndex )
{
    pathConsumer->setWaypointIndex( newIndex );
}

double
PathMaintainer::secToNextWp() const
{
    return orcaice::timeAsDouble(currentWaypoint().timeTarget) - secSinceActivation();
}

double
PathMaintainer::secSinceActivation() const
{
    cout<<"TRACE(pathmaintainer.cpp): now:   " << orcaice::toString(orcaice::getNow()) << endl;
    cout<<"TRACE(pathmaintainer.cpp): start: " << orcaice::toString(pathStartTime_) << endl;

    double diff = orcaice::timeDiffAsDouble( orcaice::getNow(), pathStartTime_ );

    cout<<"TRACE(pathmaintainer.cpp): diff:  " << diff << endl;

    return diff;
}

}
