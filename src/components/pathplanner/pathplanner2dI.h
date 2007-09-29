/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PATHPLANNER_I_H
#define ORCA2_PATHPLANNER_I_H

#include <IceStorm/IceStorm.h>

#include <orca/pathplanner2d.h>
#include <orcaiceutil/proxy.h>

namespace pathplanner
{

class PathPlanner2dI : public orca::PathPlanner2d
{
public:
    PathPlanner2dI( 
        orcaiceutil::Proxy<orca::PathPlanner2dTask> &pathPlannerTaskProxy,
        orcaiceutil::Proxy<orca::PathPlanner2dData> &pathPlannerDataProxy,
        const orcaice::Context & context
    );

    // remote calls
    virtual ::Ice::Int setTask(const ::orca::PathPlanner2dTask&, const ::Ice::Current& = ::Ice::Current());

    virtual void subscribe(const ::orca::PathPlanner2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());
    virtual void unsubscribe(const ::orca::PathPlanner2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual ::orca::PathPlanner2dData getData(const ::Ice::Current& ) const;

    // local calls
    void localSetData( const ::orca::PathPlanner2dData& data );

private:

    orcaiceutil::Proxy<orca::PathPlanner2dTask>& pathPlannerTaskProxy_;

    // the driver puts the latest computed path into here
    orcaiceutil::Proxy<orca::PathPlanner2dData>& pathPlannerDataProxy_;

    // The topic to which we'll publish
    IceStorm::TopicPrx topicPrx_;
    
    // The interface to which we'll publish
    orca::PathPlanner2dConsumerPrx  consumerPrx_;

    orcaice::Context context_;

};

} // namespace

#endif
