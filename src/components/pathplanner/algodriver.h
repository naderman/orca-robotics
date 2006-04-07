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
 
#ifndef ORCA2_PATHPLANNER_ALGORITHM_DRIVER_H
#define ORCA2_PATHPLANNER_ALGORITHM_DRIVER_H

#include <orca/pathplanner2d.h>
#include <orca/ogmap.h>

#include <orcapathplan/pathplanutils.h>

#include "skeletongraphicsI.h"

namespace pathplanner
{

class AlgoDriver 
{

public:
    AlgoDriver( const orcapathplan::Config & config, SkeletonGraphicsI* skelGraphicsI, const bool useSkeleton=false )
        : config_(config),
          skelGraphicsI_(skelGraphicsI),
          useSkeleton_(useSkeleton) {};

    virtual ~AlgoDriver() {};
    
    // Computes the path
    virtual void computePath(    const orca::OgMapDataPtr          & ogMapDataPtr,
                                const orca::PathPlanner2dTaskPtr  & taskPtr,
                                const orca::PathPlanner2dDataPtr  & pathDataPtr ) = 0;

protected:
    orcapathplan::Config config_;
    SkeletonGraphicsI* skelGraphicsI_;
    bool useSkeleton_;

};

} // namespace

#endif
