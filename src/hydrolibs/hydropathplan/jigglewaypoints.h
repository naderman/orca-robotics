#ifndef HYDROPATHPLAN__H
#define HYDROPATHPLAN__H

#include <hydroogmap/hydroogmap.h>

namespace hydropathplan {

// If the cell isn't clear, jiggle it onto a nearby clear cell
void jiggleOntoClearCell( int &cellX, int &cellY,
                          const hydroogmap::OgMap &ogMap,
                          double traversabilityThreshhold );

}

#endif
