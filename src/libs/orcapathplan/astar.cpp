/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
//
// This class was originally written by Pham Ngoc Hai (h dot pham at acfr.usyd.edu.au).
//

#include <stdlib.h>
#include <cmath>
#include <iostream>

#include "astar.h"

using namespace std;

namespace orcapathplan
{

/*******************************/
AStarNode::AStarNode(int a, int b, double weight){
	m_NodeWeight = weight;
	cell_i = a;
	cell_j = b;

	g = 0.0; h = 0.0; f = 0.0;

	m_NumOfChildren = 0;

	m_pParent = NULL;
	m_pNext = NULL;

	memset(m_pChildren, 0, sizeof(m_pChildren));
		
}

AStarNode::~AStarNode() {
	/*if (m_pParent) delete m_pParent;
	if (m_pNext) delete m_pNext;
	if (m_pDataPtr) delete m_pDataPtr;*/
}
/*******************************/



/*******************************/
AStar::AStar(double *map, int map_rows, int map_cols, double map_cellsize, bool map_AllowDiagonal, double map_ObstacleWeight) {
	//AStar::AStar() {
	m_pAStarMAP = map;

	m_bAllowDiagonal = map_AllowDiagonal;	// = ASTAR_ALLOW_DIAGONAL

	m_ObstacleWeight = map_ObstacleWeight;	// = ASTAR_OBSTACLE_COST
	m_CellSize = map_cellsize;				// = ASTAR_CELLSIZE
	m_iAStarMapRows = map_rows;
	m_iAStarMapCols = map_cols;

	m_pBestNode = NULL;
	m_pOpenList = NULL;
	m_pClosedList = NULL; 
}

AStar::~AStar() {
	ClearNodes();
}


int	AStar::Check_ASNodeValid(AStarNode *node, AStarNode *neighbor) {

	/* Avoid neighbor outside the MAP 
		-> check the first index (1 or 0) in Matlab, C/C++ */
	if (neighbor->cell_i < 0 || neighbor->cell_j < 0 || neighbor->cell_i >= m_iAStarMapRows || neighbor->cell_j >= m_iAStarMapCols) 
		return false;

	// Node neighbor is an Obstacle -> Not to go through
    //Tobias: bigger than rather than equal
    if (neighbor->m_NodeWeight >= m_ObstacleWeight) return false;
// 	if (neighbor->m_NodeWeight == m_ObstacleWeight) return false; 

	if (!m_bAllowDiagonal) { // If neighbor is not Diagonal to node
		if (node->cell_i - neighbor->cell_i != 0 && node->cell_j - neighbor->cell_j != 0) 
			return false;
	}

	return true;
}

double AStar::Cost_2AdjacentNodes(AStarNode *node, AStarNode *neighbor) {
	double D = m_CellSize;		// the minimum moving cost between node and its adjacent (neighbor)
							// this value of D can be changable depended on specified demand.

	// If neighbor is diagonal to node
	if ((node->cell_i != neighbor->cell_i) && (node->cell_j != neighbor->cell_j)) 
	{ 
		return int((sqrt(2.0) * D * neighbor->m_NodeWeight));
	}
	else return (D * neighbor->m_NodeWeight); // If neighbor is orthogonal to node
}

double AStar::ManhattanHeuristic(int node1_i, int node1_j, int node2_i, int node2_j, double D) {	
	double h = D*(abs(node2_i - node1_i) + abs(node2_j - node1_j));
	return h;
}

double AStar::DiagonalHeuristic(int node1_i, int node1_j, int node2_i, int node2_j, double D) {
	int h1 = abs(node2_i - node1_i) + abs(node2_j - node1_j);
	int h2 = (abs(node2_i - node1_i) < abs(node2_j - node1_j)) ? abs(node2_i - node1_i) : abs(node2_j - node1_j);
    
    // Tobias
    //int D2 = sqrt(2.0))*D;
  
    double h = D*(h1 - 2.0*h2) +  sqrt(2.0)*D*h2;
	return h;
}

void AStar::ClearNodes() {
	AStarNode *temp = NULL;

	if (m_pOpenList) {
		while (m_pOpenList) {
			temp = m_pOpenList->m_pNext;

			delete m_pOpenList;

			m_pOpenList = temp;
		}
	}

	if (m_pClosedList) {
		while (m_pClosedList) {
			temp = m_pClosedList->m_pNext;

			delete m_pClosedList;

			m_pClosedList = temp;
		}
	}
}


bool AStar::run() {
    cout << "INFO(astar.cpp): run..." << endl;
	RUN_Initialize();

	int retval = 0;	
	while (retval == 0) {
		retval = RUN_Step();
	};

	if (retval == -1 || !m_pBestNode) {
		m_pBestNode = NULL;
        std::cout << "ERROR(astar.cpp): Path could not be found" << endl;
        return false;
	}
    cout << "INFO(astar.cpp): Path was successfully computed" << endl;

	// A Route is successfully FOUND!
//     cout << "m_pBestNode = GOAL: cell_i = " << m_pBestNode->cell_i << endl;
//     cout << "m_pBestNode = GOAL: cell_i = " << m_pBestNode->cell_j << endl;
//     cout << "m_pBestNode = GOAL: g = " << m_pBestNode->g << endl;
	return true;
}

void AStar::RUN_Initialize() {
	// Initialize the map (step 1 of A* algorithm)
	ClearNodes();

	m_iGoalNodeID = GetNodeID(m_iGoalNode_i, m_iGoalNode_j);
	m_iStartNodeID = GetNodeID(m_iStartNode_i, m_iStartNode_j);	

	int startnode_weight = (int)*(m_pAStarMAP + m_iStartNodeID);
	AStarNode *temp = new AStarNode(m_iStartNode_i, m_iStartNode_j, startnode_weight);
	temp->g = 0;

	if (!m_bAllowDiagonal)
		// Heusistic by Manhattan Method
		temp->h = ManhattanHeuristic(m_iStartNode_i, m_iStartNode_j, m_iGoalNode_i, m_iGoalNode_j, m_CellSize);	
	else
		// Heusistic by Diagonal Method
		temp->h = DiagonalHeuristic(m_iStartNode_i, m_iStartNode_j, m_iGoalNode_i, m_iGoalNode_j, m_CellSize);
	
	temp->f = temp->g + temp->h;
	temp->m_NodeID = m_iStartNodeID;
	m_pOpenList = temp;
}


int AStar::RUN_Step() {
	if (!(m_pBestNode = GetBestNode()))
		return -1;

	if (m_pBestNode->m_NodeID == m_iGoalNodeID) {
		// Successfully find a shortest path b/w the Start and Goal nodes
		return 1;
	}

	// otherwise, expand the current node (m_pBestNode - the Top of m_pOpenList)
	ExpandChildNodes(m_pBestNode);

	return 0;
}


void AStar::ExpandChildNodes(AStarNode *node) {
	// Create 8 child nodes of the current node: AStarNode *node
	// assign each node's values such as: coordinates, f, g, and h
	// and Add the appropriate children to m_pOpenList (A* 's steps 5)

	int i, j;
	
	AStarNode neighbor = AStarNode(0, 0, 0);

	for (i=-1;i<2;i++) { // SHOUD develop for the case the node has < 8 around cells
		for (j=-1;j<2;j++) {
			neighbor.cell_i = node->cell_i+i;
			neighbor.cell_j = node->cell_j+j;

			neighbor.m_NodeID = GetNodeID(neighbor.cell_i, neighbor.cell_j);
			neighbor.m_NodeWeight = (int)*(m_pAStarMAP + neighbor.m_NodeID);

			if (((i == 0) && (j == 0)) || !Check_ASNodeValid(node, &neighbor))
				continue;
			
			LinkChildNodes(node, &neighbor); //Link neighbor to node					
		}
	}
}

void AStar::LinkChildNodes(AStarNode *node, AStarNode *neighbor) {
	int cell_i = neighbor->cell_i;
	int cell_j = neighbor->cell_j;
	int neighbor_ID = neighbor->m_NodeID;
	
	// The cost to move from node to neighbor (the distance bw them)
	double move_cost = Cost_2AdjacentNodes(node, neighbor);

	double g = node->g + move_cost;
		
	AStarNode *check_node = NULL;
    
//
//WARNING: Tobias: changed the following completely
//
    check_node = IsNodeInList(m_pOpenList, neighbor_ID);
    if (check_node != NULL) {
		node->m_pChildren[node->m_NumOfChildren++] = check_node;

		// Check if a new better path found -> update
		if (g < check_node->g) {
			check_node->m_pParent = node;
			check_node->g = g;
			check_node->f = g + check_node->h;
		}
    } else {
        check_node = IsNodeInList(m_pClosedList, neighbor_ID);    
        if ( check_node!= NULL ) {
            node->m_pChildren[node->m_NumOfChildren++] = check_node;
    
            if (g < check_node->g) {
                check_node->m_pParent = node;
                check_node->g = g;
                check_node->f = g + check_node->h;
            }
        } else {
            /*the newnode is neither in m_pOpenList nor m_pClosedList -> Add to the m_pOpenList*/
            AStarNode *newnode = new AStarNode(cell_i, cell_j, neighbor->m_NodeWeight);
            newnode->m_pParent = node;
            newnode->g = g;
        
            if (!m_bAllowDiagonal)
            // Manhattan heuristic
                newnode->h = ManhattanHeuristic(cell_i, cell_j, m_iGoalNode_i, m_iGoalNode_j, m_CellSize);
            else
            // Heusistic by Diagonal Method
                newnode->h = DiagonalHeuristic(cell_i, cell_j, m_iGoalNode_i, m_iGoalNode_j, m_CellSize);

            newnode->f = newnode->g + newnode->h;
            newnode->m_NodeID = neighbor_ID;

            AddToOpenList(newnode);
        
            node->m_pChildren[node->m_NumOfChildren++] = newnode;
        }   
    }

}

void AStar::AddToOpenList(AStarNode *newnode) {
	AStarNode *node = m_pOpenList;
	AStarNode *prev = NULL;

	if (m_pOpenList == NULL) {
		// m_pOpenList is empty
		m_pOpenList = newnode;
		m_pOpenList->m_pNext = NULL;

		return;
	}

	while(node) { // (node == m_pOpenList)
		// Try to sort the m_pOpenList increasingly (the best node - lowest f will be in the TOP)
		if (newnode->f > node->f) {
			prev = node;
			node = node->m_pNext;
		} else {
			if (prev) {
				prev->m_pNext = newnode;
				newnode->m_pNext = node;
			} else {
				AStarNode *temp = m_pOpenList;
				
				// Add the newnode in the top of m_pOpenList
				m_pOpenList = newnode;
				m_pOpenList->m_pNext = temp;
			}

			return;
		}
	}
	prev->m_pNext = newnode;
}


AStarNode *AStar::GetBestNode(){
	if (!m_pOpenList) return NULL;

	AStarNode *temp1 = m_pOpenList, *temp2 = m_pClosedList;

	// Get the best node from m_pOpenList, drop it out, and then add to m_pClosedList
	// the m_pOpenList should be sorted increasely with the best in the top
	m_pOpenList = temp1->m_pNext;

	// Add the picked node from m_pOpenList to the top of m_pClosedList
	m_pClosedList = temp1;
	m_pClosedList->m_pNext = temp2;

	return temp1;
}



AStarNode *AStar::IsNodeInList(AStarNode *node, int nodeID) {
	while (node) {
		if (node->m_NodeID == nodeID) return node;

		node = node->m_pNext;
	}

	return NULL;
}

}
