/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_VEHICLE_DESCRIPTION_INTERFACE_ICE
#define ORCA2_VEHICLE_DESCRIPTION_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_vehicleDescription VehicleDescription
    @brief Descriptions for various robotic vehicles
    @{
*/

/*!
For simple descriptions and pictures of different mechanisms, see
http://www-128.ibm.com/developerworks/library/wi-robot5/index.html
*/
enum VehicleControlType
{
    //! Steers like a tank by individually controlling rotational velocities of left and right wheels (or tracks). 
    //! E.g. Pioneer-2, Segway RMP.
    VehicleControlVelocityDifferential,
    //! Steering is done with a single wheel. A tricycle works the same way.
    VehicleControlVelocityBicycle,
    //! Steering is done by synchronized motion of all wheels
    VehicleControlVelocitySynchro,
    //! Steering is done by two wheels, whose steering angle may be different.
    VehicleControlVelocityAckerman,
    //! Something not listed here
    VehicleControlOther
};

//! Static description of the vehicle control mechanism.
//!
//! This may be important because some mechanisms impose certain constraints on 
//! attainable velocities.  These constraints are particular to the control
//! mechanism, and are described by the class that inherits from this.
class VehicleControlDescription
{
    //! Mechanism type. 
    VehicleControlType type;
};

//! Static description of a Velocity-controlled Differential-Drive vehicle.
class VehicleControlVelocityDifferentialDescription extends VehicleControlDescription
{
    //! Maximum in-plane forward speed [m/s]
    double maxForwardSpeed;
    //! Maximum in-plane reverse speed [m/s]
    double maxReverseSpeed;
    //! Maximum in-plane turnrate [rad/s] (assumes equal max turnrate in either direction).
    double maxTurnrate;

    //! Maximum turnrate when moving at max speed
    //! Since maxTurnrate is defined for 0m/s, these two parameters define a linear
    //! model for turnrate constraints as a function of speed.
    double maxTurnrateAtMaxSpeed;

    //! Maximum forward acceleration [m/s/s]
    double maxForwardAcceleration;
    //! Maximum reverse acceleration [m/s/s]
    double maxReverseAcceleration;
    //! Maximum rotational acceleration [rad/s/s] (assumes equal in either direction)
    double maxRotationalAcceleration;
};

//! Static description of a Velocity-controlled Bicycle-Drive vehicle.
class VehicleControlVelocityBicycleDescription extends VehicleControlDescription
{
    //! Maximum in-plane forward speed [m/s]
    double maxForwardSpeed;
    //! Maximum in-plane reverse speed [m/s]
    double maxReverseSpeed;
    //! Maximum steering angle [rad] when stationary.
    //! Assumes equal in either direction.
    double maxSteerAngle;

    //! Maximum steering angle when moving at maximum speed.
    //! Since maxSteerAngle is defined for 0m/s, these two parameters define a linear
    //! model for steering angle constraints as a function of speed.
    //! Assumes equal in either direction.
    double maxSteerAngleAtMaxSpeed;

    //! Maximum forward acceleration [m/s/s]
    double maxForwardAcceleration;
    //! Maximum reverse acceleration [m/s/s]
    double maxReverseAcceleration;
    //! Maximum rate of change of steering angle [rad/s] (assumes equal in either direction)
    double maxSteerAngleRate;
};

//! A set of of possible vehicle geometry descriptions
enum VehicleGeometryType
{
    //! Appropriate for vehicles shaped like extruded rectangles
    VehicleGeometryCuboid,
    //! Appropriate for eg. trash-can-shaped robots
    VehicleGeometryCylindrical,
    //! Something not listed here
    VehicleGeometryOther
};

//! Static description of the physical size and shape of the vehicle.
//!
//! The specifics are described in classes which inherit from this.
class VehicleGeometryDescription
{
    //! Geometry Type
    VehicleGeometryType type;
};

//! Geometry description for a vehicle which can be described by a simple
//! bounding cylinder.
class VehicleGeometryCylindricalDescription extends VehicleGeometryDescription
{
    //! Radius of the cylinder [m]
    double radius;
    //! Height of the cylinder [m]
    double height;

    //!
    //! The transformation:
    //! - from: the platform coordinate system,
    //! - to:   the coordinate system about which the vehicle geometry extends.
    //!
    //! The cylinder is assumed to be centred on the vehicle coordinate system,
    //! extruded in the z-direction 
    //! (so a slice through the vehicle-CS x-y plane is circular).
    //!
    Frame3d vehicleToGeometryTransform;
};

//! Geometry description for a vehicle which can be described by a simple
//! bounding cuboid.
class VehicleGeometryCuboidDescription extends VehicleGeometryDescription
{
    //! Size in 3 dimensions.
    Size3d size;

    //!
    //! The transformation:
    //! - from: the platform coordinate system,
    //! - to:   the coordinate system about which the vehicle geometry extends.
    //!
    //! The cylinder is assumed to be centred on the vehicle coordinate system,
    //! extruded in the z-direction 
    //! (so a slice through the vehicle-CS x-y plane is circular).
    //!
    Frame3d vehicleToGeometryTransform;
};

//! General description of a robotic vehicle.
struct VehicleDescription
{
    //! Description of the mechanism by which the vehicle can be controlled.
    //! Describes constraints on the vehicle's dynamics.
    VehicleControlDescription  control;

    //! Transformation:
    //! - from: the platform's coordinate system,
    //! - to:   the coordinate system of the vehicle.
    //!
    //! These will often be identical.  An example where they are different may be a car, where one
    //! wants the platform CS to be at the GPS antenna, but the vehicle CS to be on the rear axle.
    Frame3d                    platformToVehicleTransform;

    //! A description of the physical size and shape of the vehicle.
    VehicleGeometryDescription geometry;
};

//!  //@}
}; // module

#endif
