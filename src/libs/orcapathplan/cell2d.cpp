/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

/*
  \file cell2d.cpp
  \date Thu Apr 21 2005
  \author George Mathews, g.mathews@cas.edu.au
*/


//implementation file keeps these headers from getting included with cell2d.h
#include <cmath>
#include <stdlib.h>  // for abs()
#include <iostream>

#include "cell2d.h"

using namespace std; // for math functions in QNX

namespace orcapathplan {


/*****************************************************************************
 * Cell2D utility functions
 *****************************************************************************/

double
euclideanDistance( const Cell2D c1, const Cell2D c2 )
{
    return sqrt( pow(c1.x()-c2.x(),2.0) + pow(c1.y()-c2.y(),2.0) );
}

int
manhattanDistance( const Cell2D c1, const Cell2D c2 )
{
    return  ( abs( c1.x() - c2.x() ) + abs( c1.y() - c2.y() ) );

}

bool
isAdjacentN( const Cell2D c1, const Cell2D c2, unsigned int N )
{
    return ( (unsigned int)abs(c1.x()-c2.x())<=N && (unsigned int)abs(c1.y()-c2.y())<=N );
}

bool
isNull( const Cell2D c )
{
    return ( c.x()==0 && c.y()==0 );
}


Cell2D
adjacentCell( const Cell2D & cell, const int i )
{
    switch( i )
    {
        default:
        case 0:
        {
            return Cell2D( cell.x(), cell.y()+1 );
        }
        case 1:
        {
            return Cell2D( cell.x()+1, cell.y() );
        }
        case 2:
        {
            return Cell2D( cell.x(), cell.y()-1 );
        }
        case 3:
        {
            return Cell2D( cell.x()-1, cell.y() );
        }
    }
}

Cell2D
diagonalCell( const Cell2D & cell, const int i )
{
    Cell2D c(cell);
    switch( i )
    {
        default:
        case 0:
        {
            return Cell2D( cell.x()-1, cell.y()+1 );
        }
        case 1:
        {
            return Cell2D( cell.x()+1, cell.y()+1 );
        }
        case 2:
        {
            return Cell2D( cell.x()+1, cell.y()-1 );
        }
        case 3:
        {
            return Cell2D( cell.x()-1, cell.y()-1 );
        }
    }
}

Cell2D
surroundCell( const Cell2D & cell, const int i )
{
    if( i < 4 || i > 7) // contains default case
        return adjacentCell(cell,i);
    else
        return diagonalCell(cell,i-4);
}

std::ostream& operator<<( std::ostream &s, const  Cell2D &obj )
{
    s<<"("<<obj.x()<<","<<obj.y()<<")";
    
    return s;
}

}

