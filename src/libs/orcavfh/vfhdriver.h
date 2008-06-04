/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef VFHDRIVER_H
#define VFHDRIVER_H

#include <orcalocalnav/idriver.h>
#include <orcaice/context.h>
#include <gbxsickacfr/gbxiceutilacfr/timer.h>
#include <orcalocalnav/goal.h>
#include <orcavfh/vfh_algorithm.h>
#include <orcaice/heartbeater.h>

namespace vfh {

//
// Implements VFH.
// All the number-crunching is done by the VFH_Algorithm class.
// This thing just feeds in configuration and information, plus
// tries heuristic escape approaches if the robot gets stuck.
//
// @author Alex Brooks
//
class VfhDriver : public orcalocalnav::IDriver
{

public: 

    ////////////////////////////////////////////////////////////

    // Constants 
    enum DriverState 
    {
        STATE_GOAL_REACHED,
        STATE_TURNING_AT_GOAL,
        STATE_ESCAPING,
        STATE_MOVING_TO_GOAL
    };

    ////////////////////////////////////////////////////////////

    VfhDriver( const orcaice::Context                &context,
               const orca::VehicleDescription        &descr,
               const orca::RangeScanner2dDescription &scannerDescr );
    virtual ~VfhDriver();

    // Goal location is in robot's coordinate frame
    virtual hydronavutil::Velocity getCommand( const IDriver::Inputs &inputs );

private: 

    void setSpeedConstraints( float maxSpeed, float maxTurnrate );

    // Functions for setting commands
    hydronavutil::Velocity escapeCommand(       const std::vector<float> &obsRanges );
    hydronavutil::Velocity turnToGoalCommand(   const orcalocalnav::Goal &goal );
    hydronavutil::Velocity approachGoalCommand( const orcalocalnav::Goal     &goal, 
                                                const hydronavutil::Velocity &currentVelocity,
                                                const std::vector<float>     &obsRanges );
    
    // If we stall a lot, our sensors must have missed something.  
    // We need to try something else.
    bool shouldEscape( bool stalled );

    // Copy to Player units
    void copyLaserScan( const std::vector<float> &obsRanges, double playerLaserScan[361][2] );

    void maybeSendHeartbeat();

    // Class to handle the internal VFH algorithm
    // (like maintaining histograms etc).
    // This is the guy that does all the number-crunching.
    VFH_Algorithm *vfhAlgorithm_;

    // Range and bearing values in player units, to give to vfh engine
    // The '361' is what vfh_algorithm expects
    double playerLaserScan_[361][2];

    // speed constraints
    float maxSpeed_;
    float maxTurnrate_;

    // An indicator of how much we've been stalling lately
    float stallRatio_;

    // Timing for escapes
    gbxiceutilacfr::Timer             escapeTimer_;
    static const double        escapeTimeMs_ = 1000.0;

    // Current state of the algorithm
    DriverState currentState_;

    // Previous command
    hydronavutil::Velocity prevCmd_;

    // Configuration from file
    VfhAlgorithmConfig vfhConfig_;

    orcaice::Heartbeater heartbeater_;

    const orca::RangeScanner2dDescription scannerDescr_;

    orcaice::Context context_;
};
std::ostream &operator<<( std::ostream &s, VfhDriver::DriverState state );

// Used for dynamically loading driver
class VfhDriverFactory : public orcalocalnav::DriverFactory
{
public:
    orcalocalnav::IDriver *createDriver( const orcaice::Context &context,
                                         const orca::VehicleDescription &vehicleDescr,
                                         const orca::RangeScanner2dDescription &scannerDescr ) const
        {
            return new VfhDriver( context, vehicleDescr, scannerDescr );
        }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    orcalocalnav::DriverFactory *createDriverFactory();
}

#endif
