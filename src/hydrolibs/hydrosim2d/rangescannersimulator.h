#ifndef HYDROSIM2D_RANGESCANNERSIMULATOR_H
#define HYDROSIM2D_RANGESCANNERSIMULATOR_H

#include <hydroogmap/hydroogmap.h>
#include <hydroogmap/oglostracer.h>
#include <hydronavutil/hydronavutil.h>
#include <vector>
#include <hydropublish/rangescanner2dpublisher.h>

namespace hydrosim2d {

//!
//! @brief Generates simulated range-scans based on an occupancy-grid.
//!
//! @author Alex Brooks
//!
class RangeScannerSimulator
{

public: 

    struct Config {
        Config() {}
        Config( double pMaxRange,
                double pStartAngle,
                double pAngleIncrement,
                int    pNumReturns )
            : maxRange(pMaxRange),
              startAngle(pStartAngle),
              angleIncrement(pAngleIncrement),
              numReturns(pNumReturns)
            {}
        double maxRange;
        double startAngle;
        double angleIncrement;
        int    numReturns;
    };
    ////////////////////////////////////////

    RangeScannerSimulator( const Config                          &config,
                           const hydroogmap::OgMap               &ogMap,
                           hydropublish::RangeScanner2dPublisher &rangeScanPublisher );

    void getRangesFromPose( const hydronavutil::Pose &sensorPose,
                            std::vector<float>       &ranges,
                            const hydrotime::Time    &time = hydrotime::Time(0,0) ) const;

private: 

    const Config                                   config_;
    const hydroogmap::OgMap                        ogMap_;
    mutable hydroogmap::OgLosTracer                rayTracer_;
    mutable hydropublish::RangeScanner2dPublisher &rangeScanPublisher_;
};

bool isSane( const RangeScannerSimulator::Config &config, std::string &reason );


}

#endif
