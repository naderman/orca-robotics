/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef HYDRO_INTERFACES_RANGERARRAY_H
#define HYDRO_INTERFACES_RANGERARRAY_H

#include <string>
#include <vector>
#include <hydroutil/context.h>
#include <hydrobros1/types.h>

namespace hydrointerfaces 
{

/*!
    @ingroup hydro_drivers
    @defgroup hydro_drivers_rangerarray RangerArray drivers
    @brief Implementations of RangerArray interface
*/
/*!
    @ingroup hydro_interfaces
    @defgroup hydro_interface_rangerarray RangerArray
    @brief Array of ranger devices with multi-origin support

    @{
*/

//!
//! @brief Abstract class for a multi-origin ranger array.
//! 
//! All drivers should implement this, making it simple
//! to add support for different devices.
//! 
//! This guy is @b not guaranteed to be thread-safe!
//!
//! @author Mikel Astiz mastiz at fatronik.com
//!
class SOEXPORT RangerArray 
{

public:

  //! Description structure returned by getDescription()
  class Description
  {
  public:
    //! minimum range [m]
    double minRange;
    //! maximum range [m]
    double maxRange;
    //! detection cone width (field of view) of every ranger [rad]
    double detectionConeAngle;
    //! position and heading of every ranger
    std::vector<hydrobros1::Frame3d> poses;
  };

  //! Data structure returned by read()
  class Data
  {
  public:
  Data()
    : haveWarnings(false)
      {}

    //! data for every ranger: note that each of them has its own timestamp
    float              *ranges;
    int                *timeStampsSec;
    int                *timeStampsUsec;
    bool               haveWarnings;
    //! if 'haveWarnings' is set, 'warnings' will contain diagnostic information.
    std::string        warnings;
  };

  virtual ~RangerArray() {};
  
  //! Returns description information
  virtual void getDescription( Description &description )=0;

  //! Blocks till new data is available, but shouldn't occupy the thread indefinitely.
  //! Ranges can be expected to have been pre-sized correctly.
  //! Throws exceptions on un-recoverable faults.
  virtual void read( Data &data )=0;

private:

};

//! Helper class to instantiate drivers
class SOEXPORT RangerArrayFactory {
public:
  virtual ~RangerArrayFactory() {};
  //! Instantiates a driver and returns a pointer to it.
  virtual RangerArray *createDriver( const hydroutil::Context     &context ) const=0;
};

/*! @} */
} // namespace

// Function for dynamically instantiating drivers.
// A driver must have a function like so:
// extern "C" {
//     hydrointerfaces::RangerArrayFactory *createRangerArrayDriverFactory();
// }
typedef hydrointerfaces::RangerArrayFactory *RangerArrayDriverFactoryMakerFunc();

#endif
