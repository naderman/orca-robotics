#ifndef HYDROOGMAP_OGLOSTRACER_H
#define HYDROOGMAP_OGLOSTRACER_H

#include <hydroogmap/ogmap.h>
#include <hydroogmap/mapraytracer.h>

namespace hydroogmap {

//!
//! @brief Computes whether line-of-sight exists in an occupancy grid map
//!
//! @author Alex Brooks
//!
class OgLosTracer
{

public: 

    OgLosTracer( const hydroogmap::OgMap &theMap,
                 unsigned char occupiedThreshold=hydroogmap::CELL_UNKNOWN )
        : theMap_(theMap),
          cellTracer_(theMap,occupiedThreshold),
          mapRayTracer_(theMap,cellTracer_)
        {}

    //! Trace from start(x0,y0) to intendedEnd(x1,y1).  
    //! Puts the point at which it stopped tracing in actualEnd(endX,endY).
    //! Returns: 
    //!   true:  we didn't hit an obstacle.
    //!   false: we hit an obstacle or left the map.
    //!
    bool isClearCells( const GridIndices &start, const GridIndices &intendedEnd, GridIndices &actualEnd )
        {
            bool isLineOfSight = mapRayTracer_.traceCells( start, intendedEnd );
            if ( !isLineOfSight )
            {
                actualEnd = GridIndices( cellTracer_.lastEndX(), cellTracer_.lastEndY() );
                return isLineOfSight;
            }
            else
            {
                actualEnd = intendedEnd;
                return isLineOfSight;
            }
        }
    bool isClearCells( int x0,int y0,  int x1,int y1,  int &xEnd,int &yEnd )
        {
            GridIndices end;
            bool isClear = isClearCells( GridIndices(x0,y0), GridIndices(x1,y1), end );
            xEnd = end.x; yEnd = end.y;
            return isClear;
        }

    //! Trace from start(x0,y0) to intendedEnd(x1,y1).  
    //! Returns: 
    //!   true:  we didn't hit an obstacle.
    //!   false: we hit an obstacle or left the map.
    //!
    bool isClearCells( const GridIndices &start, const GridIndices &intendedEnd )
        {
            return mapRayTracer_.traceCells( start, intendedEnd );
        }
    bool isClearCells( int x0,int y0,  int x1,int y1 )
        { return isClearCells( GridIndices(x0,y0), GridIndices(x1,y1) ); }

    //! Similar to isClearCells, but specify endpoints in world coords.
    bool isClearWorld( const WorldCoords &start, const WorldCoords &intendedEnd, WorldCoords &actualEnd )
        {
            GridIndices actualEndIndices;
            bool clear = isClearCells( theMap_.gridIndices(start),
                                       theMap_.gridIndices(intendedEnd),
                                       actualEndIndices );
            actualEnd = theMap_.worldCoords( actualEndIndices );
            return clear;
        }
    bool isClearWorld( double x0,double y0,  double x1,double y1,  double &xEnd,double &yEnd )
        {
            WorldCoords end;
            bool isClear = isClearWorld( WorldCoords(x0,y0), WorldCoords(x1,y1), end );
            xEnd = end.x; yEnd = end.y;
            return isClear;
        }

    //! Similar to isClearCells, but specify endpoints in world coords.
    bool isClearWorld( const WorldCoords &start, const WorldCoords &intendedEnd )
        {
            return mapRayTracer_.traceCoords( start, intendedEnd );
        }
    bool isClearWorld( double x0,double y0,  double x1,double y1 )
        {
            return isClearWorld( WorldCoords(x0,y0), WorldCoords(x1,y1) );
        }

private: 

    // Traces through the map, stops when it hits an occupied cell
    class CellTracer {
    public:

        CellTracer( const hydroogmap::OgMap &theMap,
                    unsigned char occupiedThreshold=hydroogmap::CELL_UNKNOWN )
            : theMap_(theMap),
              occupiedThreshold_(occupiedThreshold)
        {}

        inline bool traceCell(int x, int y)
            {
                if ( theMap_.gridCell(x,y) > occupiedThreshold_ )
                {
                    // Occupied: stop tracing.
                    lastEndX_ = x;
                    lastEndY_ = y;
                    return false;
                }
                // Keep tracing.
                return true;
            }

        inline void traceSucceeded( int x, int y )
            { lastEndX_ = x; lastEndY_ = y; }

        inline void traceLeftMap( int x, int y )
            { lastEndX_ = x; lastEndY_ = y; }

        //! The coordinates where the previous ray-trace stopped.
        int lastEndX() const { return lastEndX_; }
        //! The coordinates where the previous ray-trace stopped.
        int lastEndY() const { return lastEndY_; }

    private:

        const hydroogmap::OgMap &theMap_;
        const unsigned char occupiedThreshold_;

        // The coordinates of the end of the last ray-trace.
        int lastEndX_;
        int lastEndY_;
    };
    
    const hydroogmap::OgMap &theMap_;
    mutable CellTracer       cellTracer_;
    MapRayTracer<const OgMap,CellTracer> mapRayTracer_;
};

}

#endif
