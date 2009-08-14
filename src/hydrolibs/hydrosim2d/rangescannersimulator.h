#ifndef HYDROSIM2D_RANGESCANNERSIMULATOR_H
#define HYDROSIM2D_RANGESCANNERSIMULATOR_H

#include <hydroogmap/hydroogmap.h>
#include <hydroogmap/oglostracer.h>
#include <hydronavutil/hydronavutil.h>
#include <vector>
#include <hydrosim2d/irangescanpublisher.h>

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

    RangeScannerSimulator( const Config            &config,
                           const hydroogmap::OgMap &ogMap,
                           IRangeScanPublisher     &rangeScanPublisher );

    void getRangesFromPose( const hydronavutil::Pose &sensorPose,
                            std::vector<float> &ranges ) const;

private: 

    const Config                     config_;
    const hydroogmap::OgMap          ogMap_;
    mutable hydroogmap::OgLosTracer  rayTracer_;
    mutable IRangeScanPublisher     &rangeScanPublisher_;
};

bool isSane( const RangeScannerSimulator::Config &config, std::string &reason );


}

#endif
