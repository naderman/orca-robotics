/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef HYDRO_INTERFACES_IMAGE_H
#define HYDRO_INTERFACES_IMAGE_H

#include <string>
#include <vector>
#include <hydroutil/context.h>

#include <hydroimage/structures.h>
#include <hydroimage/formats.h>

namespace hydrointerfaces 
{

/*!
    @ingroup hydro_drivers
    @defgroup hydro_drivers_image Image drivers
    @brief Implementations of Image interface
*/
/*!
    @ingroup hydro_interfaces
    @defgroup hydro_interface_image Image 
    @brief Image.

    @{
*/

//!
//! @brief Abstract class for a image source.
//! 
//! All image sources should implement this, making it simple
//! to add support for different image sources.
//! 
//! This guy is @b not guaranteed to be thread-safe!
//!
//! @author Tom Burdick thomas.burdick at gmail.com
//!
class SOEXPORT Image 
{

public:
    typedef hydroimage::Config Config;
    typedef hydroimage::Data Data;
    typedef std::vector<hydroimage::ImageFormat> FormatVector;


    virtual ~Image() {};

    //! Blocks till new data is available, but shouldn't occupy the thread indefinitely.
    //! The Image can be expected to have been pre-sized correctly.
    //! Throws exceptions on un-recoverable faults.
    virtual void read( Data &data )=0;

private:

};

//! Helper class to instantiate drivers
class SOEXPORT ImageFactory {
public:
    virtual ~ImageFactory() {};
    //! Instantiates a driver and returns a pointer to it.
    //! Some drivers may alter the config given and merely relay warnings.
    virtual Image *createDriver( Image::Config &config,
                                  const hydroutil::Context &context ) const=0;
};

/*! @} */
} // namespace

// Function for dynamically instantiating drivers.
// A driver must have a function like so:
// extern "C" {
//     hydrointerfaces::ImageFactory *createDriverFactory();
// }
typedef hydrointerfaces::ImageFactory *DriverFactoryMakerFunc();

#endif
