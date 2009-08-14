/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef HYDRO_PATHPLAN_UTIL_H
#define HYDRO_PATHPLAN_UTIL_H

#include <vector>
#include <hydroogmap/hydroogmap.h>
#include "cell2d.h"
#include "typemap.h"

namespace hydropathplan
{

    //! A function for setting the cost based on the distance from an obstacle
    class CostEvaluator {
    public:
        virtual ~CostEvaluator() {}
        
        // returns the cost of moving through a cell whose 4-distance to the nearest obstacle is distInMetres.
        virtual double costAtDistFromObstacle( double distInMetres ) const=0;
    };

    //! A reasonable default: all non-obstacle cells have unit cost
    class DefaultCostEvaluator : public CostEvaluator {
    public:
        double costAtDistFromObstacle( double distInMetres ) const
            { if ( distInMetres > 0 ) return 1; else return NAN; }
    };

// The 'util' namespace contains stuff that's not intended for use outside of this library.
namespace util {

    //! Returns: 
    //! - TRUE if cell is stricly < traversabilityThreshhold, 
    //! - FALSE if it's on the edge of the map
    bool isTraversable( const hydroogmap::OgMap &ogMap,
                        const int indX,
                        const int indY,
                        const float traversabilityThreshhold);

    //! Convenience function, see above
    inline bool isTraversable( const hydroogmap::OgMap &ogMap,
                               const Cell2D            &cell,
                               const float              traversabilityThreshhold)
    { return isTraversable(ogMap,cell.x(),cell.y(),traversabilityThreshhold); }

    //! Checks if at least one of the 4-neighbours are travesable.
    bool isTraversableNeighbors( const hydroogmap::OgMap &ogMap,
                                 const int indX,
                                 const int indY,
                                 const float traversabilityThreshhold );

    //! Checks if all the 8-neighbours are traversable.
    bool isTraversableAll8Neighbors( const hydroogmap::OgMap &ogMap,
                                 const int indX,
                                 const int indY,
                                 const float traversabilityThreshhold );

    //! Does Line-Of-Sight exist between the two cells?
    bool losExists( const hydroogmap::OgMap &ogMap,
                    double traversabilityThreshhold,
                    const Cell2D &c1,
                    const Cell2D &c2 );

    //! Returns the diameter of the robot in cells depending on the resolution of the ogmap. 
    //! If x and y resolutions are different, the smaller one is used (yielding the larger diameter in cells)
    int robotDiameterInCells( const hydroogmap::OgMap &ogMap, const double robotDiameterMetres );

    //! Modifies ogMap: grows obstacles using the diameter of the robot and the 
    //! traversabilityThreshhold (obstacles are represented by OCCUPIED).
    //  TODO: AlexB: Why grow by the diameter rather than the radius??
    void growObstaclesOgMap( hydroogmap::OgMap &ogMap,
                             const double       traversabilityThreshhold,
                             const int          robotDiameterCells );

    //! Optimizes a long path into a few waypoints only, by removing un-necessary waypoints.
    //! Doesn't take the size of the robot into account -- if you want to do so, grow the
    //! obstacles first.
    //!
    //! This function returns a subset of the cells in origPath.  It could be improved (generating a shorter path)
    //! by considering cells other than just those in origPath.
    //!
    void optimizePath( const hydroogmap::OgMap &ogMap,
                       double                   traversabilityThreshhold,
                       const Cell2DVector      &origPath,
                       Cell2DVector            &optimisedPath );

    //! calculates the cost in a straight line, using costs from the costMap.
    float straightLineCost( const Cell2D   &fromCell,
                            const Cell2D   &toCell,
                            const FloatMap &costMap );
    
}

}

#endif

