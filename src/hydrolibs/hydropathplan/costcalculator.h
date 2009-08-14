#ifndef COSTCALCULATOR_H
#define COSTCALCULATOR_H

#include <hydroogmap/ogmap.h>
#include <hydroogmap/mapraytracer.h>
#include <hydropathplan/util.h>

namespace hydropathplan {

#if 0
//
// @author Alex Brooks
//
class CostAccumulator {
public:
    CostAccumulator( const FloatMap &costMap )
        : costMap_(costMap),
          costSoFar_(0)
        {}

    inline bool traceCell( int x, int y )
        {
            costSoFar_ += element( costMap_, x, y );

            // keep tracing.
            return true;
        }

    inline void traceSucceeded( int x, int y ) {}
    inline void traceLeftMap( int x, int y ) {}

    double totalCost() const { return costSoFar_; }

private:
    const FloatMap &costMap_;
    double costSoFar_;
};

typedef hydroogmap::MapRayTracer<const hydroogmap::OgMap,CostAccumulator> CostCalculator;
#endif

}

#endif
