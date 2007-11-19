#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <orcaogmap/orcaogmap.h>
#include <hydronavutil/hydronavutil.h>
#include <orcaifaceimpl/laserscanner2dImpl.h>
#include <orcaifaceimpl/localise2dImpl.h>
#include <orcaifaceimpl/ogmapImpl.h>
#include <orca/velocitycontrol2d.h>
#include <orca/odometry2d.h>
#include <hydroutil/proxy.h>
#include <orcaice/context.h>
#include <orca/pathfollower2d.h>

namespace localnav {

//!
//! @author Alex Brooks
//!
class Simulator
{

public: 

    Simulator( const orcaice::Context &context,
               const orca::PathFollower2dData &testPath );
    ~Simulator();

    // This is the trigger to advance the simulator one step.
    void setCommand( const orca::VelocityControl2dData &cmd );

    // These can be given out to others: the simulator 
    // will put new data in them on each step.
    hydroutil::Proxy<orca::RangeScanner2dDataPtr> obsProxy_;
    hydroutil::Proxy<orca::Localise2dData>        locProxy_;
    hydroutil::Proxy<orca::Odometry2dData>        odomProxy_;

    void printState();

    orca::VehicleDescription        getVehicleDescription() const;
    
    const orca::RangeScanner2dDescription& rangeScanner2dDescription() const
        { return scannerDescr_; }
    
private: 

    void setupMap();
    void setupInterfaces();
    void applyCurrentVelocity();
    void getRanges();
    void fillPipes();
    void checkProgress();
    // get simulation time
    orca::Time now();

    hydroogmap::OgMap       ogMap_;
    hydroogmap::OgMap       grownOgMap_;
    hydroogmap::OgLosTracer rayTracer_;

    hydronavutil::Pose      pose_;
    double                 velLin_;
    double                 velRot_;

    orcaifaceimpl::LaserScanner2dImpl *laserInterface_;
    orcaifaceimpl::Localise2dImpl     *localiseInterface_;
    orcaifaceimpl::OgMapImpl          *ogMapInterface_;

    orca::LaserScanner2dDataPtr     scan_;
    orca::RangeScanner2dDescription scannerDescr_;

    orca::PathFollower2dData testPath_;

    int iterationNum_;

    bool batchMode_;
    
    orcaice::Context context_;
};

}

#endif
