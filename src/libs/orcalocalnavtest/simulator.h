#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <hydrosim2d/hydrosim2d.h>
#include <orca/pathfollower2d.h>
#include <orca/vehicledescription.h>
#include <memory>
#include <orcaifaceimpl/ogmap.h>
#include <hydropublish/rangescanner2dpublisher.h>
#include <hydropublish/odometry2dpublisher.h>
#include <hydropublish/localise2dpublisher.h>
#include <hydropublish/ogmappublisher.h>
#include <orcascanutil/orcascanutil.h>
#include <orcanavutil/orcanavutil.h>

namespace orcalocalnavtest {

//
// @author Alex Brooks
//
class Simulator
{

public: 

    struct Config {
        Config()
            : scannerOffset(0.15,0,0), initialPose(0,0,0), applyNoises(false) {}

        double                                    maxLateralAcceleration;
        bool                                      checkLateralAcceleration;
        bool                                      checkDifferentialConstraints;
        bool                                      checkVelocityConstraints;
        bool                                      useRoom;
        bool                                      batchMode;
        int                                       numIterationsBatch;
        int                                       numIterationsLimit;
        hydronavutil::Pose                        scannerOffset;
        hydronavutil::Pose                        initialPose;
        bool                                      applyNoises;
        hydrosim2d::VehicleSimulator::NoiseConfig noises;
    };

    Simulator( hydropublish::RangeScanner2dPublisherFactory &rangeScannerPublisherFactory,
               hydropublish::Localise2dPublisherFactory     &localise2dPublisherFactory,
               hydropublish::Odometry2dPublisherFactory     &odometry2dPublisherFactory,
               hydropublish::OgMapPublisherFactory          &ogMapPublisherFactory,
               const hydroogmap::OgMap                      &ogMap,
               const orca::PathFollower2dData               &testPath,
               const Config                                 &config );

    // This is the trigger to advance the simulator one step.
    void act( const hydronavutil::Velocity &cmd );

    void checkProgress( bool &pathCompleted, bool &pathFailed );

    void getObsRanges( std::vector<float> &obsRanges );

    hydronavutil::Pose pose() const
        { return vehicleSimulator_->pose(); }
    hydronavutil::Velocity velocity() const
        { return vehicleSimulator_->velocity(); }

    orca::VehicleDescription vehicleDescription() const
        { return orcanavutil::convert( controlDescr_, geomDescr_ ); }

    orca::RangeScanner2dDescription rangeScanner2dDescription() const
        { return orcascanutil::convert( scannerConfig_, config_.scannerOffset ); }
    
    hydrotime::Time time() const;
    int iterationNum() const { return iterationNum_; }

    const hydrosim2d::VehicleSimulator &vehicleSimulator() const { return *vehicleSimulator_; }
    const hydroogmap::OgMap &ogMap() const { return ogMap_; }

    const orca::PathFollower2dData &testPath() const { return testPath_; }

    static std::string scannerInterfaceName()  { return "TestLaserScanner"; }
    static std::string localiseInterfaceName() { return "TestLocalise"; }
    static std::string odomInterfaceName()     { return "TestOdometry"; }

    hydronavutil::CylindricalGeometryDescription geomDescr() const { return geomDescr_; }

private: 

    void setupInterfaces( const hydrosim2d::VehicleSimulator::Config      &vehicleSimConfig,
                          const hydrosim2d::RangeScannerSimulator::Config &rangeScanSimConfig,
                          hydropublish::RangeScanner2dPublisherFactory    &rangeScanner2dPublisherFactory,
                          hydropublish::Localise2dPublisherFactory        &localise2dPublisherFactory,
                          hydropublish::Odometry2dPublisherFactory        &odometry2dPublisherFactory,
                          hydropublish::OgMapPublisherFactory             &ogMapPublisherFactory );

    hydropublish::Localise2dPublisherPtr     posePublisher_;
    hydropublish::RangeScanner2dPublisherPtr rangeScanPublisher_;
    hydropublish::Odometry2dPublisherPtr     odomPublisher_;
    hydropublish::OgMapPublisherPtr          ogMapPublisher_;

    hydroscanutil::ScannerConfig scannerConfig_;

    hydronavutil::DiffDriveControlDescription    controlDescr_;
    hydronavutil::CylindricalGeometryDescription geomDescr_;

    hydroogmap::OgMap        ogMap_;
    orca::PathFollower2dData testPath_;

    int    iterationNum_;
    int    wpI_;

    std::auto_ptr<hydrosim2d::VehicleSimulator>      vehicleSimulator_;
    std::auto_ptr<hydrosim2d::RangeScannerSimulator> rangeScannerSimulator_;

    const Config config_;
};

std::string toString( const Simulator::Config &c );

}

#endif
