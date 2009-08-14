#ifndef HYDROSIM2D_VEHICLESIMULATOR_H
#define HYDROSIM2D_VEHICLESIMULATOR_H

#include <hydroogmap/hydroogmap.h>
#include <hydronavutil/hydronavutil.h>
#include <hydrosim2d/iposepublisher.h>
#include <hydrosim2d/iodompublisher.h>

namespace hydrosim2d {

//!
//! @brief Simulates a vehicle moving in a 2D environment
//!
//! @author Alex Brooks
//!
class VehicleSimulator
{

public: 

    class Config {
    public:
        double robotRadius;
        double timeStep;
        bool   checkDifferentialConstraints;
        double maxLinearAcceleration;
        double maxRotationalAcceleration;
        bool   checkLateralAcceleration;
        double maxLateralAcceleration;
        bool   checkVelocityConstraints;
        hydronavutil::Velocity minVelocity;
        hydronavutil::Velocity maxVelocity;
    };

    ////////////////////////////////////////

    VehicleSimulator( const Config            &config,
                      const hydroogmap::OgMap &ogMap,
                      IPosePublisher          &posePublisher,
                      IOdomPublisher          *odomPublisher = NULL );

    //! This is the trigger to advance the simulator one step.
    void act( const hydronavutil::Velocity &cmd );

    //! Has the vehicle collided with an obstacle?
    bool isCollided() const;

    hydronavutil::Pose     pose() const { return pose_; }
    hydronavutil::Velocity velocity() { return velocity_; }

    const Config &config() const { return config_; }

private: 

    const Config config_;

    // Grow the OG map by the robot radius, so we can treat the robot as a point.
    hydroogmap::OgMap       grownOgMap_;

    hydronavutil::Pose      pose_;
    hydronavutil::Velocity  velocity_;

    IPosePublisher &posePublisher_;
    IOdomPublisher *odomPublisher_;
};

}

#endif
