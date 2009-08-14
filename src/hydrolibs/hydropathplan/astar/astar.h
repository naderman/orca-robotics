/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
//
// This class was originally written by Pham Ngoc Hai (h dot pham at acfr.usyd.edu.au).
//

#ifndef ORCA2_ASTAR_H
#define ORCA2_ASTAR_H

#include <deque>

namespace hydropathplan
{

namespace astar {

class AStarNode {
public:
	AStarNode(int, int, double);			// cell's postion (i,j), and weight
	~AStarNode();

	double			m_NodeWeight;		// The weight value of the cell. This is used to calculate 
										// the moving cost to move to this cell from its adjacent node
										// cost = m_NodeWeight*ASTAR_CELLSIZE (if orthogonal nodes)
										// cost = sqrt(2)*m_NodeWeight*ASTAR_CELLSIZE (if diagonal nodes)

										
	double			f,g,h;				// total cost, cost from start to note, heuristic to goal.
	int				cell_i, cell_j;		// Cell(Node) position (i,j) in the matrix
										// Note: In C/C++, the first index starts from 0
										// then cell_i = 0,1,... and cell_i = 0,1,...
										// In Matlab: the index stars from 1 => need to convert from Matlab

	int				m_NodeID;			// Unique Identification of each node (the index in mattrix)
										// the 2x2 MAP(num_rows, num_cols) in C/C++ can be seen as an 1 dimentional
										// array with mxn elements, then m_NodeID can be:
										// m_NodeID = cell_j * m_iAStarMapRows + cell_i = cell_j*ASTAR_GRIDSIZE_H + cell_i;
	
	int				m_NumOfChildren;

	AStarNode		*m_pParent;
	AStarNode		*m_pNext;			//
	AStarNode		*m_pChildren[8];		// List of Neighbor around a node (8 neighbors)

};

//Tobias: my addition
typedef std::deque<AStarNode*> AStarNodeDeque;

class AStar {
public:
	AStar(double *map, int map_rows, int map_cols, double map_cellsize, bool map_AllowDiagonal, double map_ObstacleWeight);
	~AStar();

	bool			run();				// Run A* to find the shortest path
	void			RUN_Initialize();	// Run A* -> the 1st step: initialize
	int				RUN_Step();			// Run A* step by step

//protected:
	bool			m_bAllowDiagonal;	// Permit to go Digonal or not

	double			m_CellSize;			// The size of each cell (-> calculate distance)
	double			m_ObstacleWeight;	// Obstacle's weight -> to check whether or not a cell is obstacle.

	double			*m_pAStarMAP;		//The A* Map stored in 2x2 matrix of doubles
										// this map stores the weight of each cell
										// e.x weight = 1 if free zone
										// weight = ASTAR_OBSTACLE_COST if Obstacle
	int				m_iAStarMapRows;	//Number of Rows of A* Map
	int				m_iAStarMapCols;	//Number of Columns of A* Map

	int				m_iStartNodeID;		// the ID (index of 1-dimention matrix) of the Start Node
	int				m_iStartNode_i;		// the cell_i (index i) of the Start Node (in C/C++ matrix)
	int				m_iStartNode_j;		// the cell_j (index j) of the Start Node (in C/C++ matrix)

	int				m_iGoalNodeID;		// the ID (index of 1-dimention matrix) of the Goal Node
	int				m_iGoalNode_i;		// the cell_i (index i) of the Goal Node (in C/C++ matrix)
	int				m_iGoalNode_j;		// the cell_j (index j) of the Goal Node (in C/C++ matrix)

	// Nodes Lists
	AStarNode		*m_pBestNode;
	AStarNode		*m_pOpenList;
	AStarNode		*m_pClosedList;

	// Class Methods
	void			AddToOpenList(AStarNode *);
	AStarNode		*GetBestNode();
	AStarNode		*IsNodeInList(AStarNode *, int);
	void			ExpandChildNodes(AStarNode *);
	void			LinkChildNodes(AStarNode *, AStarNode *);

	void			ClearNodes();


	// Check whether can go from node to neighbor, basing on:
		// neighbor may be an Obstacle, 
		// or neighbor is located out of the limitation of the map
		// or permiting Diagonal route from node to neighbor or not.
	int Check_ASNodeValid(AStarNode *node, AStarNode *neighbor);


	// Calculate the moving cost from node to its adjacent node (1 of 8 adjacent nodes)
	double Cost_2AdjacentNodes(AStarNode *node, AStarNode *neighbor); 

	// Calculate Mahattan heuristic h from node1 to node2 (Orthogonal moving only)
	// D is the minimum moving cost between a node and its adjacent (D is changable...)
	double ManhattanHeuristic(int node1_i, int node1_j, int node2_i, int node2_j, double D);

	// Calculate Diagonal heuristic h from node1 to node2 (Allow Diagonal+Orthogonal moving)
	double DiagonalHeuristic(int node1_i, int node1_j, int node2_i, int node2_j, double D);

	// Inline functions.
	// This function convert the index of 2-dimentions matrix to the index in the coressponding
	// 1-dimention matrix in C/C++ (index starts from 0, other than 1 in Matlab)
	inline int GetNodeID(int cell_i, int cell_j) { return cell_j * m_iAStarMapRows + cell_i; }
};

}

}

#endif
