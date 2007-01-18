/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef VFHDRIVER_H
#define VFHDRIVER_H

#include <localnavutil/idriver.h>
#include <orcaice/context.h>
#include <orcaice/timer.h>
#include <localnavutil/goal.h>
#include <vfhdriver/vfh_algorithm.h>
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
class VfhDriver : public localnav::IDriver
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

    VfhDriver( const orcaice::Context & context );
    virtual ~VfhDriver();

    // Goal location is in robot's coordinate frame
    virtual void getCommand( bool  stalled,
                             const orca::Twist2d &currentVelocity,
                             const orca::RangeScanner2dDataPtr obs,
                             const localnav::Goal               &goal,
                             orca::VelocityControl2dData& cmd );

private: 

    void setSpeedConstraints( float maxSpeed, float maxTurnrate );

    // Functions for setting commands
    void setToZero(         orca::VelocityControl2dData& cmd );
    void setToEscape(       orca::VelocityControl2dData& cmd, const orca::RangeScanner2dDataPtr &obs );
    void setTurnToGoal(     orca::VelocityControl2dData& cmd, const localnav::Goal &goal );
    void setToApproachGoal( orca::VelocityControl2dData& cmd,
                            const localnav::Goal     &goal, 
                            const orca::Twist2d             &currentVelocity,
                            const orca::RangeScanner2dDataPtr &obs );
    
    // If we stall a lot, our sensors must have missed something.  
    // We need to try something else.
    bool shouldEscape( bool stalled );

    // Copy to Player units
    void copyLaserScan( const orca::RangeScanner2dDataPtr obs, double playerLaserScan[361][2] );

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
    orcaice::Timer             escapeTimer_;
    static const double        escapeTimeMs_ = 1000.0;

    // Current state of the algorithm
    DriverState currentState_;

    // Previous command
    orca::VelocityControl2dData prevCmd_;

    // Configuration from file
    VfhAlgorithmConfig vfhConfig_;

    orcaice::Heartbeater heartbeater_;

    orcaice::Context context_;
};
std::ostream &operator<<( std::ostream &s, VfhDriver::DriverState state );

// Used for dynamically loading driver
class VfhDriverFactory : public localnav::DriverFactory
{
public:
    localnav::IDriver *createDriver( const orcaice::Context &context ) const
        {
            return new VfhDriver( context );
        }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    localnav::DriverFactory *createDriverFactory();
}

#endif
