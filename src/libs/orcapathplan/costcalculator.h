#ifndef COSTCALCULATOR_H
#define COSTCALCULATOR_H

#include <orcaogmap/ograytracer.h>
#include <orcapathplan/pathplanutils.h>

namespace orcapathplan {

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

    double totalCost() const { return costSoFar_; }

private:
    const FloatMap &costMap_;
    double costSoFar_;
};

typedef orcaogmap::OgRayTracer<CostAccumulator> CostCalculator;

}

#endif
