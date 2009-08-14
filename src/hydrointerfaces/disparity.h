/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef HYDRO_INTERFACES_DISPARITY_H
#define HYDRO_INTERFACES_DISPARITY_H

#include <string>
#include <vector>
#include <hydroutil/context.h>
#include <hydroimage/structures.h>

namespace hydrointerfaces 
{

/*!
    @ingroup hydro_drivers
    @defgroup hydro_drivers_disparity Disparity drivers
    @brief Implementations of the Disparity interface
*/
/*!
    @ingroup hydro_interfaces
    @defgroup hydro_interface_disparity Disparity 
    @brief Disparity.

    @{
*/

//!
//! @brief Abstract class for a stereo image correspondance, or disparity, source.
//! 
//! This guy is @b not guaranteed to be thread-safe!
//!
//! @author Tom Burdick thomas.burdick at gmail.com
//!
class SOEXPORT Disparity 
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

        //! image width [pixels]
        size_t width;
        //! image height [pixels]
        size_t height;
        //! image byte size [bytes]
        size_t size;
        //! number of shifts to compute
        size_t shifts;
        //! offset to start computing shifts at
        size_t offset;
        //! image format string
        std::string format;
    };

    //! Data structure returned by process()
    typedef hydroimage::Data Data;

    virtual ~Disparity() {};

    //! Thread safe blocking call, implementation specific as to how this is done.
    //! Images can be expected to have been pre-sized correctly.
    //! Throws exceptions on un-recoverable faults.
    virtual void process( const Data& leftImage, const Data& rightImage, Data& outputImage )=0;

    //! Some implementations need an event loop (OpenGL GPGPU implementations) 
    //! This returns true if executeEventLoop needs to be called in the main thread of the program
    virtual bool hasEventLoop() = 0;

    //! Execute the event loop
    virtual int executeEventLoop() = 0;

private:

};

//! Helper class to instantiate drivers
class SOEXPORT DisparityFactory {
public:
    virtual ~DisparityFactory() {};
    //! Instantiates a driver and returns a pointer to it.
    virtual Disparity *createDriver( const Disparity::Config &config
				   , const hydroutil::Context &context ) const=0;
};

/*! @} */
} // namespace

// Function for dynamically instantiating drivers.
// A driver must have a function like so:
// extern "C" {
//     hydrointerfaces::MultiImageFactory *createDriverFactory();
// }
typedef hydrointerfaces::DisparityFactory *DriverFactoryMakerFunc();

#endif
