#include "genericmap.h"
#include <iostream>
#include <sstream>

using namespace std;

namespace hydroogmap {

std::string toString(const WorldCoords &w)
{
    std::stringstream ss;
    ss << "["<<w.x<<","<<w.y<<"]";
    return ss.str();
}
std::string toString(const GridIndices &g)
{
    std::stringstream ss;
    ss << "["<<g.x<<","<<g.y<<"]";
    return ss.str();
}
std::string toString(const Frame2d &f)
{
    std::stringstream ss;
    ss << "["<<f.p.x<<","<<f.p.y<<","<<f.o*180.0/M_PI<<"deg]";
    return ss.str();
}
bool operator==( const Frame2d &f1, const Frame2d &f2 )
{
    return ( f1.p.x == f2.p.x &&
             f1.p.y == f2.p.y &&
             f1.o   == f2.o );
}
std::string toString( const MetaData &m )
{
    std::stringstream ss;
    ss << "\toffset:        [" << m.offset << "]\n"
       << "\tnumCells:      [" << m.numCellsX << ", " << m.numCellsY << "]\n"
       << "\tmetresPerCell: [" << m.metresPerCell << "]";
    return ss.str();
}
bool operator==( const MetaData &m1, const MetaData &m2 )
{
    const double EPS = 1e-5;
    return( fabs( m1.metresPerCell - m2.metresPerCell ) < EPS &&
            fabs( m1.offset.p.x    - m2.offset.p.x ) < EPS &&
            fabs( m1.offset.p.y    - m2.offset.p.y ) < EPS &&
            fabs( m1.offset.o      - m2.offset.o ) < EPS &&
            m1.numCellsX == m2.numCellsX &&
            m1.numCellsY == m2.numCellsY );    
}

}

