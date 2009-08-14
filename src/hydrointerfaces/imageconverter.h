/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef HYDRO_INTERFACES_IMAGECONVERTER_H
#define HYDRO_INTERFACES_IMAGECONVERTER_H

#include <string>
#include <vector>
#include <hydroutil/context.h>

namespace hydrointerfaces 
{

/*!
    @ingroup hydro_drivers
    @defgroup hydro_drivers_imageconverter ImageConverter drivers
    @brief Implementations of ImageConverter interface
*/
/*!
    @ingroup hydro_interfaces
    @defgroup hydro_interface_imageconverter ImageConverter 
    @brief ImageConverter.

    @{
*/

//!
//! @brief Abstract class for a imageconverter source.
//! 
//! All imageconverters should implement this, making it simple
//! to add support for different imageconverters.
//! 
//! This guy is @b not guaranteed to be thread-safe!
//!
//! @author Tom Burdick thomas.burdick at gmail.com
//!
class SOEXPORT ImageConverter 
{

public:

    //! define the config object as hydroimage::Config
    typedef hydroimage::Config Config;

    //! define the data object as hydroimage::Data
    typedef hydroimage::Data Data;
    
    virtual ~ImageConverter() {};

    //! A blocking call that converts the inputData to the desired format of the outputData. 
    //! The ImageConverter can be expected to have been pre-sized correctly.
    //! Throws exceptions on un-recoverable faults.
    virtual void convert( Data &inputData, Data &outputData )=0;
   
    //! determine the buffer size for a given input config and desired output format
    virtual uint32_t getOutputBufferSize(const Config& config, const std::string& outputFormat) = 0;

    //! describe the input and output formats for this class
    virtual const std::vector<std::string>& getInputFormats() const = 0;
    virtual const std::vector<std::string>& getOutputFormats(const std::string &inputFormat) const = 0;

private:

};

//! Helper class to instantiate drivers
class SOEXPORT ImageConverterFactory {
public:
    virtual ~ImageConverterFactory() {};
    //! Instantiates a driver and returns a pointer to it.
    virtual ImageConverter *createDriver( const hydroutil::Context &context ) const=0;
};

/*! @} */
} // namespace

// Function for dynamically instantiating drivers.
// A driver must have a function like so:
// extern "C" {
//     hydrointerfaces::ImageConverterFactory *createDriverFactory();
// }
typedef hydrointerfaces::ImageConverterFactory *DriverFactoryMakerFunc();

#endif
