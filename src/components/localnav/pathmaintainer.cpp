/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include <hydronavutil/hydronavutil.h>
#include "pathfollowerinterface.h"
#include "pathmaintainer.h"

using namespace std;
using namespace orcaice;

namespace localnav {

namespace {

    bool
    isPathSketchy( const orca::PathFollower2dData& pathData, std::string &sketchyReason )
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
            sketchyReason = ss.str();
            return true;
        }
        else
            return false;
    }

    // Convert from global to local coords
    void
    convert( const orca::Waypoint2d   &wp,
             orcalocalnav::Goal       &goal,
             const hydronavutil::Pose &pose,
             double                    secSinceActivation )
    {
        double secToWp = orcaice::timeAsDouble(wp.timeTarget) - secSinceActivation;
    
        goal.set( wp.target.p.x,
                  wp.target.p.y,
                  wp.target.o,
                  wp.distanceTolerance,
                  wp.headingTolerance,
                  secToWp,
                  wp.maxApproachSpeed,
                  wp.maxApproachTurnrate );

        // put the goal in robot's local coord system
        hydronavutil::subtractInitialOffset( goal.x,
                                             goal.y,
                                             goal.theta,
                                             pose.x(),
                                             pose.y(),
                                             pose.theta() );
        NORMALISE_ANGLE( goal.theta );
    }

}

////////////////////////////////////////////////////////////////////////////////

PathMaintainer::PathMaintainer( PathFollowerInterface   &pathFollowerInterface,
                                const Clock             &clock,
                                const orcaice::Context  &context)
    : wpIndex_(-1),
      wpIndexChanged_(false),
      pathFollowerInterface_(pathFollowerInterface),
      clock_(clock),
      context_(context)
{
}

void
PathMaintainer::checkForNewPath()
{
    bool gotNewPath=false, gotActivation=false;
    pathFollowerInterface_.serviceRequests( gotNewPath,
                                            path_,
                                            gotActivation,
                                            pathStartTime_ );

    const bool servicedRequest = (gotNewPath || gotActivation);

    if ( servicedRequest )
    {
        if ( gotNewPath )
        {
            // See if there's anything weird about it
            std::string reason;
            if ( isPathSketchy( path_, reason ) )
            {
                string warnString = "In newly-received path: \n"+reason;
                context_.tracer().warning( warnString );                
            }
        }
        if ( gotActivation )
        {
            wpIndex_ = 0;
            if ( path_.path.size() == 0 )
            {
                context_.tracer().debug( "Path was empty.  Stopping.", 1 );
                wpIndex_ = -1;
            }
        }
        else
        {
            context_.tracer().debug( "PathMaintainer: received new path, not activating yet...", 1 );
            wpIndex_ = -1;
        }
        wpIndexChanged_ = true;

        std::stringstream ss;
        ss << "PathMaintainer: new path: " << orcaobj::toVerboseString( path_ );
        context_.tracer().debug( ss.str(), 2 );
    }


//     if ( pathFollowerInterface_.newPathArrivedStore().isNewData() || 
//          pathFollowerInterface_.activationArrivedStore().isNewData() )
//     {
//         // Load the path if it's there
//         if ( pathFollowerInterface_.newPathArrivedStore().isNewData() )
//         {
//             // Clear the newPathArrivedProxy
//             bool dummy;
//             pathFollowerInterface_.newPathArrivedStore().get(dummy);
//             pathFollowerInterface_.pathStore().get( path_ );
//             pathFollowerInterface_.localSetData( path_ ); //informs consumers

//             // Issue warnings if the path is screwy
//             checkPathOut( path_ );
//         }

//         // Have we been told to start?
//         if ( pathFollowerInterface_.activationArrivedStore().isNewData() )
//         {
//             context_.tracer().debug( "PathMaintainer: activating.", 1 );
//             bool dummy;
//             pathFollowerInterface_.activationArrivedStore().get(dummy);
//             pathFollowerInterface_.activationTimeStore().get(pathStartTime_);
//             wpIndex_ = 0;
//             if ( path_.path.size() == 0 )
//             {
//                 context_.tracer().debug( "Path was empty.  Stopping.", 1 );
//                 wpIndex_ = -1;
//             }
//         }
//         else
//         {
//             context_.tracer().debug( "PathMaintainer: received new path, not activating yet...", 1 );
//             wpIndex_ = -1;
//         }
//         wpIndexChanged_ = true;

//         std::stringstream ss;
//         ss << "PathMaintainer: new path: " << orcaobj::toVerboseString( path_ );
//         context_.tracer().debug( ss.str(), 2 );
//     }
}

void 
PathMaintainer::checkForWpIndexChange()
{
    if ( wpIndexChanged_ )
    {
        pathFollowerInterface_.updateWaypointIndex( wpIndex_ );
        wpIndexChanged_ = false;
    }
}

bool 
PathMaintainer::waypointReached( const orca::Waypoint2d &wp,
                                 const hydronavutil::Pose &pose,
                                 const double timeNow )
{
    double distanceToWp = hypot( pose.y()-wp.target.p.y,
                                 pose.x()-wp.target.p.x );
    if ( distanceToWp > wp.distanceTolerance )
        return false;

    double headingDiff = pose.theta()-wp.target.o;
    NORMALISE_ANGLE( headingDiff );
    if ( fabs(headingDiff) > wp.headingTolerance )
        return false;

    double timeTarget = wp.timeTarget.seconds + wp.timeTarget.useconds*1e-6;
    if ( timeNow < timeTarget )
    {
        stringstream ss;
        ss << "PathMaintainer: Physically at waypoint, but have to wait " << timeTarget-timeNow << " seconds.";
        context_.tracer().debug( ss.str(), 2 );
        return false;
    }

    return true;
}

bool
PathMaintainer::getActiveGoals( std::vector<orcalocalnav::Goal> &goals,
                                int                              maxNumWaypoints,
                                const hydronavutil::Pose        &pose )
{
    goals.resize(0);
    if ( wpIndex_ == -1 ) return false;

    // Time now relative to start time
    double timeNow = orcaice::timeDiffAsDouble( clock_.time(), pathStartTime_ );

    // Peel off waypoints if they're reached
    while ( waypointReached( path_.path[wpIndex_], pose, timeNow ) )
        incrementWpIndex();

    int wpI=0;
    for ( unsigned int pI=wpIndex_;
          pI < path_.path.size() && wpI < maxNumWaypoints;
          pI++, wpI++ )
    {
        orcalocalnav::Goal goal;
        convert( path_.path[pI], goal, pose, secSinceActivation() );
        goals.push_back( goal );
    }
    
    bool haveGoal = goals.size() > 0;
    return haveGoal;
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
PathMaintainer::secSinceActivation() const
{
    // cout<<"TRACE(pathmaintainer.cpp): now:   " << orcaice::toString(timeNow_) << endl;
    // cout<<"TRACE(pathmaintainer.cpp): start: " << orcaice::toString(pathStartTime_) << endl;

    double diff = orcaice::timeDiffAsDouble( clock_.time(), pathStartTime_ );

    // cout<<"TRACE(pathmaintainer.cpp): diff:  " << diff << endl;

    return diff;
}

}