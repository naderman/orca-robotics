#include "solve.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <hydropathplan/hydropathplan.h>

using namespace std;

namespace hydropathplan {
namespace sparseskel {

namespace {

// Just for debugging.
const float M_PER_CELL=0.04;

std::string
toWorldString( const Cell2D &c )
{
    std::stringstream ss;
    ss << "("<<c.x()*M_PER_CELL+M_PER_CELL/2.0<<","<<c.y()*M_PER_CELL+M_PER_CELL/2.0<<")";
    return ss.str();
}

void
insertInOrder( std::list<const SparseSkelNode*> &l, 
               const SparseSkelNode             *n)
{
    //cout<<"TRACE(sparseskel.cpp): insertInOrder: costs prior: " << endl;
    //for ( std::list<const SparseSkelNode*>::const_iterator it=l.begin(); it != l.end(); it++ )
    //{
    //    cout << " " << (*it)->nodeCost*M_PER_CELL;
    //}
    //cout << endl;
    
    if ( l.size() == 0 )
    {
        l.push_back( n );
        return;
    }

    std::list<const SparseSkelNode*>::iterator it=l.begin();
    while ( (it != l.end()) && ((*it)->nodeCost < n->nodeCost) )
        it++;

    // at this point (*it)->nodeCost >= n->nodeCost
    l.insert( it, n ); // insert n just before (*it).

#ifndef NDEBUG
    float val  = -1;
    bool isBad = false;
    for ( std::list<const SparseSkelNode*>::const_iterator it=l.begin(); it != l.end(); it++ )
    {
        if ( (*it)->nodeCost < val )
            isBad = true;
        val = (*it)->nodeCost;
    }
    if ( isBad )
    {
        cout<<"TRACE(sparseskel.cpp): insertInOrder screwed up.  costs are: " << endl;

        for ( std::list<const SparseSkelNode*>::const_iterator it=l.begin(); it != l.end(); it++ )
        {
            cout << " " << (*it)->nodeCost*M_PER_CELL;
        }
        cout << endl;

        assert( false && "insertInOrder screwed up." );
    }
#endif

    return;
}

void setCosts( const ContiguousSparseSkel   &cSkel,
               const SparseSkelNode         *start,
               const SparseSkelNode         *goal,
               const std::vector<SparseSkelNode*> &nodes )
{
    // initialise
    for ( unsigned int i=0; i < nodes.size(); i++ )
        nodes[i]->nodeCost = NAN;

    // Unexpanded is kept ordered from low cost to high cost
    std::list<const SparseSkelNode*> unexpanded;
    
    unexpanded.push_back( start );
    start->nodeCost = 0.0;
    
    if ( start==goal ) return;

    const SparseSkelNode *current;
    while ( unexpanded.size() > 0 )
    {
        current = unexpanded.front();
        unexpanded.pop_front();
        //cout<<"TRACE(sparseskel.cpp): popped " << current->nodeCost*M_PER_CELL << endl;

        // for each arc leaving current
        for ( unsigned int i=0; i < current->arcs.size(); i++ )
        {
            SparseSkelNode *toNode = current->arcs[i]->toNode;
            float costToNode = current->nodeCost + current->arcs[i]->cost;
            if ( isnan( toNode->nodeCost ) )
            {
                toNode->nodeCost = costToNode;
                if ( toNode == goal )
                {
                    return;
                }
                else
                {
                    //cout<<"TRACE(sparseskel.cpp): adding " << toWorldString(toNode->pos) << ", cost " << toNode->nodeCost*M_PER_CELL << endl;
                    insertInOrder( unexpanded, toNode );
                }
            }
        }

//         cout<<"TRACE(sparseskel.cpp): loop end: unexpanded size is " << unexpanded.size() << endl;
//         cout<<"TRACE(sparseskel.cpp): --------------------------------" << endl;
    }
    // Shouldn't get here.
    throw hydropathplan::Exception( ERROR_INFO, "Couldn't find a path to the goal." );
}

const SparseSkelNode *
minCostNode( const SparseSkelNode *node )
{
    float minCost = node->nodeCost;
    SparseSkelNode *cheapest = NULL;
    for ( unsigned int i=0; i < node->arcs.size(); i++ )
    {
        SparseSkelNode *possibleNode = node->arcs[i]->toNode;
        //cout<<"TRACE(sparseskel.cpp): minCostNode: possibly " 
        //    << toWorldString(possibleNode->pos) << "(" << possibleNode->nodeCost << ")" << endl;
        
        if ( isnan(possibleNode->nodeCost) )
            continue;
        if ( possibleNode->nodeCost < minCost )
        {
            minCost  = possibleNode->nodeCost;
            cheapest = possibleNode;
        }
    }

    if ( cheapest == NULL )
        throw hydropathplan::Exception( ERROR_INFO, "Couldn't generate reverse plan from goal back to start!" );
    return cheapest;
}

}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void findShortestPath( const ContiguousSparseSkel        &cSkel,
                       const SparseSkelNode              *start,
                       const SparseSkelNode              *goal,
                       std::vector<const SparseSkelNode *> &path )
{
#ifndef NDEBUG
    //  find start and goal
    int startI = -1;
    int goalI  = -1;
    for ( unsigned int i=0; i < cSkel.nodes().size(); i++ )
    {
        if ( cSkel.nodes()[i] == start ) startI = i;
        if ( cSkel.nodes()[i] == goal )  goalI  = i;
    }
    assert( startI!=-1 && goalI!=-1 );
#endif

    setCosts( cSkel,
              start,
              goal,
              cSkel.nodes() );

//     cout<<"TRACE(sparseskel.cpp): ======= costs =======" << endl;
//     cout<<"TRACE(sparseskel.cpp): goal at: " << toWorldString(goal->pos) << endl;
//     for ( unsigned int i=0; i < cSkel.nodes().size(); i++ )
//     {
//        cout << "  node at " << toWorldString(cSkel.nodes()[i]->pos) << ": " << cSkel.nodes()[i]->nodeCost*M_PER_CELL<<endl;
//        for ( unsigned int j=0; j < cSkel.nodes()[i]->arcs.size(); j++ )
//        {
//            cout << "    --> " << toWorldString(cSkel.nodes()[i]->arcs[j]->toNode->pos) << "[" << cSkel.nodes()[i]->arcs[j]->toNode->nodeCost*M_PER_CELL << "]" << endl;
//        }
//     }

    // now search backwards along the lowest-cost path
    path.resize(0);

    //cout<<"TRACE(sparseskel.cpp): searching backwards..." << endl;

    const SparseSkelNode *currentNode = goal;
    assert( !isnan( goal->nodeCost ) );
    while ( true )
    {
        path.push_back( currentNode );
        if ( currentNode == start ) break;

//         cout<<"  " << currentNode->pos << "(cost="<<currentNode->nodeCost << ") -->" <<endl;

        // select a new node
        const SparseSkelNode *nextNode = minCostNode( currentNode );
        assert( path.size() <= cSkel.nodes().size() );

        currentNode = nextNode;
    }

    // We put them on in the wrong order.
    std::reverse( path.begin(), path.end() );

    //cout<<"TRACE(sparseskel.cpp): findShortestPath() returning." << endl;
}

}
}
