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

#ifndef ORCA2_PATHPLANNER_I_H
#define ORCA2_PATHPLANNER_I_H

#include <IceStorm/IceStorm.h>

#include <orca/pathplanner2d.h>
#include <orcaice/ptrproxy.h>

namespace pathplanner
{

class PathPlanner2dI : public orca::PathPlanner2d
{
public:
    PathPlanner2dI( 
        orcaice::PtrProxy<orca::PathPlanner2dTaskPtr> &pathPlannerTaskProxy,
        orcaice::PtrProxy<orca::PathPlanner2dDataPtr> &pathPlannerDataProxy,
        orcaice::Context context
    );

    // remote calls
    virtual ::Ice::Int setTask(const ::orca::PathPlanner2dTaskPtr&, const ::Ice::Current& = ::Ice::Current());

    virtual void subscribe(const ::orca::PathPlanner2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());
    virtual void unsubscribe(const ::orca::PathPlanner2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual ::orca::PathPlanner2dDataPtr getData(const ::Ice::Current& ) const;

    // local calls
    void localSetData( const ::orca::PathPlanner2dDataPtr data );

private:

    orcaice::PtrProxy<orca::PathPlanner2dTaskPtr> &pathPlannerTaskProxy_;

    // the driver puts the latest computed path into here
    orcaice::PtrProxy<orca::PathPlanner2dDataPtr> &pathPlannerDataProxy_;

    // The topic to which we'll publish
    IceStorm::TopicPrx topicPrx_;
    // The interface to which we'll publish
    orca::PathPlanner2dConsumerPrx  consumerPrx_;

    orcaice::Context context_;

};

} // namespace

#endif
