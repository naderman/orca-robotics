#ifndef VFHDRIVER_H
#define VFHDRIVER_H

#include <localnavdriver.h>
#include <orcaice/context.h>
#include <orcaice/timer.h>
#include <goalwatcher.h>
#include <vfhdriver/vfh_algorithm.h>

//
// Implements VFH.
// All the number-crunching is done by the VFH_Algorithm class.
// This thing just feeds in configuration and information, plus
// tries heuristic escape approaches if the robot gets stuck.
//
// @author Alex Brooks
//
class VfhDriver : public LocalNavDriver
{

public: 

    VfhDriver( const GoalWatcher        &goalWatcher,
               orcaice::Context context );
    virtual ~VfhDriver();

    virtual void setLocalNavParameters( LocalNavParameters params );

    // Goal location is in robot's coordinate frame
    virtual LocalNavDriver::DriverState getCommand( bool  stalled,
                                                    const orca::Twist2d &currentVelocity,
                                                    const orca::RangeScannerDataPtr obs,
                                                    orca::Velocity2dCommandPtr &cmd );

private: 

    // Functions for setting commands
    void setToZero(         orca::Velocity2dCommandPtr &cmd );
    void setToEscape(       orca::Velocity2dCommandPtr &cmd, const orca::RangeScannerDataPtr &obs );
    void setTurnToGoal(     orca::Velocity2dCommandPtr &cmd, const GoalWatcher &goalWatcher );
    void setToApproachGoal( orca::Velocity2dCommandPtr &cmd,
                            const GoalWatcher   &goalWatcher, 
                            const orca::Twist2d &currentVelocity,
                            const orca::RangeScannerDataPtr &obs );
    
    // If we stall a lot, our sensors must have missed something.  
    // We need to try something else.
    bool shouldEscape( bool stalled );

    // Copy to Player units
    void copyLaserScan( const orca::RangeScannerDataPtr obs, double playerLaserScan[361][2] );

    // Class to handle the internal VFH algorithm
    // (like maintaining histograms etc).
    // This is the guy that does all the number-crunching.
    VFH_Algorithm *vfhAlgorithm_;

    // Range and bearing values in player units, to give to vfh engine
    // The '361' is what vfh_algorithm expects
    double playerLaserScan_[361][2];

    // An indicator of how much we've been stalling lately
    float stallRatio_;

    // Timing for escapes
    orcaice::Timer             escapeTimer_;
    static const double        escapeTimeMs_ = 1000.0;

    // Navigation parameters
    LocalNavParameters         localNavParameters_;

    // Current state of the algorithm
    LocalNavDriver::DriverState  currentState_;

    // Previous command
    orca::Velocity2dCommandPtr prevCmd_;

    // Configuration from file
    VfhAlgorithmConfig vfhConfig_;

    orcaice::Context context_;
};

#endif
