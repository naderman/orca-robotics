/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef HYDRO_PATHPLAN_LATOMBE_UTIL_H
#define HYDRO_PATHPLAN_LATOMBE_UTIL_H

#include <vector>
#include <hydroogmap/hydroogmap.h>
#include <hydropathplan/util.h>

namespace hydropathplan
{

namespace latombeutil {

    // Distance to a 4-adjacent cell
    const float AC = 1.0;
    // Distance to a diagonal 8-adjacent cell
    const float DC = 1.4142;

// ================= CHECKING FUNCTIONS ================================
    //! Checks whether all values are nans
    bool areAllNans( const FloatMap &floatMap );
    
    //! Checks whether the cell contains a nan value (returns TRUE if yes, otherwise FALSE). 
    //! If cell is outside map, FALSE is returned.
    inline bool containsNan( const FloatMap & navMap, const int x, const int y) 
#ifdef __QNX__
    { return ( isnan(element(navMap, x, y )) ); };
#else
    { return ( std::isnan(element(navMap, x, y )) ); };
#endif
    //! Convenience function, see above
    inline bool containsNan( const FloatMap & navMap, const Cell2D & c ) 
    { return containsNan(navMap,c.x(),c.y()); };

    // Checks whether cell is in a free space region.
    // If 'checkForNan' is set, 'nan' means non-free.  Else '0' means non-free.
    // If not, attempts to move it into one of the numCells surrounding cells. 
    // If successful return TRUE, otherwise FALSE.
    bool validateCell( const FloatMap & theMap, Cell2D & c, int numCells, bool checkForNan );

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

// =========================================================================

// ============== SKELETON NAVIGATION FUNCTIONS =============================

    //
    // Notes on parameters:
    //
    // - distGrid:
    //   - for every OG cell, stores the 4-distance to the nearest obstacle cell
    //   - zero means 'obstacle in this cell'.
    //     - used in generating the skeleton
    //     - useful for connecting free space cells to the skeleton (just follow the gradient of distGrid until you hit the skeleton)
    //
    // - costMap: 
    //   - stores the cost of moving through each cell
    //   - 'NAN' represents infinite cost.
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
    //!  - skel
    //!  - distGrid
    bool computeSkeleton( const hydroogmap::OgMap &ogMap,
                          Cell2DVector           &skel,
                          FloatMap               &distGrid,
                          double                  traversabilityThreshhold,
                          double                  robotDiameterMetres );

    //! Finds the point on the skeleton nearest to fromCell, by surfing the gradient of distGrid.
    //! returns false on failure.
    bool findClosestPointOnSkeleton( const Cell2DVector   &skel,
                                     const FloatMap       &distGrid,
                                     const Cell2D         &fromCell,
                                     Cell2D               &closestCell );

    //! Connects a cell to a given skeleton, by surfing the gradient of distGrid away from obstacles
    //! Modifies skel to include the connection.
    bool connectCell2Skeleton( Cell2DVector   &skel,
                               Cell2D         &cell,
                               const FloatMap &distGrid,
                               int             robotRadiusCells );

    //! Computes the potential function U along the skeleton
    bool computePotentialSkeleton( const hydroogmap::OgMap &ogMap,
                                   const FloatMap         &costMap,
                                   FloatMap               &navMap,
                                   const Cell2DVector     &skel,
                                   const Cell2D           &startCell );

    //! Computes the potential function U in the free space (not on the skeleton)
    void computePotentialFreeSpace( const hydroogmap::OgMap &ogMap,
                                    const FloatMap         &costMap,
                                    FloatMap               &navMap,
                                    const Cell2DVector     &skel,
                                    double                  traversabilityThreshhold );

    //! Convenience function that calls the other functions sequentially 
    //! (algorithm as described by Latombe, SKELETON and NF2, ch.4.2.2, p.324).
    //! Returns false if unsuccessful.
    bool calcSkeletonNavigation( const hydroogmap::OgMap &ogMap,
                                 FloatMap               &navMap,
                                 Cell2D                 &startCell,
                                 double                  traversabilityThreshhold,
                                 double                  robotDiameterMetres);

    //! Generate costs from distGrid
    void computeCostsFromDistGrid( const FloatMap      &distGrid,
                                   FloatMap            &costMap,
                                   double               metresPerCell,
                                   const CostEvaluator &costEvaluator=DefaultCostEvaluator() );

    void computeUniformCosts( const hydroogmap::OgMap &ogMap,
                              FloatMap               &costMap,
                              double                  traversabilityThreshhold );

// =========================================================================

// ================= GENERAL PATHPLANNING FUNCTIONS ====================

    //! Results returned by calcPath
    enum Result 
    { 
        PathOk, 
        PathStartNotValid, 
        PathDestinationNotValid, 
        PathDestinationUnreachable,
        OtherError
    };

    //! Finds a series of connected cells using gradient descent on precalculated navigation function. Returns result codes.
    Result calcPath( const hydroogmap::OgMap &ogMap,
                     const FloatMap         &navMap,
                     Cell2D                 &goalCell,
                     Cell2DVector           &path,
                     double                  robotDiameterMetres );

// =====================================================================

}

}

#endif

