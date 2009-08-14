/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_INTERFACES_ROBOT2D_H
#define HYDRO_INTERFACES_ROBOT2D_H

#include <hydroutil/context.h>
#include <exception>

namespace hydrointerfaces
{

/*!
    @ingroup hydro_drivers
    @defgroup hydro_drivers_robot2d Robot2d drivers
    @brief Implementations of Robot2d hardware interface
*/
/*!
    @ingroup hydro_interfaces
    @defgroup hydro_interface_robot2d Robot2d
    @brief Access to a generic 2D robotic platform.

    @{
*/

/*!

@brief Abstract interface class for providing access to a generic 2D robotic platform.

Member functions throw Exception on error conditions.

The implementations of this interface are @b not expected to be thread-safe.

@author Alex Makarenko, Alex Brooks

*/
class SOEXPORT Robot2d
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
        //! Yaw angle [rad]
        double yaw;
        //! Longitudinal velocity [m/s]
        double vlong;
        //! Transverse velocity [m/s]
        double vtransverse;
        //! Yaw rate [rad/s]
        double dyaw;
    
        //! Converts to a human readable string
        std::string toString() const;
    };
    
    //! Motion command structure
    class Command
    {
    public:
        //! Desired longitudinal velocity [m/s]
        double vlong;
        //! Desired transverse velocity [m/s]
        double vtransverse;
        //! Desired yaw rate [rad/s]
        double dyaw;
    
        //! Converts to a human readable string
        std::string toString() const;
    };

    //! Exception thrown by implementations of Robot2d interface.
    class Exception : public std::exception
    {
    public:
        //! Constructor with message
        Exception(const char *message)
            : message_(message) {}
        //! Constructor with message
        Exception(const std::string &message)
            : message_(message) {}
    
        virtual ~Exception() throw() {}
        //! Returns the message.
        virtual const char* what() const throw() { return message_.c_str(); }
    
    protected:
    
        std::string  message_;
    };

    virtual ~Robot2d() {}

    //! Throws Exception on failure.
    //! It is not an error to call this repeatedly: repeated calls should
    //! re-initialise the hardware.
    //! (eg this will be called if the hardware reports an error condition)
    virtual void enable()=0;

    //! Blocking read.
    //! Returns: 
    //!   true:  important change in internal state occured (details can be read with getStatus).
    //!   false: no important change
    virtual bool read( Data &data )=0;
    
    //! Writes motion command.
    virtual void write( const Command& command )=0;

    //! Get information about the current status of the driver.
    //! the string 'status' is a human-readable description.
    //! Note that there are two ways for faults to be notified:
    //!  - This function tells of hardware faults reported normally by the hardware.
    //!  - Exceptions can be thrown from read/write for non-recoverable faults, such as inability to
    //!    communicate with the hardware.
    virtual void getStatus( std::string &status, bool &isWarn, bool &isFault )=0;

    //! Modifies the arguments, thresholding to match what this hardware can actually do.
    virtual void applyHardwareLimits( double &maxForwardSpeed, double &maxReverseSpeed, 
                              double &maxTurnrate, double &maxTurnrateAtMaxSpeed )=0;

};

//! Helper class to instantiate drivers
class SOEXPORT Robot2dFactory {
public:
    virtual ~Robot2dFactory() {};
    //! Instantiates a driver and returns a pointer to it.
    virtual Robot2d *createDriver( const hydroutil::Context& context ) const=0;
};

/*! @} */
} // namespace

// Function for dynamically instantiating drivers.
// A driver must have a function like so:
// extern "C" {
//     hydrointerfaces::Robot2dFactory *createDriverFactory();
// }
typedef hydrointerfaces::Robot2dFactory *DriverFactoryMakerFunc();

#endif
