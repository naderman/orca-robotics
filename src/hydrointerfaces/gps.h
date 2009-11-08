/*
 * Orca-Robotics Project: Components for robotics
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Michael Moser
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_INTERFACES_GPS_H
#define HYDRO_INTERFACES_GPS_H

#include <cstdlib>
#include <string>
#include <memory>
#include <vector>

#include <hydroutil/context.h>


namespace hydrointerfaces
{

/*!
    @ingroup hydro_drivers
    @defgroup hydro_drivers_gps Gps drivers
    @brief Implementations of Gps interface
*/
/*!
    @ingroup hydro_interfaces
    @defgroup hydro_interface_gps Gps
    @brief Access to a GPS device.

    INS stands for inertial navigation system; basically it works by measuring the acceleration
    and turnrate vectors with an IMU (inertial measurement unit) and (double) integrating those
    to yield position velocity and attitude (PVA). To compensate for unbounded (quadratic) error growth
    it is often combined with a global sensor, e.g. GPS.
    Such a combined system ideally yields a locally smooth, globally correct PVA solution.

    @{
*/
//!
//! @brief Abstract class for a GPS system
//! 
//! All drivers should implement this, making it simple
//! to add support for different drivers.
//!
//! Initialization is assumed to be performed in the constructor.
//! 
//! This guy is @b not guaranteed to be thread-safe!
//!
//! @author Matthew Ridley, Tobias Kaupp, Alex Makarenko
//!
class SOEXPORT Gps
{

public:

    //! General configuration of a Gps system; empty for now
    class Config
    {
    public:
        Config() {};
        bool isValid() const { return true; };
        std::string toString() const { return std::string(); };
        //bool operator==( const Config & other );
        //bool operator!=( const Config & other );
    };

    //! Gps position types.
    //! Using Novatel codes here is probably not the best thing. With more
    //! thought it's probably possible to categorize these position types
    //! into more generic categories. For now, non-Novatel receivers should
    //! use the generic types listed first.
    enum PositionType {
        //! Invalid or not available
        GpsPositionTypeNotAvailable,
        //! Autonomous position
        //! (This is the normal case for non-differential GPS)
        GpsPositionTypeAutonomous,
        //! Differentially corrected
        GpsPositionTypeDifferential,
        NovatelNone,
        NovatelFixedPos,
        NovatelFixedHeigth,
        NovatelFloatConv,
        NovatelWideLane,
        NovatelNarrowLane,
        NovatelDopplerVelocity,
        NovatelSingle,
        NovatelPsrDiff,
        NovatelWAAS,
        NovatelPropagated,
        NovatelOmnistar,
        NovatelL1Float,
        NovatelIonFreeFloat,
        NovatelNarrowFloat,
        NovatelL1Int,
        NovatelWideInt,
        NovatelNarrowInt,
        NovatelRTKDirectINS,
        NovatelINS,
        NovatelINSPSRSP,
        NovatelINSPSRFLOAT,
        NovatelINSRTKFLOAT,
        NovatelINSRTKFIXED,
        NovatelOmnistarHP,
        NovatelUnknown
    };
    
    //! Gps data structure
    struct Data
    {
        //! Time (according to the computer clock) when data was measured.
        //! Number of seconds
        int timeStampSec;
        //! Time (according to the computer clock) when data was measured.
        //! Number of microseconds
        int timeStampUsec;
        //! UTC time (according to GPS device), reference is Greenwich.
        //! Hour [0..23]
        int utcTimeHrs;
        //! UTC time (according to GPS device), reference is Greenwich.
        //! Minutes [0..59]
        int utcTimeMin;
        //! UTC time (according to GPS device), reference is Greenwich.
        //! Seconds [0.0..59.9999(9)]
        double utcTimeSec;
    
        //! Latitude (degrees)
        double latitude;
        //! Longitude (degrees)
        double longitude;
        //! Altitude (metres above ellipsoid)
        double altitude;
        
        //! Horizontal position error: one standard deviation (metres)
        double horizontalPositionError;
        //! Tells us whether the vertical position error can be estimated
        bool isVerticalPositionErrorValid;
        //! Vertical position error: one standard deviation (metres) (see verticalPositionError)
        double verticalPositionError;
        
        //! Heading/track/course with respect to true north (rad)
        double heading; 
        //! Horizontal velocity (metres/second)
        double speed;
        //! Vertical velocity (metres/second)
        double climbRate;
        
        //! Number of satellites
        int satellites;
        int observationCountOnL1;
        int observationCountOnL2;
        //! Position type (see above)
        PositionType positionType;
        //! Geoidal Separation (metres)
        double geoidalSeparation;    
    };

    //! Reads from GPS, blocks till timout expires
    //! May throw GpsException
    virtual void read( Data &data )=0;

    virtual ~Gps() {};

private:

};

//! Helper class to instantiate drivers
class SOEXPORT GpsFactory {
public:
    virtual ~GpsFactory() {};
    //! Instantiates a driver and returns a pointer to it.
    virtual Gps *createDriver( const Gps::Config &config,
                                  const hydroutil::Context &context ) const=0;
};

/*! @} */
} // namespace

// Function for dynamically instantiating drivers.
// A driver must have a function like so:
// extern "C" {
//     hydrointerfaces::GpsFactory *GpsFactory();
// }
typedef hydrointerfaces::GpsFactory *GpsDriverFactoryMakerFunc();

#endif
