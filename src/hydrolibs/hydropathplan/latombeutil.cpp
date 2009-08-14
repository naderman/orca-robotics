/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "latombeutil.h"
#include "ipathplanner2d.h"
#include <gbxutilacfr/mathdefs.h>
#include <hydroutil/cpustopwatch.h>
#include "costcalculator.h"
#include <sstream>
#include <algorithm>

using namespace std;
using namespace hydroogmap;


namespace hydropathplan {
namespace latombeutil {

namespace {

    // Returns the cell with the lowest value for all 8 cells surrounding c
    Cell2D min8Cells( const FloatMap & floatMap, const Cell2D & c )
    {
        double qval;
        Cell2D q;

        Cell2D cmin = c;
        double cval = element(floatMap,  c );

        for ( int k=0; k<8; k++ )
        {
            q = surroundCell(c,k);
            if( floatMap.isInGrid( q ) )
            {
                qval = element(floatMap,  q );
                if( qval < cval && !isnan(qval) )
                {
                    cmin = q;
                    cval = qval;
                }
            }
        }
        return cmin;
    }

    // Returns the cell with the highes value for the 4 adjacent cells around c
    Cell2D max4Cells( const FloatMap & floatMap, const Cell2D & c )
    {
        Cell2D q,cmax;
        float qval,cval;
    
        //start with one cell
        cmax = adjacentCell(c,0);
        cval = element(floatMap,  cmax );
    
        // and check the rest
        for ( int i=1; i<4; i++ )
        {
            q = adjacentCell(c,i);
            qval = element(floatMap,  q );
        
            if( qval > cval )
            {
                cmax = q;
                cval = qval;
            }
        }
        return cmax;
    }

    // Returns the cell with the highest value for all 8 cells surrounding c
    Cell2D max8Cells( const FloatMap & floatMap, const Cell2D & c )
    {
        Cell2D q;
        float qval;
    
        //start with one cell
        Cell2D cmax = surroundCell(c,0);
        float cval = element(floatMap,  cmax );

        // and check the rest
        for ( int i=1; i<8; i++ )
        {
            q = surroundCell(c,i);
            qval = element(floatMap,  q );
            if( qval > cval )
            {
                cmax = q;
                cval = qval;
            }
        }
        return cmax;
    }
}

bool areAllNans( const FloatMap &floatMap )
{
    for (unsigned int x=0; x<floatMap.sizeX(); x++ )
    {
        for (unsigned int y=0; y<floatMap.sizeY(); y++ )
        {
            // if we find a single element that is not nan we return false
            if ( !isnan(element(floatMap,x,y)) ) return false;
        }
    }
    // if we get here all elements are nan
    return true;
}

int sub2ind( const int          &indX,
             const int          &indY,
             const unsigned int &sizeX,
             const unsigned int &sizeY )
{
    assert( indX >= 0         &&
            indX < (int)sizeX && 
            indY >= 0         &&
            indY < (int)sizeY );

    return indY * sizeX + indX;
}

int sub2ind( const Cell2D       &cell,
             const unsigned int &sizeX,
             const unsigned int &sizeY )
{
    return sub2ind( cell.x(), cell.y(), sizeX, sizeY );
}
    
Cell2D ind2sub( const int          &index,
                const unsigned int &sizeX,
                const unsigned int &sizeY )
{
    int x = index % sizeX;
    int y = index / sizeX;

    assert ( x >= 0 && x < (int)sizeX );
    assert ( y >= 0 && y < (int)sizeY );
    return Cell2D( x, y );
}

bool isTraversable( const OgMap &ogMap,
                    const int indX,
                    const int indY,
                    const float traversabilityThreshhold)
{
    unsigned char val;
    if( ogMap.tryGridCell( indX, indY, val ) == true )
    {
        return ( val < (unsigned char) ((traversabilityThreshhold)*hydroogmap::CELL_OCCUPIED) );
    }
    return false;   
}

bool isTraversableNeighbors( const hydroogmap::OgMap &ogMap,
                             const int indX,
                             const int indY,
                             const float traversabilityThreshhold )
{
    return ( isTraversable(ogMap,indX+1,indY,traversabilityThreshhold) || 
             isTraversable(ogMap,indX-1,indY,traversabilityThreshhold) ||
             isTraversable(ogMap,indX,indY+1,traversabilityThreshhold) || 
             isTraversable(ogMap,indX,indY-1,traversabilityThreshhold) );
}

// Checks whether cell is in a free space region.
// If 'checkForNan' is set, 'nan' means non-free.  Else '0' means non-free.
// If not, attempts to move it into one of the numCells surrounding cells. 
// If successful return TRUE, otherwise FALSE.
bool validateCell( const FloatMap & theMap, Cell2D & c, int numCells, bool checkForNan )
{
    {
        bool isFree;
        if ( checkForNan ) 
            isFree = !latombeutil::containsNan( theMap, c );
        else 
            isFree = element( theMap, c ) != 0;
        if( isFree ) return true;

        cout<<"TRACE(pathplanutils.cpp): validateCell(): !isFree." << endl;
        cout<<"TRACE(pathplanutils.cpp): element(theMap,c): " << element(theMap,c) << endl;

    }

    //cout<< "validateCell: ("<<c.x()<<","<<c.y()<<") is inside or too close to obstacle, trying too move it out..." << endl;

    Cell2DVector Lcurr,Lnext;
    Cell2D q, q1;

    Lcurr.push_back(c);

    for( int i=0; i<numCells; i++ )
    {
        while ( !Lcurr.empty() )
        {
            q = Lcurr.back();
            Lcurr.pop_back();
            for ( int k=0; k<4; k++ )
            {
                q1 = adjacentCell(q, k);

                bool isFree;
                if ( checkForNan ) isFree = !latombeutil::containsNan( theMap, c );
                else isFree = element( theMap, c ) != 0;

                if ( !isFree )
                {
                    Lnext.push_back( q1 );
                }
                else
                {
                    c=q1;
                    //cout<< "cell moved to: ("<<c.x()<<","<<c.y()<<")"<<endl;
                    return true;
                }
            }
        }
        Lcurr.swap(Lnext); // Lcurr was empty
    }
    return false;
}

// Returns true if the cell is at the border of the ogMap otherwise false
bool isBorderCell( const hydroogmap::OgMap & ogMap, int i, int j )
{
    int sizeX = ogMap.numCellsX();
    int sizeY = ogMap.numCellsY();
        
    if ( (i==0) || (i==sizeX-1) || (j==0) || (j==sizeY-1) ) return true;
     
    return false;
}

bool calcSkeletonNavigation( const hydroogmap::OgMap &ogMap,
                             FloatMap               &navMap,
                             Cell2D                 &startCell,
                             const double            traversabilityThreshhold,
                             const double            robotDiameterMetres )
{
    Cell2DVector skel;
    FloatMap distGrid;
    FloatMap costMap;
    
    // 1. Compute skeleton for this ogmap
    if ( !computeSkeleton( ogMap,
                           skel,
                           distGrid,
                           traversabilityThreshhold,
                           robotDiameterMetres ) ) return false;

    // 2. Connect start cell to skeleton 
    if( !connectCell2Skeleton( skel,
                               startCell,
                               distGrid,
                               util::robotDiameterInCells(ogMap, robotDiameterMetres) ) ) return false;

    // 3. calculate costs
    computeCostsFromDistGrid( distGrid, costMap, ogMap.metresPerCell() );

    // 4. Compute potential function U along the skeleton
    if( !computePotentialSkeleton( ogMap, costMap, navMap, skel, startCell ) ) return false;

    // 5. Compute potential function U in the free space
    computePotentialFreeSpace( ogMap, costMap, navMap, skel, traversabilityThreshhold );

    return true;
}

void
computeCostsFromDistGrid( const FloatMap      &distGrid,
                          FloatMap            &costMap,
                          double               metresPerCell,
                          const CostEvaluator &costEvaluator )
{
    // initialise the costMap
    costMap.resize( distGrid.sizeX(), distGrid.sizeY() );
    costMap.fill( NAN );

    for ( unsigned int xi = 0; xi < distGrid.sizeX(); xi++ )
    {
        for ( unsigned int yi = 0; yi < distGrid.sizeY(); yi++ )
        {
            costMap.setElement( xi, yi, 
                                costEvaluator.costAtDistFromObstacle( metresPerCell*element(distGrid,xi,yi) ) );
        }
    }
}

void computeUniformCosts( const hydroogmap::OgMap &ogMap,
                          FloatMap               &costMap,
                          double                  traversabilityThreshhold )
{

    costMap.resize( ogMap.numCellsX(), ogMap.numCellsY() );

    for ( int x=0; x < ogMap.numCellsX(); x++ )
    {
        for ( int y=0; y < ogMap.numCellsY(); y++ )
        {
            if ( isTraversable( ogMap, x, y, traversabilityThreshhold ) )
                costMap.setElement( x, y, 1 );
            else
                costMap.setElement( x, y, NAN );
        }
    }
}

bool computeSkeleton( const hydroogmap::OgMap &ogMap,
                      Cell2DVector           &skel,
                      FloatMap               &distGrid,
                      double                  traversabilityThreshhold,
                      double                  robotDiameterMetres )
{    
    assert( traversabilityThreshhold >= 0.0 && traversabilityThreshhold <= 1.0 );
    int robotDiameterCells = util::robotDiameterInCells( ogMap, robotDiameterMetres );

    int sx = ogMap.numCellsX();
    int sy = ogMap.numCellsY(); 

    double M = 2.0 * sx * sy;

    // initalise the distance grid
    distGrid.resize( sx, sy );
    distGrid.fill( 0 );

    // take robot size into account
    const int ALPHA = 2 * MAX(robotDiameterCells/2,2);

    Cell2DVector Lcurr, Lnext;
    Cell2D q, q1, c, c1;
    
    IntMap zeroMap;
    zeroMap.resize( sx, sy );    
    // AlexB: added this just to be sure uninitialised elements aren't touched...
    zeroMap.fill( -1 );

    for ( int i=0; i<sx; i++ )
    {
        for ( int j=0; j<sy; j++ )
        {
            // if cell is in free space and not on the border of the map
            if ( isTraversable( ogMap, i, j, traversabilityThreshhold ) && !isBorderCell(ogMap,i,j) )
            {
                // for every cell q in GCfree do U(q)=M (large number)
                distGrid.setElement( i,j, M );
            }
            // cell is occupied
            else 
            {
                // if the cell is occupied and has at least one non-occupied neighbour
                // (ie it lies on a border)
                if ( isTraversableNeighbors( ogMap, i, j, traversabilityThreshhold ) )
                {
                    zeroMap.setElement( i,j, sub2ind( i,j, sx, sy ) );
                    Lcurr.push_back( Cell2D( i,j ) );
                } 
            } 
        }
    }

    // AlexB: I think:
    //        At this point:
    //        - LCurr    : the list of all cells within an obstacle but bordering GCfree
    //        - distGrid : every cell in GCfree = M, every other cell = 0
    //        - zeroMap  : every border cell (ie elements of LCurr): a unique ID
    //                     every other  cell: undefined
    //
    //        This next section:
    //        - sets distGrid to the 4-distance to the nearest obstacle
    //        - sets zeroMap: each cell will contain the ID of the border point that originated 
    //                        the wave which first reached that cell.
    
    // FIND SKELETON
    // for i=0,1,..., until Li is empty
    while ( !Lcurr.empty() )
    {
        // for every q in Li
        for ( Cell2DVector::iterator it=Lcurr.begin(); it!=Lcurr.end(); it++ )
        {
            q = *it;

            // for every 1st neighbor q' of q in GC_free
            for ( int k=0; k<4; k++ )
            {
                q1 = adjacentCell(q, k);

                // if q1 is traversable ...
                if ( element( distGrid, q1 ) != 0 )
                {
                    int val;
                    if ( element(distGrid, q1 )==M ) // i.e. traversable and untouched
                    {
                        distGrid.setElement( q1, element(distGrid, q ) + AC );
                        zeroMap.tryElement( q, val);
                        zeroMap.setElement( q1, val );
                        Lnext.push_back( q1 );
                    }
                    else
                    { 
                        // two waves meet
                        // Set c and c1 to the cells of the wave-originating border points.
                        zeroMap.tryElement( q, val  );
                        c  = ind2sub( val, sx, sy );
                        zeroMap.tryElement( q1, val  );
                        c1 = ind2sub( val, sx, sy );
                        // if distance between the two originating points is greater than ALPHA
                        if ( euclideanDistance( c, c1 ) > ALPHA )
                        {
                            // AlexB: How does this check work?
                            if ( element(distGrid, q ) <= element(distGrid, q1 ) 
                                 && !isIncluded( skel, q ) && !isIncluded( skel, q1 ) )
                            {
                                skel.push_back( q1 );
                            }
                        }
                    }
                }
            }
        }
        Lcurr.swap( Lnext );
        Lnext.clear();
    }

    if ( skel.size()==0 ) return false;

    return true;
}

bool
findClosestPointOnSkeleton( const Cell2DVector   &skel,
                            const FloatMap       &distGrid,
                            const Cell2D         &fromCell,
                            Cell2D               &closestCell )
{
    Cell2D currentCell = fromCell;
    Cell2DVector visitedCells;

    while ( !isIncluded( skel, currentCell ) )
    {
        visitedCells.push_back( currentCell );

        // select a neighbor having the largest value d1 i.e. move away from obstacles
        currentCell = max8Cells( distGrid, currentCell );

        if( isIncluded( visitedCells, currentCell ) )
        {
            // Can't reach skeleton
            cout<<"TRACE(pathplanutils.cpp): Couldn't reach skeleton from " << fromCell << endl;
            return false;
        }
    }
    closestCell = currentCell;
    return true;
}


bool
connectCell2Skeleton( Cell2DVector   &skel,
                      Cell2D         &cell,
                      const FloatMap &distGrid,
                      int             robotDiameterCells )
{
    // G - a list of configuration connection goal to S
    Cell2DVector G;

    // check current cell is sane (and shift it slightly if it isn't)
    if( !validateCell( distGrid, cell, robotDiameterCells, false ) ) 
    {
        cout << "ERROR(pathplanutils.cpp, connectCell2Skeleton): cell is invalid" <<endl;
        return false;
    }
    
    // q <- q_goal
    Cell2D q = cell;
    
    // insert q_goal in G
    G.push_back( cell );
    
    // while q is not in S
    unsigned int count = 0;
    while ( !isIncluded( skel, q ) )
    {
        // select a neighbor q' having the largest value d1 i.e. move away from obstacles
        q = max8Cells( distGrid, q );

        if( !isIncluded( G, q ) )
        {
            G.push_back( q );

            //cout<<"("<<q.x()<<","<<q.y()<<")"<<endl;
            if ( ++count>distGrid.sizeX() )
            {
                std::cout<<"ERROR(pathplanutils.cpp, connectCell2Skeleton): Can't reach skeleton from cell ("<<cell.x()<<","<<cell.y()<<") "<<"skeleton size:"<<skel.size()<<std::endl;
                return false;
            }
        }
    }
    
    // combine SIGMA and S ( drop the last element which is in both vectors )
    skel.insert( skel.end(), G.begin(), G.end() - 1 );
    
    return true;
}

bool
computePotentialSkeleton( const OgMap        &ogMap,
                          const FloatMap     &costMap,
                          FloatMap           &navMap,
                          const Cell2DVector &skel,
                          const Cell2D       &startCell )
{
    // startCell must be on the skeleton
    if( !isIncluded( skel, startCell ) ) 
    {
        cout << "ERROR(pathplanutils.cpp, skeleton2SkeletonUtil): Start cell does not lie on the skeleton" << endl;
        return false;   
    }

    // initialise navMap
    navMap.resize( ogMap.numCellsX(), ogMap.numCellsY() );
    navMap.fill( NAN );

    // U(q_goal) = 0
    navMap.setElement( startCell, 0.0 );

    // INSERT(startcell, Q)
    Cell2DList Q;
    Q.push_back( startCell );

    Cell2D q, q1;
    double qval, q1val;

    cout << "Size of the skel: " << skel.size() << endl;

    // while !EMPTY(Q)
    while ( !Q.empty() )
    {
        //cout << "Q size: " << Q.size() << endl;

        // q<-FIRST(Q) [assign Q(0) to q and remove it]
        q = Q.front(); //always pull off lowest utility valued cell i.e. cell that is closest to start
        Q.pop_front();
        
        qval = element(navMap,q);

        // for every m-neighbors q' of q in S (this is m-dim space, so 2nd neighbors)
        for ( unsigned int k=0; k<8; k++ )
        {
            q1 = surroundCell(q,k);
            
            // if cell is not part of the skeleton go back to beginning of for loop
            if ( !isIncluded( skel, q1 ) ) continue; 

            // if we haven't set the potential yet we need to
            if ( containsNan(navMap, q1) )
            {
//                 if ( k<4 ) {
//                     q1val = qval + AC;  // adjacent cells (0-3)
//                 } else {
//                     q1val = qval + DC;  // diagonal cells (4-7)
//                 }
                q1val = qval + element( costMap, q1 );
                navMap.setElement( q1, q1val );
                
                bool hasInserted = false;
               
                // reverse the list, so we can traverse it with a normal iterator (and use insert)
                Q.reverse();
                for ( Cell2DList::iterator it=Q.begin(); it!=Q.end(); it++ )
                {
                    if ( q1val <= element(navMap, *it ) ) 
                    {
                        Q.insert( it, q1 ); //insert it just in front of *it
                        hasInserted = true;
                        break;
                    }
                }
                // we need this trick: if q1val wasn't smaller than any cell's potential in the list, it didn't insert it at all
                if (!hasInserted)
                {
                    Q.push_back( q1 );
                }
            }
            
        }//for
    }//while

    return true;
}

void computePotentialFreeSpace( const OgMap        &ogMap,
                                const FloatMap     &costMap,
                                FloatMap           &navMap,
                                const Cell2DVector &skel,
                                const double        traversabilityThreshhold )
{
    double M = 2.0 * ogMap.numCellsX() * ogMap.numCellsY();

    Cell2DVector Lcurr, Lnext;
    Cell2D q, q1;

    // initialize Lo
    Lcurr = skel;

    // for i=0,1,..., until Li is empty
    while ( !Lcurr.empty() )
    {
        // for every q in Li
        for (  Cell2DVector::iterator it=Lcurr.begin(); it!=Lcurr.end(); it++  )
        {
            q = *it;

            for ( int k=0; k<4; k++ )
            {
                q1 = adjacentCell(q,k);
                if ( util::isTraversable( ogMap, q1, traversabilityThreshhold ) )
                {
                    if ( element(navMap, q1 ) == M  )
                    {
                        navMap.setElement( q1, element(navMap, q ) + element(costMap,q1) );
                        Lnext.push_back( q1 );
                    }
                }
            }
        }
        Lcurr.swap(Lnext);
        Lnext.clear();
    }
}

Result calcPath( const OgMap    &ogMap,
                 const FloatMap &navMap,
                 Cell2D         &goalCell,
                 Cell2DVector   &path,
                 double          robotDiameterMetres )
{
    
    // Make sure that navigation map is not empty and values are not all nans
    assert( navMap.sizeX()!=0 && navMap.sizeY()!=0 && !areAllNans(navMap) );
    
    int robotDiameterCells = util::robotDiameterInCells( ogMap, robotDiameterMetres );

    //check if target position is sane
    if( !validateCell( navMap, goalCell, robotDiameterCells, true ) )
    {
        cout<<"ERROR(pathplanutils.cpp, calcPath): Goal cell is not valid"<<endl;
        return PathDestinationNotValid;
    }

    cout << "INFO(pathplanutils.cpp, calcPath): Goal cell is validated" << endl;

    // start at target and work backwards
    Cell2D cnext = goalCell;
    Cell2D cprev;
    int count = 0;
    path.clear();
    path.push_back( cnext );

    // sanity check for path length
    //TODO: Magic number??
    const int MAX_PATH_LENGTH = 5*navMap.sizeX();

    // termination conditions: zero distance or max path length
    while ( element(navMap, cnext ) > 0.0 )
    {
        cprev = cnext;
        cnext = min8Cells( navMap, cnext );
        
//         //Debugging only
//         float worldX, worldY;
//         ogMap.getWorldCoords(cnext.x(),cnext.y(),worldX,worldY);
//         cout << cnext << ": (" << worldX << ", " << worldY <<"):" << element(navMap, cnext ) << endl;
        
        if ( cnext == cprev )
        {
            cout<<"ERROR(pathplanutils.cpp): Cannot calculate path, stuck in local minimum (error in previously calculated navigation function)"<<endl;
            return PathDestinationUnreachable;
        }
        if( count++ > MAX_PATH_LENGTH )
        {
            cout<<"ERROR(pathplanutils.cpp): Path to long"<<endl;
            return PathDestinationUnreachable;
        }
        path.push_back( cnext );
    }

    //path should start from starting point i.e. element( cnext ) = 0.0
    reverse(path.begin(),path.end());
    return PathOk;

}

}
}
