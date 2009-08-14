#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <hydrosim2d/hydrosim2d.h>
#include <orca/pathfollower2d.h>
#include <orcasim2d/orcasim2d.h>
#include <memory>
#include <orcaifaceimpl/ogmap.h>

namespace orcalocalnavtest {

//
// @author Alex Brooks
//
class Simulator
{

public: 

    struct Config {
        double maxLateralAcceleration;
        bool   checkLateralAcceleration;
        bool   checkDifferentialConstraints;
        bool   checkVelocityConstraints;
        bool   useRoom;
        bool   batchMode;
        int    numIterationsBatch;
        int    numIterationsLimit;
    };

    Simulator( const orcaice::Context         &context,
               const hydroogmap::OgMap        &ogMap,
               const orca::PathFollower2dData &testPath,
               const Config                   &config );

    // This is the trigger to advance the simulator one step.
    void act( const hydronavutil::Velocity &cmd );

    void checkProgress( bool &pathCompleted, bool &pathFailed );

    void getObsRanges( std::vector<float> &obsRanges )
        { rangeScannerSimulator_->getRangesFromPose( vehicleSimulator_->pose(), obsRanges ); }

    hydronavutil::Pose pose() const
        { return vehicleSimulator_->pose(); }
    hydronavutil::Velocity velocity() const
        { return vehicleSimulator_->velocity(); }

    orca::VehicleDescription vehicleDescription() const
        { return vehicleDescr_; }

    const orca::RangeScanner2dDescription& rangeScanner2dDescription() const
        { return scannerDescr_; }
    
    orca::Time time() const;

    const hydrosim2d::VehicleSimulator &vehicleSimulator() const { return *vehicleSimulator_; }
    const hydroogmap::OgMap &ogMap() const { return ogMap_; }

    const orca::PathFollower2dData &testPath() const { return testPath_; }

    static std::string scannerInterfaceName()  { return "TestLaserScanner"; }
    static std::string localiseInterfaceName() { return "TestLocalise"; }
    static std::string odomInterfaceName()     { return "TestOdometry"; }

private: 

    void setupInterfaces( const hydrosim2d::VehicleSimulator::Config &vehicleSimConfig,
                          const hydrosim2d::RangeScannerSimulator::Config rangeScanSimConfig );

    std::auto_ptr<orcasim2d::PosePublisher>      posePublisher_;
    std::auto_ptr<orcasim2d::RangeScanPublisher> rangeScanPublisher_;
    std::auto_ptr<orcasim2d::OdomPublisher>      odomPublisher_;
    orcaifaceimpl::OgMapImplPtr                  ogMapInterface_;

    orca::RangeScanner2dDescription scannerDescr_;
    orca::VehicleDescription        vehicleDescr_;

    hydroogmap::OgMap        ogMap_;
    orca::PathFollower2dData testPath_;

    int    iterationNum_;
    int    wpI_;

    std::auto_ptr<hydrosim2d::VehicleSimulator>      vehicleSimulator_;
    std::auto_ptr<hydrosim2d::RangeScannerSimulator> rangeScannerSimulator_;

    const Config config_;

    orcaice::Context context_;
};

std::string toString( const Simulator::Config &c );

}

#endif
