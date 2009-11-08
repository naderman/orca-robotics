/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_INTERFACES_SEGWAYRMP_H
#define HYDRO_INTERFACES_SEGWAYRMP_H

#include <hydroutil/context.h>

namespace hydrointerfaces
{

/*!
    @ingroup hydro_drivers
    @defgroup hydro_drivers_segwayrmp SegwayRmp drivers
    @brief Implementations of SegwayRmp hardware interface
*/
/*!
    @ingroup hydro_interfaces
    @defgroup hydro_interface_segwayrmp SegwayRmp
    @brief Access to Segway's RMP robotic platform.

    @{
*/

/*!

@brief Abstract interface class for providing access to Segway's RMP robotic platform.

Member functions throw Exception on error conditions.

The implementations of this interface are @b not expected to be thread-safe.

@author Alex Makarenko, Alex Brooks

*/
class SOEXPORT SegwayRmp
{

public: 

    //! %Data structure
    class Data
    {
    public:
        //! Timestamp [s]
        int seconds;
        //! Timestamp [us]
        int useconds;
        //! x component of odometry [m]
        double x;
        //! y component of odometry [m]
        double y;
        //! Roll angle [rad]
        double roll;
        //! Pitch angle [rad]
        double pitch;
        //! Yaw angle [rad]
        double yaw;
        //! Longitudinal velocity [m/s]
        double vx;
        //! Roll rate [rad/s]
        double droll;
        //! Pitch rate [rad/s]
        double dpitch;
        //! Yaw rate [rad/s]
        double dyaw;
        //! Left motor torque [Nm]
        double leftTorque;
        //! Right motor torque [Nm]
        double rightTorque;
        //! Voltage of the main batteries (min of the two) [V]
        double mainvolt;
        //! Voltage of the UI batteries [V]
        double uivolt;

        //! Is the hardware in a non-critical non-standard state?
        bool hasWarnings;
        //! Is the hardware in a critical non-standard state?
        bool hasFaults;
        //! Describes the warnings/faults in human-readable form.
        std::string warnFaultReason;
    
        //! Converts to a human readable string
        std::string toString() const;
    };
    
    //! Motion command structure
    class Command
    {
    public:
        Command( double speed, double turnrate )
            : vx(speed), w(turnrate) {}
        Command() {}

        //! Desired longitudinal velocity [m/s]
        double vx;
        //! Desired yaw rate [rad/s]
        double w;
    
        //! Converts to a human readable string
        std::string toString() const;
    };

    //! Capabilities of the hardware
    class Capabilities
    {
    public:
        //! A positive quantity
        double maxForwardSpeed;
        //! A positive quantity
        double maxReverseSpeed;
        //! A positive quantity
        double maxTurnrate;
        //! A positive quantity
        double maxLateralAcceleration;        

        bool isSane() const;
        std::string toString() const;
    };

    virtual ~SegwayRmp() {}

    //! Initialises the hardware.
    //! Throws Exception on failure.
    //! It is not an error to call this repeatedly: repeated calls should
    //! re-initialise the hardware.
    //! (eg this will be called if the hardware reports an error condition)
    virtual void enable()=0;

    //! Blocking read.
    virtual void read( Data &data )=0;
    
    //! Writes motion command.
    virtual void write( const Command& command )=0;

    //! Read the capabilities of the hardware
    virtual Capabilities capabilities() const=0;
};

//! Helper class to instantiate drivers
class SOEXPORT SegwayRmpFactory {
public:
    virtual ~SegwayRmpFactory() {};
    //! Instantiates a driver and returns a pointer to it.
    virtual SegwayRmp *createDriver( const std::string &powerbaseName, const hydroutil::Context& context ) const=0;
};

// Streaming operators
inline std::ostream &operator<<( std::ostream &s, const SegwayRmp::Data &data )
{ return s << data.toString(); }
inline std::ostream &operator<<( std::ostream &s, const SegwayRmp::Command &cmd )
{ return s << cmd.toString(); }
inline std::ostream &operator<<( std::ostream &s, const SegwayRmp::Capabilities &c )
{ return s << c.toString(); }

//! Helper function for constraining capabilities based on some limits
//! (does a thresholding operation)
void constrain( SegwayRmp::Capabilities &capabilities, 
                const SegwayRmp::Capabilities &limits );

bool operator==( const SegwayRmp::Capabilities &c1, const SegwayRmp::Capabilities &c2 );

bool commandPossible( const hydrointerfaces::SegwayRmp::Command      &cmd,
                      const hydrointerfaces::SegwayRmp::Capabilities &capabilities );

void limit( hydrointerfaces::SegwayRmp::Command            &cmd,
            const hydrointerfaces::SegwayRmp::Capabilities &capabilities );

/*! @} */
} // namespace

// Function for dynamically instantiating drivers.
// A driver must have a function like so:
// extern "C" {
//     hydrointerfaces::SegwayRmpFactory *createSegwayRmpDriverFactory();
// }
typedef hydrointerfaces::SegwayRmpFactory *SegwayRmpDriverFactoryMakerFunc();

#endif
