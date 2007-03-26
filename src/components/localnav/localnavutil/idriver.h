/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef LOCALNAVDRIVER_H
#define LOCALNAVDRIVER_H

#include <orca/velocitycontrol2d.h>
#include <orca/rangescanner2d.h>
#include <orcaice/context.h>
#include <orca/vehicledescription.h>
#include <orcanavutil/pose.h>

#include <orcalocalnav/goal.h>

#include <localnavutil/isensormodel.h>
#include <localnavutil/isensordata.h>
#include <localnavutil/isensordescription.h>
#include <localnavutil/icontrol.h>
#include <localnavutil/icontroldata.h>

namespace localnav {

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

    IDriver() {};
    virtual ~IDriver() {};

    // The number of waypoints we look into the future (and therefore
    // expect to be provided with)
    virtual int waypointHorizon() { return 1; }

    // Driver sets everything up, and is told what time it is
    virtual void init( const orca::Time &time )=0;

    //
    // Sets cmd.
    //
    // The pose is in a global coordinate frame, while the goals are
    // in the local coordinate frame.
    //
    virtual void getCommand( bool                                   stalled,
                             bool                                   localisationUncertain,
                             const orcanavutil::Pose               &pose,
                             const ISensorData                     &obs,
                             const std::vector<orcalocalnav::Goal> &goals,
                             const IStateData                      &state,
                             IControlData                          &cmd ) = 0;
                             
    // queries the driver for the required sensor model type
    virtual SensorModelType sensorModelType()=0;
    // queries the driver for the required control type
    virtual ControlType controlType()=0;
    
    // tell the driver the sensor description
    virtual void setSensorModelDescription( ISensorDescription& descr )=0;
    // tell the driver the vehicle description
    virtual void setVehicleDescription( orca::VehicleDescription& descr )=0;
    
    // once all the sensor, control, and state info have been set up, 
    // print out the configuration
    virtual void printConfiguration()=0;


protected: 

};

// Helper class to instantiate drivers
class DriverFactory {
public:
    virtual ~DriverFactory() {};
    virtual IDriver *createDriver( const orcaice::Context &context ) const=0;
                                   // const orca::VehicleDescription &descr ) const=0;
};

} // namespace

// Function for dynamically instantiating drivers.
// A driver must have a function like so:
// extern "C" {
//     localnav::DriverFactory *createDriverFactory();
// }
typedef localnav::DriverFactory *DriverFactoryMakerFunc();

#endif
