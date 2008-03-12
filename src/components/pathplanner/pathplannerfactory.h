#ifndef PATHPLANNER_PATHPLANNERFACTORY_H
#define PATHPLANNER_PATHPLANNERFACTORY_H

#include <orcaice/context.h>
#include <hydroutil/hydroutil.h>
#include <hydroogmap/hydroogmap.h>
#include <hydropathplan/hydropathplan.h>

namespace pathplanner {

//
// @author Alex Brooks
//
class PathPlannerFactory
{

public: 

    // Need the context to set up skeleton graphics
    PathPlannerFactory( const orcaice::Context &context )
        : orcaIceContext_(context)
        {}

    // Allocates memory.
    hydropathplan::IPathPlanner2d *getPathPlanner( hydroogmap::OgMap  &ogMap, 
                                                   double              traversabilityThreshhold,
                                                   double              robotDiameterMetres,
                                                   hydroutil::Context  context );

private: 

    orcaice::Context orcaIceContext_;

};

}

#endif
