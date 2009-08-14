/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef HYDRO_INTERFACES_LASERSCANNER2D_H
#define HYDRO_INTERFACES_LASERSCANNER2D_H

#include <string>
#include <vector>
#include <hydroutil/context.h>

namespace hydrointerfaces 
{

/*!
    @ingroup hydro_drivers
    @defgroup hydro_drivers_laserscanner2d LaserScanner2d drivers
    @brief Implementations of LaserScanner2d interface
*/
/*!
    @ingroup hydro_interfaces
    @defgroup hydro_interface_laserscanner2d LaserScanner2d
    @brief 2D laser scanner hardware.

    @{
*/

//!
//! @brief Abstract class for a scanning laser.
//! 
//! All laser scanners should implement this, making it simple
//! to add support for different laser scanners.
//! 
//! This guy is @b not guaranteed to be thread-safe!
//!
//! @author Alex Brooks a.brooks at cas.edu.au
//!
class SOEXPORT LaserScanner2d 
{

public:

    //! Configuration structure
    class Config
    {   
    public:
        Config();
        bool validate() const;
        std::string toString() const;
        bool operator==( const Config & other );
        bool operator!=( const Config & other );

        //! minimum range [m]
        double minRange;
        //! maximum range [m]
        double maxRange;
        //! field of view [rad]
        double fieldOfView;
        //! starting angle [rad]
        double startAngle;
        //! number of samples in a scan
        int    numberOfSamples;
    };

    //! Data structure returned by read()
    class Data
    {
    public:
        Data()
            : haveWarnings(false)
            {}

        float         *ranges;
        unsigned char *intensities;
        int            timeStampSec;
        int            timeStampUsec;
        bool           haveWarnings;
        //! if 'haveWarnings' is set, 'warnings' will contain diagnostic information.
        std::string    warnings;
    };

    virtual ~LaserScanner2d() {};

    //! Blocks till new data is available, but shouldn't occupy the thread indefinitely.
    //! Ranges and intensities can be expected to have been pre-sized correctly.
    //! Throws exceptions on un-recoverable faults.
    virtual void read( Data &data )=0;

private:

};

//! Helper class to instantiate drivers
class SOEXPORT LaserScanner2dFactory {
public:
    virtual ~LaserScanner2dFactory() {};
    //! Instantiates a driver and returns a pointer to it.
    virtual LaserScanner2d *createDriver( const LaserScanner2d::Config &config,
                                          const hydroutil::Context     &context ) const=0;
};

/*! @} */
} // namespace

// Function for dynamically instantiating drivers.
// A driver must have a function like so:
// extern "C" {
//     hydrointerfaces::LaserScanner2dFactory *createDriverFactory();
// }
typedef hydrointerfaces::LaserScanner2dFactory *DriverFactoryMakerFunc();

#endif
