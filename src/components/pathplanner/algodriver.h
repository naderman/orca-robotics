/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ALGORITHM_DRIVER_H
#define ALGORITHM_DRIVER_H

#include <orca/pathplanner2d.h>
#include <orca/ogmap.h>

#include <hydropathplan/hydropathplan.h>

namespace pathplanner
{

//
// Base-class for path-planning drivers
//
class AlgoDriver 
{
public:
    virtual ~AlgoDriver() {};
    
    // Computes the path
    virtual void computePath( const orca::PathPlanner2dTask& task,
                              orca::PathPlanner2dData& path ) = 0;
};

} // namespace

#endif
