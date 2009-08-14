#ifndef HYDROSIM2D_OBSTACLEUTIL_H
#define HYDROSIM2D_OBSTACLEUTIL_H

#include <hydroogmap/hydroogmap.h>

namespace hydrosim2d {

//! Adds a square obstacle at this point in the map
void
placeSquareObstacle( hydroogmap::OgMap &ogMap,
                     double x,
                     double y,
                     double width );

}

#endif
