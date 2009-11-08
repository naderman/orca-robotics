#ifndef HYDROSIM2D_VEHICLESIMULATOR_H
#define HYDROSIM2D_VEHICLESIMULATOR_H

#include <hydroogmap/hydroogmap.h>
#include <hydronavutil/hydronavutil.h>
#include <hydropublish/localise2dpublisher.h>
#include <hydropublish/odometry2dpublisher.h>

namespace hydrosim2d {

//!
//! @brief Simulates a vehicle moving in a 2D environment
//!
//! @author Alex Brooks
//!
class VehicleSimulator
{

public: 

    struct NoiseConfig {
        NoiseConfig()
            : linearMultiplicativeCov(0.0025),
              rotationalMultiplicativeCov(0.00022),
              thetaXCov(0.00022)
            {}
        double linearMultiplicativeCov;
        double rotationalMultiplicativeCov;
        double thetaXCov;
    };

    struct Config {
        Config()
            : initialPose(0,0,0), applyNoises(false) {}

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
        hydronavutil::Pose initialPose;
        bool applyNoises;
        NoiseConfig noises;
    };

    ////////////////////////////////////////

    VehicleSimulator( const Config                      &config,
                      const hydroogmap::OgMap           &ogMap,
                      hydropublish::Localise2dPublisher *posePublisher = NULL,
                      hydropublish::Odometry2dPublisher *odomPublisher = NULL );

    //! This is the trigger to advance the simulator one step.
    void act( const hydronavutil::Velocity &cmd,
              const hydrotime::Time        &time );

    //! Has the vehicle collided with an obstacle?
    bool isCollided() const;

    hydronavutil::Pose     pose()     const { return pose_; }
    hydronavutil::Velocity velocity() const { return velocity_; }

    const Config &config() const { return config_; }

private: 

    const Config config_;

    // Grow the OG map by the robot radius, so we can treat the robot as a point.
    hydroogmap::OgMap       grownOgMap_;

    hydronavutil::Pose      pose_;
    hydronavutil::Velocity  velocity_;

    hydropublish::Localise2dPublisher *posePublisher_;
    hydropublish::Odometry2dPublisher *odomPublisher_;
};

std::string toString( const VehicleSimulator::NoiseConfig &c );
inline std::ostream &operator<<( std::ostream &s, const VehicleSimulator::NoiseConfig &c )
{ return s << toString(c); }
std::string toString( const VehicleSimulator::Config &c );
inline std::ostream &operator<<( std::ostream &s, const VehicleSimulator::Config &c )
{ return s << toString(c); }

}

#endif
