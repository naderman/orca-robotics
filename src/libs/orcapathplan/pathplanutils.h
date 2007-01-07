/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_PATHPLAN_UTILS_H
#define ORCA2_PATHPLAN_UTILS_H

#include <vector>
#include <orcaogmap/orcaogmap.h>
#include "cell2d.h"
#include "typemap.h"

namespace orcapathplan
{
    typedef TypeMap<float> FloatMap;
    typedef TypeMap<int> IntMap;
    
    enum Result 
    { 
        PathOk, 
        PathStartNotValid, 
        PathDestinationNotValid, 
        PathDestinationUnreachable,
        OtherError
    };

// =========== ELEMENT ACCESS (without specializing templated TypeMap) =========
    
    //! Gets the value of cell with index x,y. Will return NaN if cell is outside the map
    float element( const FloatMap &floatMap, int x, int y );      
    
    //! Convenience function, see above
    float element( const FloatMap &floatMap,  Cell2D c );
    
// ================= CHECKING FUNCTIONS ================================
    //! Checks whether all values are nans
    bool areAllNans( const FloatMap &floatMap );
    
    //! Does Line-Of-Sight exist between the two cells?
    bool losExists( const orcaogmap::OgMap &ogMap,
                    double traversabilityThreshhold,
                    const Cell2D &c1,
                    const Cell2D &c2 );
    
    //! Returns TRUE if c is in C otherwise false
    bool isIncluded( const Cell2DVector & C, const Cell2D & c );
    //! Returns TRUE if c is in C otherwise false
    bool isIncluded( const Cell2DList & C, const Cell2D & c );

    //! Returns: 
    //! - TRUE if cell is stricly < traversabilityThreshhold, 
    //! - FALSE if it's on the edge of the map
    bool isTraversable( const orcaogmap::OgMap &ogMap,
                        const int indX,
                        const int indY,
                        const float traversabilityThreshhold);

    //! Convenience function, see above
    inline bool isTraversable( const orcaogmap::OgMap & ogMap,
                               const Cell2D cell,
                               const float traversabilityThreshhold)
    { return isTraversable(ogMap,cell.x(),cell.y(),traversabilityThreshhold); }

    //! Checks whether the cell contains a nan value (returns TRUE if yes, otherwise FALSE). 
    //! If cell is outside map, FALSE is returned.
    inline bool containsNan( const FloatMap & navMap, const int x, const int y) 
    { return ( isnan(element(navMap, x, y )) ); };
    //! Convenience function, see above
    inline bool containsNan( const FloatMap & navMap, const Cell2D & c ) 
    { return containsNan(navMap,c.x(),c.y()); };
// ======================================================================
    
// ================= CONVERSION FUNCTIONS ================================ 
    //! Converts a 2d grid reference into a 1d index, check that the resulting index is within the specified size limit
    int sub2ind( const int          &indX,
                 const int          &indY,
                 const unsigned int &sizeX,
                 const unsigned int &sizeY );
            
    //! Convenience function, see above
    int sub2ind( const Cell2D       &cell,
                 const unsigned int &sizeX,
                 const unsigned int &sizeY );
    
    //! Converts a 1d index into a 2d cell, checks that the supplied indeces are within the specified size
    Cell2D ind2sub( const int          &index,
                    const unsigned int &sizeX,
                    const unsigned int &sizeY );
// =====================================================================

// ================= SIMPLE NAVIGATION FUNCTIONS ============================ 
    //! Calculates simple navigation function. Returns false if unsuccessful
    //! (algorithm described by Latombe, NF1, ch.4.2.1, p.322)
    bool calcSimpleNavigation( const orcaogmap::OgMap &ogMap,
                               FloatMap               &navMap,
                               Cell2D                 &startCell,
                               double                  traversabilityThreshhold,
                               const double            robotDiameterMetres );

    //! Modifies navMap: grows obstacles using the diameter of the robot (obstacles are represented by NANs)
    void growObstaclesNavMap( const orcaogmap::OgMap &ogMap,
                              FloatMap               &navMap,
                              double                  traversabilityThreshhold,
                              const double            robotDiameterMetres );

    //! Modifies ogMap: grows obstacles using the diameter of the robot and the 
    //! traversabilityThreshhold (obstacles are represented by OCCUPIED).
    void growObstaclesOgMap( orcaogmap::OgMap &ogMap,
                             const double      traversabilityThreshhold,
                             const int         robotDiameterCells );
// =========================================================================

// ============== SKELETON NAVIGATION FUNCTIONS =============================

    //
    // Notes on parameters:
    //
    // - distGrid:
    //   - for every OG cell, stores the distance to the nearest obstacle cell
    //     - used in generating the skeleton
    //     - useful for connecting free space cells to the skeleton (just follow the gradient of distGrid until you hit the skeleton)
    //
    // - navMap: 
    //   - contains the potential at each OG cell
    //     - potential = cost_to_goal, NAN means obstacle or close to obstacle
    //     - following the gradient leads to the goal
    //
    // - skel:
    //   - A vector of cells which lie on the skeleton
    //
    
    //! Computes the skeleton for a given OgMap. Returns false if skeleton can't be computed.
    //! Outputs (needn't be initialised before calling this function):
    //!  - navMap
    //!  - skel
    //!  - distGrid
    bool computeSkeleton( const orcaogmap::OgMap &ogMap,
                          FloatMap               &navMap,
                          Cell2DVector           &skel,
                          FloatMap               &distGrid,
                          double                  traversabilityThreshhold,
                          double                  robotDiameterMetres );

    
    //! Finds the point on the skeleton nearest to fromCell.
    //! returns false on failure.
    bool findClosestPointOnSkeleton( const Cell2DVector   &skel,
                                     const FloatMap       &distGrid,
                                     const Cell2D         &fromCell,
                                     Cell2D               &closestCell );

    //! Connects a cell to a given skeleton, by surfing the gradient of distGrid away from obstacles
    bool connectCell2Skeleton( Cell2DVector   &skel,
                               Cell2D         &cell,
                               const FloatMap &navMap,
                               const FloatMap &distGrid,
                               int             robotRadiusCells );

    //! Computes the potential function U along the skeleton
    bool computePotentialSkeleton( const orcaogmap::OgMap &ogMap,
                                   FloatMap               &navMap,
                                   const Cell2DVector     &skel,
                                   const Cell2D           &startCell );

    //! Computes the potential function U in the free space (not on the skeleton)
    void computePotentialFreeSpace( const orcaogmap::OgMap &ogMap,
                                    FloatMap               &navMap,
                                    const Cell2DVector     &skel,
                                    double                  traversabilityThreshhold );
    
    //! Convenience function that calls the last four functions sequentially 
    //! (algorithm as described by Latombe, SKELETON and NF2, ch.4.2.2, p.324).
    //! Returns false if unsuccessful.
    bool calcSkeletonNavigation( orcaogmap::OgMap &ogMap,
                                 FloatMap         &navMap,
                                 Cell2D           &startCell,
                                 double            traversabilityThreshhold,
                                 double            robotDiameterMetres);
    
// =========================================================================

// ================= GENERAL PATHPLANNING FUNCTIONS ====================
    //! Finds a series of connected cells using gradient descent on precalculated navigation function. Returns result codes.
    Result calcPath( const orcaogmap::OgMap &ogMap,
                     const FloatMap         &navMap,
                     Cell2D                 &goalCell,
                     Cell2DVector           &path,
                     double                  robotDiameterMetres );

    //! Returns the diameter of the robot in cells depending on the resolution of the ogmap. 
    //! If x and y resolutions are different, the smaller one is used (yielding the larger diameter in cells)
    int robotDiameterInCells( const orcaogmap::OgMap & ogMap, const double robotDiameterMetres );

    //! Optimizes a long path into a few waypoints only, by removing un-necessary waypoints.
    //! Doesn't take the size of the robot into account -- if you want to do so, grow the
    //! obstacles first.
    //!
    //! This function returns a subset of the cells in origPath.  It could be improved (generating a shorter path)
    //! by considering cells other than just those in origPath.
    //!
    void optimizePath( const orcaogmap::OgMap &ogMap,
                       double                  traversabilityThreshhold,
                       const Cell2DVector     &origPath,
                       Cell2DVector           &optimisedPath );

// =====================================================================

}

#endif

