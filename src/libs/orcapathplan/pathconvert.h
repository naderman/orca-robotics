/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCA_PATH_CONVERT_H
#define ORCA_PATH_CONVERT_H

#include <vector>
#include <string>
#include <orca/pathplanner2d.h>
#include <orcaogmap/orcaogmap.h>
#include <orcapathplan/orcapathplan.h>
#include <hydropathplan/hydropathplan.h>

namespace orcapathplan {

    struct WaypointParameter
    {
        float      distanceTolerance;
        float      headingTolerance;
        orca::Time timeTarget;
        float      maxApproachSpeed;
        float      maxApproachTurnrate;
    };
    std::string toString( const WaypointParameter &wp );

//     //! Transfers the result code into the corresponding field of the Slice-defined data structure
//     void convert( hydropathplan::Result     result,
//                   orca::PathPlanner2dData &output );

    //! Converts a path (cell vector in cell coordinate system)
    //! into the Slice-defined PathPlanner2dData (world coordinate system). 
    //! Heading values are set in the direction of two connected waypoints.
    //! The first waypoint's heading can be specified optionally (0.0 by default).
    //! Will append input cells to the output path if output path already contains entries. 
    //! Note that parameters are *not* set.
    void convertAndAppend( const hydroogmap::OgMap           &ogMap,
                           const hydropathplan::Cell2DVector &input,
                           orca::PathPlanner2dData           &output,
                           double                             firstHeading = 0.0  );
    
    //! As above but also adds parameters.
    void convertAndAppend( const hydroogmap::OgMap               &ogMap,
                           const hydropathplan::Cell2DVector     &input,
                           const std::vector<WaypointParameter>  &wpPara,
                           orca::PathPlanner2dData               &output,
                           double                                 firstHeading = 0.0 );
    
    //! Sets all parameters of pathData contained in vector wpPara
    void setParameters( orca::PathPlanner2dData              &pathData,
                        const std::vector<WaypointParameter> &wpPara );

}
#endif
