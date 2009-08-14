/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef SPARSESKELETONPATHPLANNER_H
#define SPARSESKELETONPATHPLANNER_H

#include <hydropathplan/ipathplanner2d.h>
#include <hydropathplan/util.h>
#include <hydroogmap/hydroogmap.h>

namespace hydropathplan {

// fwd decl
namespace sparseskel {
    class SparseSkel;
    class ContiguousSparseSkel;
    class SparseSkelNode;
}

//!
//! @brief Similar to SkeletonPathPlanner, but faster.
//!
//! Works by taking the output of Skeleton path planning, and
//! extracting a set of nodes (ie junctions in the skeleton).
//!
//! Plans using this sparse set of nodes (and links between them)
//! rather than the full skeleton, which is much faster.
//!
//! @author Alex Brooks
//!
class SparseSkeletonPathPlanner : public IPathPlanner2d
{

public: 

    //!
    //! Warning: may throw 'hydropathplan::Exception's
    //!
    SparseSkeletonPathPlanner( const  hydroogmap::OgMap &ogMap,
                               double robotDiameterMetres,
                               double traversabilityThreshhold,
                               bool   doPathOptimization,
                               bool   addExtraNodes,
                               double extraNodeResolution,
                               bool   jiggleBadWaypoints,
                               const  CostEvaluator &costEvaluator=DefaultCostEvaluator() );

    ~SparseSkeletonPathPlanner();

    //!
    //! The cells in 'path' are guaranteed to be in line-of-sight of
    //! one another, but are not guaranteed to be adjacent to one another.
    //!
    //! Warning: may throw 'hydropathplan::Exception's
    //!
    void computePath( int           startX,
                      int           startY,
                      int           endX,
                      int           endY,
                      Cell2DVector &path ) const;

    // Allow access for debugging/displaying
    const Cell2DVector &denseSkel() const  { return skel_; }
    const sparseskel::SparseSkel   &sparseSkel() const { return *sparseSkel_; }

private: 

    // May modify the inputs if jiggleBadWaypoints is set.
    void checkAndFixInputs( int           &startX,
                            int           &startY,
                            int           &endX,
                            int           &endY,
                            bool          jiggleBadWaypoints ) const;

    void findClosestNode( const Cell2D                                    &cell,
                          sparseskel::ContiguousSparseSkel               *&cSkel,
                          sparseskel::SparseSkelNode                     *&closestNode,
                          const std::vector<sparseskel::SparseSkelNode*>  &nodesToExclude ) const;

    void connectCell2SparseSkel( const Cell2D                      &cellOnDenseSkel,
                                 sparseskel::ContiguousSparseSkel *&contiguousSparseSkel,
                                 sparseskel::SparseSkelNode       *&node ) const;

    void planAlongSparseSkel( const sparseskel::ContiguousSparseSkel         &contiguousSparseSkel,
                              const sparseskel::SparseSkelNode               &startNode, 
                              const sparseskel::SparseSkelNode               &goalNode,
                              std::vector<const sparseskel::SparseSkelNode*> &nodePath ) const;

    //
    // Takes: a set of start/goal cells off the sparse skel, and a set of nodes,
    //        and makes a single complete path.
    //
    void convertToCellVector( const std::vector<Cell2D>                      &startCells,
                              const std::vector<Cell2D>                      &goalCells,
                              std::vector<const sparseskel::SparseSkelNode*> &nodePath,
                              Cell2DVector                                   &path ) const;

    void optimisePath( Cell2DVector &path ) const;

    const hydroogmap::OgMap &ogMap_;

    // OgMap with obstacles grown
    hydroogmap::OgMap        grownOgMap_;

    // (a pointer:) The map that the sparse planner uses
    const hydroogmap::OgMap *planOgMap_;

    Cell2DVector            skel_;
    sparseskel::SparseSkel *sparseSkel_;
    FloatMap                costMap_;

    const double robotDiameterMetres_;
    const double traversabilityThreshhold_;
    const bool   doPathOptimization_;

    const bool jiggleBadWaypoints_;
};

}

#endif
