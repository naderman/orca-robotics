/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef SPARSESKELETONPATHPLANNER_H
#define SPARSESKELETONPATHPLANNER_H

#include <orcapathplan/ipathplanner2d.h>
#include <orcapathplan/pathplanutils.h>
#include <orcaogmap/orcaogmap.h>
#include <orcapathplan/sparseskel.h>

namespace orcapathplan {

//!
//! @author Alex Brooks
//!
class SparseSkeletonPathPlanner : public IPathPlanner2d
{

public: 

    //!
    //! Warning: may throw 'orcapathplan::Exception's
    //!
    SparseSkeletonPathPlanner( const hydroogmap::OgMap &ogMap,
                               double robotDiameterMetres,
                               double traversabilityThreshhold,
                               bool   doPathOptimization,
                               const CostEvaluator &costEvaluator=DefaultCostEvaluator() );

    //!
    //! The cells in 'path' are guaranteed to be in line-of-sight of
    //! one another, but are not guaranteed to be adjacent to one another.
    //!
    //! Warning: may throw 'orcapathplan::Exception's
    //!
    void computePath( int           startX,
                      int           startY,
                      int           endX,
                      int           endY,
                      Cell2DVector &path ) const;

    const Cell2DVector &denseSkel() const  { return skel_; }
    const SparseSkel   &sparseSkel() const { return *sparseSkel_; }

private: 

    void checkInputs( int           startX,
                      int           startY,
                      int           endX,
                      int           endY ) const;

    void findClosestNode( const Cell2D                        &cell,
                          ContiguousSparseSkel               *&cSkel,
                          SparseSkelNode                     *&closestNode,
                          const std::vector<SparseSkelNode*>  &nodesToExclude ) const;

    void connectCell2SparseSkel( const Cell2D          &cellOnDenseSkel,
                                 ContiguousSparseSkel *&contiguousSparseSkel,
                                 SparseSkelNode       *&node ) const;

    void planAlongSparseSkel( const ContiguousSparseSkel   &contiguousSparseSkel,
                              const SparseSkelNode         &startNode, 
                              const SparseSkelNode         &goalNode,
                              std::vector<const SparseSkelNode*> &nodePath ) const;

    //
    // Takes: a set of start/goal cells off the sparse skel, and a set of nodes,
    //        and makes a single complete path.
    //
    void convertToCellVector( const std::vector<Cell2D>          &startCells,
                              const std::vector<Cell2D>          &goalCells,
                              std::vector<const SparseSkelNode*> &nodePath,
                              Cell2DVector                       &path ) const;

    void optimisePath( Cell2DVector &path ) const;

    const hydroogmap::OgMap &ogMap_;

    // OgMap with obstacles grown
    hydroogmap::OgMap        grownOgMap_;

    // (a pointer:) The map that the sparse planner uses
    const hydroogmap::OgMap *planOgMap_;

    Cell2DVector skel_;
    SparseSkel   *sparseSkel_;
    FloatMap     costMap_;

    const double robotDiameterMetres_;
    const double traversabilityThreshhold_;
    const bool   doPathOptimization_;
};

}

#endif
