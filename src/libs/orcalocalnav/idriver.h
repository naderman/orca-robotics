/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCALOCALNAV_LOCALNAVDRIVER_H
#define ORCALOCALNAV_LOCALNAVDRIVER_H

#include <orca/velocitycontrol2d.h>
#include <orca/rangescanner2d.h>
#include <orcaice/context.h>
#include <orca/vehicledescription.h>
#include <hydronavutil/hydronavutil.h>
#include <orcalocalnav/goal.h>

namespace orcalocalnav {

//
// @author Alex Brooks
//
// Base class for local navigation drivers.
// The manager sets the goal location (in the robot's coordinate system)
// by modifying the goal.
//
class IDriver
{

public: 

    // The pose is in a global coordinate frame, while the goals are
    // in the local coordinate frame.
    struct Inputs {
        bool                            stalled;
        bool                            isLocalisationUncertain;
        hydronavutil::Pose              localisePose;
        orca::Time                      poseTime;
        hydronavutil::Velocity          currentVelocity;
        std::vector<float>              obsRanges;
        orca::Time                      obsTime;
        std::vector<orcalocalnav::Goal> goals;
    };

    ////////////////////////////////////////

    IDriver() {};
    virtual ~IDriver() {};

    // The number of waypoints we look into the future (and therefore
    // expect to be provided with)
    virtual int waypointHorizon() { return 1; }

    // Gets a velocity command.
    virtual hydronavutil::Velocity getCommand( const Inputs &inputs ) = 0;
};

// Helper class to instantiate drivers
class DriverFactory {
public:
    virtual ~DriverFactory() {};
    virtual IDriver *createDriver( const orcaice::Context &context,
                                   const orca::VehicleDescription &vehicleDescr,
                                   const orca::RangeScanner2dDescription &rangeScannerDescr ) const=0;
};

std::string toString( const IDriver::Inputs &inputs );
inline std::ostream &operator<<( std::ostream &s, const IDriver &inputs )
{ return s << inputs; }

} // namespace

// Function for dynamically instantiating drivers.
// A driver must have a function like so:
// extern "C" {
//     orcalocalnav::DriverFactory *createLocalNavDriverFactory();
// }
typedef orcalocalnav::DriverFactory *LocalNavDriverFactoryMakerFunc();

#endif
