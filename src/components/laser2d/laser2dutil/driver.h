/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_LASER2D_HW_DRIVER_H
#define ORCA2_LASER2D_HW_DRIVER_H

#include <string>
#include <orca/orca.h>
#include <orcaice/context.h>

namespace laser2d {

//!
//! @brief Abstract class for a scanning laser.
//! 
//! All laser scanners should implement this, making it simple
//! to add support for different laser scanners.
//! 
//! This guy is _not_ guaranteed to be thread-safe!
//!
//! Note: Driver should _not_ throw exceptions from the constructor.
//!
//! @author Alex Brooks a.brooks at cas.edu.au
//!
class Driver 
{

public:

    class Config
    {   
    public:
        Config();
        bool validate() const;
        std::string toString() const;
        bool operator==( const Config & other );
        bool operator!=( const Config & other );

        double minRange;
        double maxRange;
        double fieldOfView;
        double startAngle;
        int    numberOfSamples;
    };

    virtual ~Driver() {};

    // Blocks till new data is available, but shouldn't occupy the thread indefinitely.
    // ranges and intensities are pre-sized correctly.
    virtual void read( std::vector<float> &ranges, 
                       std::vector<unsigned char> &intensities, 
                       orca::Time &timeStamp )=0;

private:

};

// Helper class to instantiate drivers
class DriverFactory {
public:
    virtual ~DriverFactory() {};
    virtual Driver *createDriver( const Driver::Config &config,
                                  const orcaice::Context &context ) const=0;
};

} // namespace

// Function for dynamically instantiating drivers.
// A driver must have a function like so:
// extern "C" {
//     laser2d::DriverFactory *createDriverFactory();
// }
typedef laser2d::DriverFactory *DriverFactoryMakerFunc();

#endif
