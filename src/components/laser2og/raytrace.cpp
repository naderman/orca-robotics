/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alexei Makarenko, George Mathews, Matthew Ridley
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "raytrace.h"
#include <orca/bros1.h>

#include <orcapathplan/orcapathplan.h>
#include <orcaogmap/ogmap.h>
#include <orcaiceutil/mathdefs.h>

using namespace orca;
using namespace orcaogmap;
using namespace orcapathplan;
using namespace std;

//
// Uses Bresenham's line algorithm
//

namespace details
{

    void rayTraceOct0( const orcapathplan::Cell2D & c0,
		      const orcapathplan::Cell2D & c1,
		      std::vector<Cell2D> & retCellVect )
    {
        int y = c0.y(); 
        int dx = 2*(c1.x()-c0.x());
        int dy = 2*(c1.y()-c0.y()); 
        int dydx = dy-dx; 
        int F = (dy-dx)/2; 

        for (int x=c0.x(); x<=c1.x(); x++) 
        { 
            retCellVect.push_back( Cell2D(x,y) );
            if ( F<0 ) 
            {
                F += dy; 
            }
            else 
            {
                y++; 
                F += dydx;
            } 
        }
    }

    void rayTraceOct1( const orcapathplan::Cell2D & c0,
		      const orcapathplan::Cell2D & c1,
		      std::vector<Cell2D> & retCellVect  )
    {
        int x = c0.x(); 
        int dx = 2*(c1.x()-c0.x());
        int dy = 2*(c1.y()-c0.y()); 
        int dxdy = dx-dy; 
        int F = (dx-dy)/2; 

        for (int y=c0.y(); y<=c1.y(); y++) 
        { 
            retCellVect.push_back( Cell2D(x,y) );
            if ( F<0 ) 
            {
                F += dx; 
            }
            else 
            {
                x++; 
                F += dxdy;
            } 
        }
    }

    void rayTraceOct2( const orcapathplan::Cell2D & c0,
		      const orcapathplan::Cell2D & c1,
		      std::vector<Cell2D> & retCellVect  )
    {
        int x = c0.x(); 
        int dx = -2*(c1.x()-c0.x());
        int dy = 2*(c1.y()-c0.y()); 
        int dxdy = dx-dy; 
        int F = (dx-dy)/2; 

        for (int y=c0.y(); y<=c1.y(); y++) 
        { 
            retCellVect.push_back( Cell2D(x,y) );
            if ( F<0 ) 
            {
                F += dx; 
            }
            else 
            {
                x--; 
                F += dxdy;
            } 
        }
    }

    void rayTraceOct7( const orcapathplan::Cell2D & c0,
		      const orcapathplan::Cell2D & c1,
		      std::vector<Cell2D> & retCellVect  )
    {
        int y = c0.y(); 
        int dx = 2*(c1.x()-c0.x());
        int dy = -2*(c1.y()-c0.y());
        int dydx = dy-dx; 
        int F = (dy-dx)/2; 

        for (int x=c0.x(); x<=c1.x(); x++) 
        { 
            retCellVect.push_back( Cell2D(x,y) );
            if ( F<0 ) 
            {
                F += dy; 
            }
            else 
            {
                y--; 
                F += dydx;
            } 
        }
    }
    
}

std::vector<Cell2D>
laser2og::rayTrace(const orcapathplan::Cell2D & c0,
		   const orcapathplan::Cell2D & c1 )
{
    /*
    *  Octants are defined as follows:
    *
    *   \2|1/ 
    *   3\|/0
    *   --x--
    *   4/|\7
    *   /5|6\ 
    *
    */
    
    
    std::vector<Cell2D> retCellVect;
    
    if( c0 == c1 )
    {
        retCellVect.push_back(c0);
        return retCellVect;
    }
    
    float slope;
    if ( c0.x() == c1.x() )
        slope = 999999;
    else
        slope = (float)(c1.y() - c0.y()) / (float)(c1.x() - c0.x());
    
    if ( slope >= 0.0 && slope <= 1.0 )
    {
        if ( c1.x() > c0.x() ) //(x1 > x0 )
        {
            // octant 0
            details::rayTraceOct0( c0, c1, retCellVect );
        }
        else
        {
            // octant 4
            details::rayTraceOct0( c1, c0, retCellVect );
        }
    }
    else if ( slope > 1.0 ) //(& = -inf)
    {
        if ( c1.y() > c0.y() ) //( x1 > x0 )
        {
            // octant 1
            details::rayTraceOct1(  c0, c1, retCellVect  );
        }
        else
        {
            // octant 5
            details::rayTraceOct1( c1, c0, retCellVect );
        }
    }
    else if ( slope < -1.0 )
    {
        if ( c1.y() > c0.y() ) //( y1 > y0 )
        {
            // octant 2
            details::rayTraceOct2( c0, c1, retCellVect  );
        }
        else
        {
            // octant 6
            details::rayTraceOct2( c1, c0, retCellVect );
        }
    }
    else // ( slope < 0.0 && slope >= -1.0 )
    {
        if ( c1.x() > c0.x() )//( x1 > x0 )
        {
            // octant 7
            details::rayTraceOct7(  c0, c1, retCellVect  );
        }
        else
        {
            // octant 3
            details::rayTraceOct7(  c1, c0, retCellVect  );
        }
    }
    
    // check ordering or cell
    if( retCellVect.size() > 1 )
        if( c0 != retCellVect.front() )
            reverse( retCellVect.begin(), retCellVect.end() );
    
//     if( retCellVect.size() > 1 )
//         if( c0 != retCellVect.front() )
//             cout<<"error"<<endl;
    
    return retCellVect;
}


std::vector<Cell2D>
laser2og::rayTrace( const orcaogmap::CartesianPoint2d & p1, const orcaogmap::CartesianPoint2d & p2,
                              const double originX, const double originY,
                              const double mapResX, const double mapResY ) 
{
    Cell2D c1 = point2cell(p1, originX, originY, mapResX, mapResY);
    Cell2D c2 = point2cell(p2, originX, originY, mapResX, mapResY);
    
    return rayTrace(c1,c2);
}


