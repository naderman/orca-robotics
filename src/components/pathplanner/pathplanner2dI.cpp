/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <iostream>

#include <orcaice/orcaice.h>

#include "pathplanner2dI.h"

using namespace std;
using namespace orca;
using namespace pathplanner;

PathPlanner2dI::PathPlanner2dI( 
        orcaice::PtrProxy<orca::PathPlanner2dTaskPtr> &pathPlannerTaskProxy,
        orcaice::PtrProxy<orca::PathPlanner2dDataPtr> &pathPlannerDataProxy,
        orcaice::Context context)
            : pathPlannerTaskProxy_(pathPlannerTaskProxy),
              pathPlannerDataProxy_(pathPlannerDataProxy),
              context_(context)
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<PathPlanner2dConsumerPrx>
        ( context_, consumerPrx_, "PathPlanner2d" );    
}

Ice::Int
PathPlanner2dI::setTask(const ::orca::PathPlanner2dTaskPtr& taskPtr, const ::Ice::Current&)
{

    cout << "Just received a new task: " << endl << taskPtr; 

    pathPlannerTaskProxy_.set( taskPtr );

    return 0;
    
//     //TODO: This can be implemented differently: the return value should be the number of tasks in a queue.
//     // For now, we only allow one task (proxy instead of buffer) and overwriting the current task is not allowed.
//     if ( pathPlannerTaskProxy_.isEmpty() )
//     {
//         pathPlannerTaskProxy_.set( taskPtr );
//         return 0;
//     }
//     else
//     {
//         return 1;
//     }
}

orca::PathPlanner2dDataPtr
PathPlanner2dI::getData(const Ice::Current& current ) const 
{
    std::cout << "getData()" << std::endl;

    // we don't need to pop the data here because we don't block on it.
    if ( pathPlannerDataProxy_.isEmpty() )
    {
        throw orca::DataNotExistException( "try again later." );
    }

    // create a null pointer. data will be cloned into it.
    PathPlanner2dDataPtr data;
    pathPlannerDataProxy_.get( data );

    return data;
}

void 
PathPlanner2dI::localSetData( const PathPlanner2dDataPtr data )
{
    pathPlannerDataProxy_.set( data );

    // Try to push it out to IceStorm too
    try {
        consumerPrx_->setData( data );
    }
    catch ( Ice::ConnectionRefusedException &e )
    {
        // This could happen if IceStorm dies.
        // If we're running in an IceBox and the IceBox is shutting down, 
        // this is expected (our co-located IceStorm is obviously going down).
        context_.tracer()->warning( "Failed push to IceStorm." );
    }
}

// Subscribe people
void 
PathPlanner2dI::subscribe(const PathPlanner2dConsumerPrx &subscriber, const ::Ice::Current&)
{
//     cout<<"subscribe()"<<endl;
    IceStorm::QoS qos;
    topicPrx_->subscribe( qos, subscriber );
}

// Unsubscribe people
void 
PathPlanner2dI::unsubscribe(const PathPlanner2dConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout<<"unsubscribe()"<<endl;
    topicPrx_->unsubscribe( subscriber );
}



