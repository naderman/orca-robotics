/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include <orcaice/orcaice.h>
#include "pathfollower2dI.h"

#include "pathmaintainer.h"

using namespace std;
using namespace orcaice;

namespace localnav {

PathMaintainer::PathMaintainer( PathFollower2dI            &pathFollowerInterface,
                                const orcaice::Context     &context)
    : wpIndex_(-1),
      wpIndexChanged_(false),
      pathFollowerInterface_(pathFollowerInterface),
      context_(context)
{
}

const orca::Waypoint2d &
PathMaintainer::currentWaypoint() const
{
    assert( wpIndex_ >= 0 && wpIndex_ <= (int) (path_.path.size()) );
    return path_.path[wpIndex_];
}

void
PathMaintainer::checkPathOut( const orca::PathFollower2dData& pathData )
{
    std::stringstream ss;
    bool normal=true;
    const float epsLinear     = 1e-3;
    const float epsRotational = 1.0*M_PI/180.0;
    for ( unsigned int i=0; i < pathData.path.size(); i++ )
    {
        const orca::Waypoint2d &wp = pathData.path[i];

        if ( wp.distanceTolerance < epsLinear )
        {
            ss << "Waypoint " << i << ": possibly sketchy distance tolerance: " 
               << wp.distanceTolerance << "m" << endl;
            normal = false;
        }
        if ( wp.headingTolerance < epsRotational )
        {
            ss << "Waypoint " << i << ": possibly sketchy heading tolerance: " 
               << wp.headingTolerance*180.0/M_PI << "deg" << endl;
            normal = false;
        }
        if ( wp.maxApproachSpeed < epsLinear )
        {
            ss << "Waypoint " << i << ": possibly sketchy maxApproachSpeed: " 
               << wp.maxApproachSpeed << "m/s" << endl;
            normal = false;
        }
        if ( wp.maxApproachTurnrate < epsRotational )
        {
            ss << "Waypoint " << i << ": possibly sketchy maxApproachTurnrate: " 
               << wp.maxApproachTurnrate*180.0/M_PI << "deg/s" << endl;
            normal = false;
        }
    }
    if ( !normal )
    {
        std::string warnString = "In newly-received path: \n";
        warnString += ss.str();
        context_.tracer()->warning( warnString );
    }
}

void
PathMaintainer::checkForNewPath( orca::PathFollower2dConsumerPrx &pathConsumer )
{
    if ( pathFollowerInterface_.newPathArrivedProxy().isNewData() || 
         pathFollowerInterface_.activationProxy().isNewData() )
    {
        // Load the path if it's there
        if ( pathFollowerInterface_.newPathArrivedProxy().isNewData() )
        {
            // Clear the newPathArrivedProxy
            bool dummy;
            pathFollowerInterface_.newPathArrivedProxy().get(dummy);
            pathFollowerInterface_.pathProxy().get( path_ );
            pathFollowerInterface_.localSetData( path_ ); //informs consumers

            // Issue warnings if the path is screwy
            checkPathOut( path_ );
        }

        // Have we been told to start?
        if ( pathFollowerInterface_.activationProxy().isNewData() )
        {
            context_.tracer()->debug( "PathMaintainer: activating.", 1 );
            pathFollowerInterface_.activationProxy().get(pathStartTime_);
            wpIndex_ = 0;
            if ( path_.path.size() == 0 )
            {
                context_.tracer()->debug( "Path was empty.  Stopping.", 1 );
                wpIndex_ = -1;
            }
        }
        else
        {
            context_.tracer()->debug( "PathMaintainer: received new path, not activating yet...", 1 );
            wpIndex_ = -1;
        }
        wpIndexChanged_ = true;

        std::stringstream ss;
        ss << "PathMaintainer: new path: " << orcaice::toVerboseString( path_ );
        context_.tracer()->debug( ss.str(), 2 );
    }
}

void 
PathMaintainer::checkForWpIndexChange( orca::PathFollower2dConsumerPrx &pathConsumer )
{
    if ( wpIndexChanged_ )
    {
        pathFollowerInterface_.localSetWaypointIndex( wpIndex_ );
    }
}

void  
PathMaintainer::incrementWpIndex()
{
    wpIndexChanged_ = true;
    wpIndex_++;
    if ( wpIndex_ >= (int) path_.path.size() )
    {
        wpIndex_ = -1;
    }
}

double
PathMaintainer::secToNextWp() const
{
    return orcaice::timeAsDouble(currentWaypoint().timeTarget) - secSinceActivation();
}

double
PathMaintainer::secSinceActivation() const
{
    // cout<<"TRACE(pathmaintainer.cpp): now:   " << orcaice::toString(orcaice::getNow()) << endl;
    // cout<<"TRACE(pathmaintainer.cpp): start: " << orcaice::toString(pathStartTime_) << endl;

    double diff = orcaice::timeDiffAsDouble( orcaice::getNow(), pathStartTime_ );

    // cout<<"TRACE(pathmaintainer.cpp): diff:  " << diff << endl;

    return diff;
}

}
