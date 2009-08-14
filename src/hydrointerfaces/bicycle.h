/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_INTERFACES_BICYCLE_H
#define HYDRO_INTERFACES_BICYCLE_H

#include <hydroutil/context.h>
#include <exception>

namespace hydrointerfaces
{

/*!
    @ingroup hydro_drivers
    @defgroup hydro_drivers_bicycle Bicycle drivers
    @brief Implementations of Bicycle hardware interface
*/
/*!
    @ingroup hydro_interfaces
    @defgroup hydro_interface_bicycle Bicycle
    @brief Access to a generic robotic platform with bicycle steering.

    @{
*/

/*!

@brief Abstract interface class for providing access to a generic robotic platform with bicycle steering.

Member functions throw Exception on error conditions.

The implementations of this interface are @b not expected to be thread-safe.

@author Alex Makarenko, Alex Brooks, Mikel Astiz

*/
class SOEXPORT Bicycle
{

public: 

    //! %Data structure returned by read()
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
	//! Bicycle steer angle [rad]
	double steerAngle;
        //! Longitudinal velocity of the steer wheel [m/s]
        double vlong;
        //! Yaw rate [rad/s]
        double dyaw;
    
        //! Converts to a human readable string
        std::string toString() const;
    };
    
    //! Motion command structure
    class Command
    {
    public:
        //! Desired longitudinal velocity of the steer wheel [m/s]
        double vlong;
        //! Desired steer angle [rad]
        double steerAngle;
    
        //! Converts to a human readable string
        std::string toString() const;
    };

    //! Exception thrown by implementations of Bicycle interface.
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

    virtual ~Bicycle() {}

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
};

//! Helper class to instantiate drivers
class SOEXPORT BicycleFactory {
public:
    virtual ~BicycleFactory() {};
    //! Instantiates a driver and returns a pointer to it.
    virtual Bicycle *createDriver( const hydroutil::Context& context ) const=0;
};

/*! @} */
} // namespace

// Function for dynamically instantiating drivers.
// A driver must have a function like so:
// extern "C" {
//     hydrointerfaces::BicycleFactory *createDriverFactory();
// }
typedef hydrointerfaces::BicycleFactory *DriverFactoryMakerFunc();

#endif
