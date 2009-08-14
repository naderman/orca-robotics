#ifndef HYDROOGMAP_MAPRAYTRACER_H
#define HYDROOGMAP_MAPRAYTRACER_H

#include <hydroogmap/raytracer.h>

namespace hydroogmap {

//!
//! @brief Adapts the RayTracer gear for dealing with 'GenericMap's.
//!        The template parameter CellTracer gets called once per cell that we trace through.
//!          Note that we guarantee that CellTracer doesn't get called for a cell outside the map.
//!          For an example CellTracer, see OgLosTracer.
//!
//! @Author Alex Brooks
//!
template<typename MapType,
         class MapCellTracer>
class SOEXPORT MapRayTracer {

public:

    MapRayTracer( MapType       &theMap,
                  MapCellTracer &mapCellTracer )
        : theMap_(theMap),
          uncheckedMapCellTracer_(mapCellTracer),
          uncheckedRayTracer_(uncheckedMapCellTracer_),
          safeMapCellTracer_(theMap,uncheckedMapCellTracer_),
          safeRayTracer_(safeMapCellTracer_)
        {}

    //! Trace from start(x0,y0) to destination(x1,y1).  
    //! Returns: 
    //!   true:  we traced to the end
    //!   false: we stopped tracing early
    //!
    bool traceCells( const GridIndices &start, const GridIndices &destination )
        {
            if ( theMap_.cellWithinMap(start) &&
                 theMap_.cellWithinMap(destination) )
                return uncheckedRayTracer_.trace( start.x, start.y, destination.x, destination.y );
            else
                return safeRayTracer_.trace( start.x, start.y, destination.x, destination.y );
        }
    bool traceCells( int x0,int y0,  int x1,int y1 )
        { return traceCells( GridIndices(x0,y0), GridIndices(x1,y1) ); }

    //! Similar to traceCells, but specify endpoints in world coords.
    bool traceCoords( const WorldCoords &start, const WorldCoords &destination )
        {
            return traceCells( theMap_.gridIndices(start), theMap_.gridIndices(destination) );
        }
    bool traceCoords( double x0,double y0,  double x1,double y1 )
        { return traceCoords( WorldCoords(x0,y0), WorldCoords(x1,y1) ); }

private:

    class SafeMapCellTracer
    {
    public:
        SafeMapCellTracer( MapType       &theMap, 
                           MapCellTracer &uncheckedMapCellTracer )
            : theMap_(theMap),
              uncheckedMapCellTracer_(uncheckedMapCellTracer)
            {}

        //! Keeps tracing until it goes off the edge of the map,
        //! or finds an occupied cell.
        inline bool traceCell(int x, int y)
            {
                if ( !theMap_.cellWithinMap( x, y ) )
                {
                    // Outside the map: stop tracing.
                    uncheckedMapCellTracer_.traceLeftMap( x, y );
                    return false;
                }
                else
                {
                    return uncheckedMapCellTracer_.traceCell( x, y );
                }
            }

        inline void traceSucceeded( int x, int y )
            { uncheckedMapCellTracer_.traceSucceeded(x,y); }

    private:

        MapType       &theMap_;
        MapCellTracer &uncheckedMapCellTracer_;
    };

    MapType &theMap_;

    // Use this if we don't care about the exact endpoint, and 
    // we know we're gonna stay on the map
    MapCellTracer            &uncheckedMapCellTracer_;
    RayTracer<MapCellTracer>  uncheckedRayTracer_;

    // Use this if we're gonna go off the map, and we want to know when
    SafeMapCellTracer            safeMapCellTracer_;
    RayTracer<SafeMapCellTracer> safeRayTracer_;
};

}

#endif
