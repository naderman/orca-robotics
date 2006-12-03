/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_PATH_CONVERT_H
#define ORCA2_PATH_CONVERT_H

#include <vector>

#include <orca/pathplanner2d.h>

#include <orcaogmap/orcaogmap.h>

#include "orcapathplan.h"


namespace orcapathplan {

    struct WaypointParameter
    {
        float      distanceTolerance;
        float      headingTolerance;
        orca::Time timeTarget;
        float      maxApproachSpeed;
        float      maxApproachTurnrate;
    };

    //! Converts the result code into the corresponding field of the Slice-defined data structure
    void convert( const orcapathplan::Result        result,
                  const orca::PathPlanner2dDataPtr &output );

    //! Converts a path (cell vector in cell coordinate system) and the result code
    //! into the Slice-defined PathPlanner2dDataPtr (world coordinate system). 
    //! Will append input cells to the output path if output path already contains 
    //! entries. Note that not all entries are set by this function (e.g. tolerances)
    void convert( const orcaogmap::OgMap               &ogMap,
                  const Cell2DVector                   &input,
                  const orcapathplan::Result            result,
                  const orca::PathPlanner2dDataPtr     &output );
    
    //! As above, but also sets the remaining waypoint parameters
    void convert( const orcaogmap::OgMap               &ogMap,
                  const Cell2DVector                   &input,
                  const std::vector<WaypointParameter> &wpPara,
                  const orcapathplan::Result            result,
                  const orca::PathPlanner2dDataPtr     &output );

}
#endif
