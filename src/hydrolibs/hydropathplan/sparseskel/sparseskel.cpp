/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include "sparseskel.h"
#include <hydropathplan/ipathplanner2d.h>
#include <gbxutilacfr/mathdefs.h>
#include <hydroogmap/oglostracer.h>
#include <iostream>
#include <sstream>
#include <algorithm>
using namespace std;

#define SUPER_DEBUG 1

namespace hydropathplan {

namespace sparseskel {

namespace {

    // helper function for debugging
    void printCellList( const Cell2DList &l )
    {
        for ( Cell2DList::const_iterator it=l.begin(); it != l.end(); it++ )
        {
            cout << "  " << *it << endl;
        }
    }

    void findWps( const Cell2DVector        &skel,
                  Cell2DList                &wps )
    {
        // A non-wp is defined as a cell with 2 non-adjacent adjacencies.
        // A wp is anything else.
        
        std::vector<Cell2D> adjacencies;

        // This could _definitely_ be optimised...
        for ( unsigned int i=0; i < skel.size(); i++ )
        {
            adjacencies.clear();

            for ( unsigned int j=0; j < skel.size(); j++ )
            {
                if ( (isAdjacentN( skel[i], skel[j], 1 )) && (skel[i] != skel[j]) )
                {
                    adjacencies.push_back( skel[j] );
                }
            }
            if ( adjacencies.size() == 2 &&
                 !isAdjacentN( adjacencies[0], adjacencies[1], 1 ) )
            {
                // not a wp
            }
            else
            {
                wps.push_back( skel[i] );
            }
        }
    }

    //
    // rayTracer isn't necessarily symmetric.
    // This tests both directions.
    //
    bool directLinkExists( const hydroogmap::OgMap &ogMap,
                           double                  traversabilityThreshhold,
                           const Cell2D           &p1,
                           const Cell2D           &p2 )
    {
        return ( util::losExists( ogMap, traversabilityThreshhold, p1, p2 ) &&
                 util::losExists( ogMap, traversabilityThreshhold, p2, p1 ) );
    }

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

SparseSkel::SparseSkel( const hydroogmap::OgMap &ogMap,
                        double                  traversabilityThreshhold,
                        const Cell2DVector     &skel,
                        const FloatMap         &costMap,
                        bool                    addExtraNodes,
                        double                  extraNodeResolution )
    : ogMap_(ogMap),
      traversabilityThreshhold_(traversabilityThreshhold)
{
    cout<<"TRACE(sparseskel.cpp): SparseSkel: constructor" << endl;

    extraNodesOgMap_ = ogMap_;
    util::growObstaclesOgMap( extraNodesOgMap_, traversabilityThreshhold_, 2 );

    Cell2DList wps;

    cout<<"TRACE(sparseskel.cpp): finding wps ... "<<endl;
    findWps( skel, wps );
    cout<<"  ok"<<endl;

    // Convert skel from a vector to a list
    cout<<"TRACE(sparseskel.cpp): converting skel from vector to list ... " << endl;
    Cell2DList skelList;
    for ( unsigned int i=0; i < skel.size(); i++ )
        skelList.push_back( skel[i] );
    cout<<"  ok"<<endl;

    build( skelList, wps, costMap, addExtraNodes, extraNodeResolution );
}

SparseSkel::~SparseSkel()
{
    for ( unsigned int i=0; i < contiguousSkels_.size(); i++ )
    {
        delete contiguousSkels_[i];
    }
}

int
SparseSkel::numNodes() const
{
    int num = 0;
    for ( unsigned int i=0; i < contiguousSkels_.size(); i++ )
    {
        num += contiguousSkels_[i]->nodes().size();
    }
    return num;
}

void 
SparseSkel::build( Cell2DList                &skelList,
                   Cell2DList                &wps,
                   const FloatMap            &costMap,
                   bool                       addExtraNodes,
                   double                     extraNodeResolution )
{
    if ( wps.size() == 0 )
        throw hydropathplan::Exception( ERROR_INFO, "SparseSkel: Couldn't find any wps!" );

    while ( wps.size() > 0 )
    {
        //cout<<"TRACE(skeletonutils.cpp): ============================" << endl;
        //cout<<"TRACE(skeletonutils.cpp): starting new contiguous skel for wps:" << endl;
        //for ( Cell2DList::iterator it=wps.begin(); it != wps.end(); it++ )
        //{
        //    cout << "  " << *it << endl;
        //}

        //
        // the ContiguousSparseSkel constructor removes its waypoints from wps.
        //
        contiguousSkels_.push_back( new ContiguousSparseSkel( *this,
                                                              wps,
                                                              skelList,
                                                              costMap,
                                                              addExtraNodes,
                                                              extraNodeResolution ) );
    }

#if SUPER_DEBUG
    if ( skelList.size() != 0 )
    {
        cout << "ERROR(skeletonutils.cpp): Left-over skelList items:" << endl;
        printCellList( skelList );        
    }
    assert( skelList.size() == 0 );
#endif
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//
// The sparse skeleton building algorithm works like this:
//
// - Each wp has n exit points.
//   - traverse the skeleton along each exit point to link to new wps
//     - this traversal is guaranteed to end at exactly one wp.
//       (if it could end in >1 wps, that junction is itself a wp)
//

ContiguousSparseSkel::ContiguousSparseSkel( SparseSkel     &parent,
                                            Cell2DList     &wps,
                                            Cell2DList     &skel,
                                            const FloatMap &costMap,
                                            bool            addExtraNodes,
                                            double          extraNodeResolution )
    : parent_(parent)
{
    cout<<"TRACE(sparseskel.cpp): ContiguousSparseSkel: constructor" << endl;

    // check assumptions
    assert( wps.size() > 0 );
    assert( skel.size() > 0 );

    // Pick a wp to start from
    Cell2DList ungrownCells;
    ungrownCells.push_back( wps.front() );
    skel.remove( wps.front() );

    // create an initial node
    nodes_.push_back( new SparseSkelNode( ungrownCells.front() ) );
    if ( !util::isTraversable( parent_.ogMap(), ungrownCells.front(), traversabilityThreshhold() ) )
    {
        std::stringstream ss;        
        ss << "While building sparse skel: dense skeleton crossed non-traversable area at OG Cell " << ungrownCells.front();
        throw hydropathplan::Exception( ERROR_INFO, ss.str() );
    }

    // Keep growing out from ungrown cells till there are no more cells to grow from
    while ( ungrownCells.size() > 0 )
    {
        // Create a new node from this ungrown cell
        SparseSkelNode *nodeToGrow = findNode( ungrownCells.front() );
        assert( nodeToGrow != NULL );
        ungrownCells.pop_front();

        // and grow it
        growNode( nodeToGrow, ungrownCells, wps, skel );
    }

    // modify wps, stripping the ones we found.
    //cout<<"TRACE(sparseskel.cpp): original wps was of size " << wps.size() << endl;
    //cout<<"TRACE(sparseskel.cpp): Found " << nodes_.size() << " nodes:" << endl;
    for ( unsigned int i=0; i < nodes_.size(); i++ )
    {
        Cell2D wpCell = nodes_[i]->pos;
        //cout<<"  " << wpCell << endl;
        Cell2DList::iterator i = find( wps.begin(), wps.end(), wpCell );

        // if required because node may be a stepping-stone node.
        if ( i != wps.end() )
            wps.erase( i );
    }
    //cout<<"TRACE(sparseskel.cpp): wps is now of size " << wps.size() << endl;


    cout<<"TRACE(sparseskel.cpp): merging adjacent nodes ... "<<endl;
    mergeAdjacentNodes();
    assert( isSane() );
    cout << "  ok" << endl;

    if ( addExtraNodes )
    {
        cout<<"TRACE(sparseskel.cpp): adding extra nodes ... ";
        const int stepInCells = (int)(extraNodeResolution / parent_.extraNodesOgMap().metresPerCell());
        // cout<<"TRACE(sparseskel.cpp): stepInCells: " << stepInCells << endl;
        assert( stepInCells > 1 );
        this->addExtraNodes( parent_.extraNodesOgMap(), stepInCells );
        assert( isSane() );
        cout << "ok" << endl;
    }

    cout<<"TRACE(sparseskel.cpp): setting arc costs ... ";
    setArcCosts( costMap );
    assert( isSane() );
    cout << "ok" << endl;
}

ContiguousSparseSkel::~ContiguousSparseSkel()
{
    // cout<<"TRACE(sparseskel.cpp): ~ContiguousSparseSkel()" << endl;
    for ( unsigned int i=0; i < nodes_.size(); i++ )
    {
        delete nodes_[i];
    }
}

bool
areLinked( const SparseSkelNode *a, const SparseSkelNode *b )
{
    bool aLinksb=false;

    for ( unsigned int i=0; i < a->arcs.size(); i++ )
    {
        if ( a->arcs[i]->toNode == b )
            aLinksb = true;
    }
#if SUPER_DEBUG
    bool bLinksa=false;

    for ( unsigned int i=0; i < b->arcs.size(); i++ )
    {
        if ( b->arcs[i]->toNode == a )
            bLinksa = true;
    }

    assert( bLinksa == aLinksb );
#endif
    return aLinksb;
}

bool
ContiguousSparseSkel::canMerge( SparseSkelNode *slave, SparseSkelNode *master )
{
    assert( master != slave );

    // go through all the nodes
    for ( unsigned int i=0; i < nodes_.size(); i++ )
    {
        // go through each node's arcs
        for ( unsigned int j=0; j < nodes_[i]->arcs.size(); j++ )
        {
            if ( nodes_[i]->arcs[j]->toNode == slave )
            {
                if ( nodes_[i] == master || nodes_[i] == slave ) continue;

                // check if we can switch link from slave to master
                if ( !directLinkExists( parent_.ogMap(), traversabilityThreshhold(), nodes_[i]->pos, master->pos ) )
                {
                    // It's not OK -- we'd break line-of-sight.
                    return false;
                }
            }
        }
    }
    return true;
}

void
ContiguousSparseSkel::merge( SparseSkelNode *slave, SparseSkelNode *master )
{
    // go through all the nodes, linking to master if linked to slave
    for ( unsigned int i=0; i < nodes_.size(); i++ )
    {
        SparseSkelNode *considerNode = nodes_[i];
        if ( considerNode == slave || considerNode == master ) continue;

        // go through each node's arcs
        for ( unsigned int j=0; j < considerNode->arcs.size(); j++ )
        {
            // if we currently link to slave,
            if ( considerNode->arcs[j]->toNode == slave )
            {
                // and we're not already linked to master,
                if ( !areLinked( considerNode, master ) )
                {
                    // then link to master
                    const float cost = -1;
                    considerNode->arcs[j]->toNode = master;
                    considerNode->arcs[j]->cost   = cost;
                    master->arcs.push_back( new SparseSkelArc( considerNode, cost ) );
                }
            }
        }
    }

    // cout<<"TRACE(sparseskel.cpp): removing links to " << slave->pos << endl;

    // go through all the nodes, removing links to slave
    for ( unsigned int i=0; i < nodes_.size(); i++ )
    {
        SparseSkelNode *considerNode = nodes_[i];
        if ( considerNode == slave ) continue;

        while ( true )
        {
            std::vector<SparseSkelArc*>::iterator it;
            for ( it = considerNode->arcs.begin();
                  it != considerNode->arcs.end();
                  it++ )
            {
                if ( (*it)->toNode == slave ) break;
            }
            if ( it != considerNode->arcs.end() )
                considerNode->arcs.erase( it );
            else
                break;
        }
    }
    // cout<<"TRACE(sparseskel.cpp): Deletion done." << endl;

#if SUPER_DEBUG
    // Double-check.
    for ( unsigned int i=0; i < nodes_.size(); i++ )
    {
        if ( nodes_[i] == slave ) continue;

        for ( unsigned int j=0; j < nodes_[i]->arcs.size(); j++ )
        {
            if ( nodes_[i]->arcs[j]->toNode == slave )
            {
                assert( false );
            }
        }
    }
#endif
}

void
ContiguousSparseSkel::mergeAdjacentNodes()
{
    // Have to be careful...  We're iterating through nodes_ *and* modifying it.

    bool changed = true;
    while ( changed )
    {
        // cout<<"TRACE(sparseskel.cpp): --------- mergeAdjacentNodes loop ---------" << endl;
        changed = false;

        for ( std::vector<SparseSkelNode*>::iterator i=nodes_.begin();
              i != nodes_.end(); 
              ++i )
        {
            if ( changed ) break;
            for ( std::vector<SparseSkelNode*>::iterator j=nodes_.begin();
                  j != nodes_.end(); 
                  ++j )
            {
                if ( (*i) == (*j) ) continue;
                if ( isAdjacentN( (*i)->pos, (*j)->pos, 1 ) )
                {
                    if ( canMerge( *i, *j ) )
                    {
                        // cout<<"TRACE(sparseskel.cpp): mergeAdjacentNodes(): erasing " << (*i)->pos << endl;
                        merge( *i, *j );
                        changed = true;
                        nodes_.erase( i );
                        break;
                    }
                }
            }
        }
    }

#if SUPER_DEBUG
    for ( std::vector<SparseSkelNode*>::iterator i=nodes_.begin();
          i != nodes_.end(); 
          ++i )
    {
        for ( std::vector<SparseSkelNode*>::iterator j=nodes_.begin();
              j != nodes_.end(); 
              ++j )
        {
            if ( *i == *j ) continue;
            if ( isAdjacentN( (*i)->pos, (*j)->pos, 1 ) )
            {
                if ( canMerge( *i, *j ) )
                {
                    cout<<"TRACE(sparseskel.cpp): WARN: nodes " << (*i)->pos << " and " << (*i)->pos << " are adjacent." << endl;
                    cout<<"TRACE(sparseskel.cpp): They can be merged, why weren't they?" << endl;
                    assert( false );
                }
            }
//             for ( unsigned int k=0; k < (*i)->arcs.size(); k++ )
//             {
//                 if ( (*i)->arcs[k]->cost <= 1.42 )
//                 {
//                     // arc spans only one cell
//                     if ( canMerge( *i, (*i)->arcs[k]->toNode ) )
//                     {
//                         cout<<"TRACE(sparseskel.cpp): WARN: node " << (*i)->pos << "'s arc spans only one cell (to " << (*i)->arcs[k]->toNode->pos << ")" << endl;
//                         cout<<"TRACE(sparseskel.cpp): They can be merged, why weren't they?" << endl;
//                         assert( false );
//                     }
//                 }
//             }
        }        
    }    
#endif
}

void
ContiguousSparseSkel::setArcCosts( const FloatMap &costMap )
{
    for ( unsigned int i=0; i < nodes_.size(); i++ )
    {
        SparseSkelNode &node = *(nodes_[i]);
        for ( unsigned int j=0; j < node.arcs.size(); j++ )
        {
            SparseSkelArc &arc = *(node.arcs[j]);
            arc.cost = util::straightLineCost( node.pos, arc.toNode->pos, costMap );
//             cout<<"TRACE(sparseskel.cpp): arc.cost from " << node.pos << " to " << arc.toNode->pos << ": " << arc.cost << endl;
        }
    }
}

SparseSkelNode*
ContiguousSparseSkel::findNode( const Cell2D &pos )
{
    for ( unsigned int i=0; i < nodes_.size(); i++ )
    {
        if ( pos == nodes_[i]->pos )
            return nodes_[i];
    }
    return NULL;
}

void
ContiguousSparseSkel::growNode( SparseSkelNode *fromNode,
                                Cell2DList     &ungrownCells,
                                Cell2DList     &wps,
                                Cell2DList     &skel )
{
    //cout<<"TRACE(sparseskel.cpp): growNode at " << fromNode->pos << endl;

    //
    // NOTE: growNode explores _all_ surrounding cells.
    //       findNeighbourNode greedily explores the first viable cell.
    //
    for ( int i=0; i < 8; i++ )
    {
        bool found = false;
        Cell2D neighbourNodePos;
        Cell2DVector cellsEnRoute;

        Cell2D neighbourCell = surroundCell( fromNode->pos, i );

        //cout<<" growNode from "<<fromNode->pos<<": neighbourCell " << neighbourCell;

        if ( isIncluded( wps, neighbourCell ) )
        {
            // Make sure we arc to adjacent neighbours
            found = true;
            neighbourNodePos = neighbourCell;
            cellsEnRoute.push_back( neighbourCell );

            // erase neighbourCell from skel if we haven't already
            Cell2DList::iterator i = find( skel.begin(), skel.end(), neighbourCell );
            if ( i != skel.end() )
                skel.erase( i );
            
            //cout<<" is adjacent wp" << endl;
        }
        else
        {
            if ( !isIncluded( skel, neighbourCell ) )
            {
                //cout << " is useless" << endl;
                continue;
            }

            //cout<<" is path on skel" << endl;

            // Search through the skel to find the next node in this
            // direction. findNeighbourNode deletes all visited
            // members of skel.  Visited members of skel get put in
            // cellsEnRoute.
            //
            found = findNeighbourNode( fromNode,
                                       neighbourCell,
                                       neighbourNodePos,
                                       cellsEnRoute,
                                       wps,
                                       skel );
        }

        if ( found )
        {
            assert( isIncluded( wps, neighbourNodePos ) );
            if ( !findNode( neighbourNodePos ) )
                ungrownCells.push_back( neighbourNodePos );

            //cout<<"TRACE(sparseskel.cpp): arcing from " << fromNode->pos << " to " << neighbourNodePos << endl;
            if ( !util::isTraversable( parent_.ogMap(), neighbourNodePos, traversabilityThreshhold() ) )
            {
                std::stringstream ss;
                ss << "While building sparse skel: Dense skeleton crossed non-traversable area at OG Cell " << neighbourNodePos;
                throw hydropathplan::Exception( ERROR_INFO, ss.str() );
            }
            createArc( fromNode, neighbourNodePos, cellsEnRoute );
        }
    }
}

bool
ContiguousSparseSkel::findNeighbourNode( const SparseSkelNode *fromNode,
                                         const Cell2D         &startCell,
                                         Cell2D               &neighbourNodeCell,
                                         Cell2DVector         &cellsEnRoute,
                                         Cell2DList           &wps,
                                         Cell2DList           &skel )
{
    //cout<<"TRACE(sparseskel.cpp): findNeighbourNode from " << startCell << endl;

    Cell2D currentCell = startCell;
    cellsEnRoute.resize(0);

    while ( true )
    {
        cellsEnRoute.push_back( currentCell );
        
        // Erase this cell from skel
        // NOTE: Searching through the skel every time is gonna be _real_ expensive...
        //
        Cell2DList::iterator skelIt = find( skel.begin(), skel.end(), currentCell );
        assert( skelIt != skel.end() );
        skel.erase( skelIt );

        // cout<<"TRACE(sparseskel.cpp): erased (and searching from) " << currentCell << endl;

        // keep searching.
        bool foundNextCell=false;
        for ( int i=0; i < 8; i++ )
        {
            // Look for a cell on the skel.
            Cell2D considerCell = surroundCell( currentCell, i );

            // cout<<"  considering cell " << considerCell << ": ";

            // Is considerCell the terminal wp?
            Cell2DList::iterator wpIt = find( wps.begin(), wps.end(), considerCell );
            if ( wpIt != wps.end() && (*wpIt) != fromNode->pos )
            {
                // cout<<"terminating wp." << endl;

                // Found the terminating wp!
                neighbourNodeCell = considerCell;
                
                // erase considerCell from skel if we haven't already
                Cell2DList::iterator i = find( skel.begin(), skel.end(), considerCell );
                if ( i != skel.end() )
                {
                    skel.erase( i );
                }

                return true;
            }

            // Can we keep searching this cell along the skel?
            if ( isIncluded( skel, considerCell ) )
            {
                // cout<<"next along skel." << endl;
                assert( considerCell != fromNode->pos );
                currentCell = considerCell;
                foundNextCell = true;
                break;
            }
            // cout<<"not useful." << endl;
        }

        // 
        // If we got to here, we couldn't find the next cell.  But if
        // there's no next cell, we're on the end of a line, so this
        // should be a wp?!?!?
        //
        if ( !foundNextCell )
        {
            cout<<"TRACE(sparseskel.cpp): while searching from currentCell " << currentCell << endl;
            assert( false && "Couldn't find next cell..." );
            return false;
        }
    }

    assert( false && "Shouldn't get here" );
    return false;
}

SparseSkelNode *
ContiguousSparseSkel::createArc( SparseSkelNode  *fromNode, 
                                 Cell2D          &neighbourNodePos,
                                 Cell2DVector    &cellsEnRoute )
{
//     cout<<"TRACE(sparseskel.cpp): createArc()" << endl;
//     cout<<"TRACE(sparseskel.cpp): arcing from " << fromNode->pos << " to " << neighbourNodePos << endl;

    assert( util::isTraversable( parent_.ogMap(), fromNode->pos,    traversabilityThreshhold() ) );
    assert( util::isTraversable( parent_.ogMap(), neighbourNodePos, traversabilityThreshhold() ) );
    if ( !directLinkExists( parent_.ogMap(), traversabilityThreshhold(), fromNode->pos, neighbourNodePos ) )
    {
        // Nodes can't see one-another.  Create a stepping-stone node in the middle.
        Cell2DVector segment1Cells;
        Cell2DVector segment2Cells;
        
        // If there's an odd number, make segment1 larger
        unsigned int midI = (int)(ceil(cellsEnRoute.size()/2.0));
        for ( unsigned int i=0; i < midI; i++ )
            segment1Cells.push_back( cellsEnRoute[i] );
        for ( unsigned int i=midI; i < cellsEnRoute.size(); i++ )
            segment2Cells.push_back( cellsEnRoute[i] );

        assert( segment1Cells.size() > 0 );

        SparseSkelNode *midNode = createArc( fromNode, segment1Cells.back(), segment1Cells );
        SparseSkelNode *endNode = createArc( midNode,  neighbourNodePos,     segment2Cells );
        return endNode;
    }

    float cost = -1;

    // See if this node already exists, and if so whether we're already linked.
    SparseSkelNode *toNode = findNode( neighbourNodePos );
    if ( toNode != NULL )
    {
        if ( areLinked( fromNode, toNode ) )
        {
            //cout<<"TRACE(sparseskel.cpp): createArc(): already linked." << endl;
            return toNode;
        }
        else
        {
            //cout<<"TRACE(sparseskel.cpp): createArc(): toNode exists but not linked." << endl;
        }
    }
    else
    {
        // Node doesn't exist -- create a new node
        toNode = new SparseSkelNode( neighbourNodePos );
        nodes_.push_back( toNode );
    }

    assert( !areLinked( toNode, fromNode ) );
    assert( toNode != fromNode );

    // Link the two up!
    SparseSkelArc *newFromArc = new SparseSkelArc( toNode,   cost );
    SparseSkelArc *newToArc   = new SparseSkelArc( fromNode, cost );
    fromNode->arcs.push_back( newFromArc );
    toNode->arcs.push_back( newToArc );

    assert( util::losExists( ogMap(), traversabilityThreshhold(), fromNode->pos, newFromArc->toNode->pos ) );
    assert( util::losExists( ogMap(), traversabilityThreshhold(), toNode->pos, newToArc->toNode->pos ) );

    return toNode;
}

void
ContiguousSparseSkel::tryAddExtraNode( const Cell2D &cell, 
                                       double minDistForLinkSq,
                                       double maxDistForLinkSq,
                                       const hydroogmap::OgMap &ogMap )
{
//     cout<<"TRACE(sparseskel.cpp): tryAddExtraNode( " << cell << " )" << endl;

    if ( !util::isTraversable( ogMap, cell, traversabilityThreshhold() ) )
        return;

    SparseSkelNode *newNode = NULL;
    std::vector<SparseSkelNode *> nodesToLinkTo;

    for ( size_t i=0; i < nodes_.size(); i++ )
    {
        SparseSkelNode *existingNode = nodes_[i];
        const double distSq = distanceSq( cell, existingNode->pos );

        if ( distSq <= maxDistForLinkSq &&
             directLinkExists( ogMap, traversabilityThreshhold(), cell, existingNode->pos ) )
        {
            // Another node is visible and within range...
            if ( distSq <= minDistForLinkSq )
            {
                // we're too close to another node -- don't create one here.
                return;
            }
            else
            {
                // Link to this node
                nodesToLinkTo.push_back( existingNode );
            }
        }
    }

    if ( nodesToLinkTo.size() == 0 )
        return;

    // Create a new node
    // cout<<"TRACE(sparseskel.cpp): Creating extra node at " << cell << endl;
    newNode = new SparseSkelNode( cell );
    nodes_.push_back( newNode );

    for ( size_t i=0; i < nodesToLinkTo.size(); i++ )
    {
        // cout<<"TRACE(sparseskel.cpp):   linking to node at " << nodesToLinkTo[i]->pos << endl;
        
        // Link to the existing node
        SparseSkelArc *arcToExisting = new SparseSkelArc( nodesToLinkTo[i], -1 );
        SparseSkelArc *arcToNew = new SparseSkelArc( newNode, -1 );
        newNode->arcs.push_back( arcToExisting );
        nodesToLinkTo[i]->arcs.push_back( arcToNew );
    }
}

void
ContiguousSparseSkel::addExtraNodes( const hydroogmap::OgMap &ogMap, int stepInCells )
{
    const int xMin = 0;
    const int xMax = ogMap.numCellsX();

    const int yMin = 0;
    const int yMax = ogMap.numCellsY();

    // double maxDistForLink = stepInCells*sqrt(2)*1.05;
    // Never link two cells over in a given direction, but link anything else.
    double maxDistForLink = stepInCells*1.99;
    double maxDistForLinkSq = maxDistForLink*maxDistForLink;

    double minDistForLinkSq = (maxDistForLink/2.0)*(maxDistForLink/2.0);

    for ( int x = xMin+stepInCells;
          x <= xMax-stepInCells;
          x += stepInCells )
    {
        for ( int y = yMin+stepInCells;
              y <= yMax-stepInCells;
              y += stepInCells )
        {
            tryAddExtraNode( Cell2D( x, y ), minDistForLinkSq, maxDistForLinkSq, ogMap );
        }
    }
}

SparseSkelNode::~SparseSkelNode()
{
    // cout<<"TRACE(sparseskel.cpp): ~SparseSkelNode()" << endl;
    for ( unsigned int i=0; i < arcs.size(); i++ )
    {
        delete arcs[i];
    }
}

////////////////////////////////////////////////////////////////////////////////

bool
ContiguousSparseSkel::isSane() const
{
    bool isOK = true;

#if SUPER_DEBUG
    // Loop over nodes
    for ( unsigned int i=0; i < nodes_.size(); i++ )
    {
        // NOT A VALID CHECK: a contiguous dense skel can consist of a single point.
//         // at least one arc
//         if ( nodes_[i]->arcs.size() == 0 )
//         {
//             cout<<"TRACE(sparseskel.cpp): node " << nodes_[i]->pos << " has no arcs." << endl;
//             isOK = false;
//         }

        // Loop over this node's arcs
        for ( unsigned int j=0; j < nodes_[i]->arcs.size(); j++ )
        {
            // symmetric arcs
            bool arcSymmetric = false;
            for ( unsigned int k=0; k < nodes_[i]->arcs[j]->toNode->arcs.size(); k++ )
            {
                if ( nodes_[i]->arcs[j]->toNode->arcs[k]->toNode == nodes_[i] )
                    arcSymmetric = true;
            }
            if ( !arcSymmetric )
            {
                cout<<"TRACE(sparseskel.cpp): asymetric arc between " 
                    << nodes_[i]->pos << " and " 
                    << nodes_[i]->arcs[j]->toNode->pos << endl;
                isOK = false;
            }
            
            // no repeat arcs
            for ( unsigned int k=0; k < nodes_[i]->arcs.size(); k++ )
            {
                if ( j == k ) continue;
                if ( nodes_[i]->arcs[j]->toNode == nodes_[i]->arcs[k]->toNode )
                {
                    cout<<"TRACE(sparseskel.cpp): repeat arc: " 
                        << nodes_[i]->pos << " to " 
                        << nodes_[i]->arcs[j]->toNode->pos << endl;
                    isOK = false;
                }
            }

            // LOS along arc
            if ( !util::losExists( ogMap(), traversabilityThreshhold(), nodes_[i]->pos, nodes_[i]->arcs[j]->toNode->pos ) )
            {
                cout<<"TRACE(sparseskel.cpp): no LOS along arc from:" << endl;
                cout<<"TRACE(sparseskel.cpp): " << nodes_[i]->pos << "  -->  " << nodes_[i]->arcs[j]->toNode->pos << endl;
                isOK = false;
            }
        }

        // no repeat nodes
        for ( unsigned int j=i+1; j < nodes_.size(); j++ )
        {
            if ( nodes_[i]->pos == nodes_[j]->pos )
            {
                cout<<"TRACE(sparseskel.cpp): repeat node: " << nodes_[i]->pos << endl;
                isOK = false;
            }
        }
    }
#endif

    if ( !isOK )
    {
        cout<<"TRACE(sparseskel.cpp): contiguousSparseSkel has " << nodes_.size() << " nodes." << endl;
        
    }
    return isOK;
}

}

}

