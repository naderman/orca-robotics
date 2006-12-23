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

#include "pathplanner2dI.h"

using namespace std;
using namespace orca;
using namespace pathplanner;

PathPlanner2dI::PathPlanner2dI( 
        orcaice::Proxy<orca::PathPlanner2dTask> &pathPlannerTaskProxy,
        orcaice::Proxy<orca::PathPlanner2dData> &pathPlannerDataProxy,
        const orcaice::Context & context )
            : pathPlannerTaskProxy_(pathPlannerTaskProxy),
              pathPlannerDataProxy_(pathPlannerDataProxy),
              context_(context)
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<PathPlanner2dConsumerPrx>
        ( context_, consumerPrx_, "PathPlanner2d" );    
}

Ice::Int
PathPlanner2dI::setTask(const ::orca::PathPlanner2dTask& task, const ::Ice::Current&)
{

    cout << "TRACE(pathplanner2dI.cpp): Just received a new task: " << endl << orcaice::toVerboseString(task); 

    pathPlannerTaskProxy_.set( task );

    return 0;
    
//     //TODO: This can be implemented differently: the return value should be the number of tasks in a queue.
//     // For now, we only allow one task (proxy instead of buffer) and overwriting the current task is not allowed.
//     if ( pathPlannerTaskProxy_.isEmpty() )
//     {
//         pathPlannerTaskProxy_.set( task );
//         return 0;
//     }
//     else
//     {
//         return 1;
//     }
}

orca::PathPlanner2dData
PathPlanner2dI::getData(const Ice::Current& current ) const 
{
    std::cout << "getData()" << std::endl;

    // we don't need to pop the data here because we don't block on it.
    if ( pathPlannerDataProxy_.isEmpty() )
    {
        throw orca::DataNotExistException( "try again later." );
    }

    // create a null pointer. data will be cloned into it.
    PathPlanner2dData data;
    pathPlannerDataProxy_.get( data );

    return data;
}

void 
PathPlanner2dI::localSetData( const PathPlanner2dData& data )
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
