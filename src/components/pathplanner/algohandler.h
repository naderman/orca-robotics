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
 
#ifndef ORCA2_PATHPLANNER_ALGORITHM_HANDLER_H
#define ORCA2_PATHPLANNER_ALGORITHM_HANDLER_H

#include <orcaice/thread.h>
#include <orcaice/context.h>
#include <orcaice/ptrproxy.h>

#include <orca/ogmap.h>
#include <orca/pathplanner2d.h>
#include <orca/qgraphics2d.h>

namespace pathplanner
{

class AlgoDriver;
class SkeletonGraphicsI;
class PathPlanner2dI;

class AlgoHandler : public orcaice::Thread
{

public: 

    AlgoHandler( const orcaice::Context & context );
    ~AlgoHandler();

    virtual void run();

private:

    // generic inerface to the algorithm
    AlgoDriver* driver_;

    orca::OgMapPrx ogMapPrx_;
    orca::OgMapDataPtr ogMapDataPtr_;

//     Ice::ObjectPtr pathPlanner2dObj_;
    PathPlanner2dI* pathPlannerI_;
    SkeletonGraphicsI* graphicsI_;
    orcaice::PtrProxy<orca::PathPlanner2dTaskPtr>* pathPlannerTaskProxy_;
    orcaice::PtrProxy<orca::PathPlanner2dDataPtr>* pathPlannerDataProxy_;
//     orcaice::PtrProxy<orca::QGraphics2dPtr>* graphicsProxy_;
    
    void initNetwork();
    void initDriver();
    
    orcaice::Context context_;

};

} // namespace

#endif
