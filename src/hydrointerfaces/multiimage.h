/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef HYDRO_INTERFACES_MULTIIMAGE_H
#define HYDRO_INTERFACES_MULTICOLLECTION_H

#include <string>
#include <vector>
#include <hydroutil/context.h>

#include <hydroimage/structures.h>

namespace hydrointerfaces 
{

/*!
    @ingroup hydro_drivers
    @defgroup hydro_drivers_multiimage MultiImage drivers
    @brief Implementations of MultiImage interface
*/
/*!
    @ingroup hydro_interfaces
    @defgroup hydro_interface_multiimage MultiImage 
    @brief MultiImage.

    @{
*/

//!
//! @brief Abstract class for an multiimage source.
//! 
//! All image sources that require multiple images to 
//! be processed at the same time should implement this, 
//! making it simple to add support for different 
//! image collection sources. Used for things such as
//! stereo cameras like the bumblebee2
//! 
//! This guy is @b not guaranteed to be thread-safe!
//!
//! @author Tom Burdick thomas.burdick at gmail.com
//!
class SOEXPORT MultiImage
{

public:

    typedef std::vector<hydroimage::Config> Config;

    typedef std::vector<hydroimage::Data> Data;

    virtual ~MultiImage() {};

    //! Blocks till new data is available, but shouldn't occupy the thread indefinitely.
    //! Images can be expected to have been pre-sized correctly.
    //! Throws exceptions on un-recoverable faults.
    virtual void read( MultiImage::Data &data )=0;

private:

};

//! Helper class to instantiate drivers
class SOEXPORT MultiImageFactory {
public:
    virtual ~MultiImageFactory() {};
    //! Instantiates a driver and returns a pointer to it.
    virtual MultiImage *createDriver( MultiImage::Config &config,
                                  const hydroutil::Context &context ) const=0;
};

/*! @} */
} // namespace

// Function for dynamically instantiating drivers.
// A driver must have a function like so:
// extern "C" {
//     hydrointerfaces::MultiImageFactory *createMultiImageDriverFactory();
// }
typedef hydrointerfaces::MultiImageFactory *MultiImageDriverFactoryMakerFunc();

#endif
