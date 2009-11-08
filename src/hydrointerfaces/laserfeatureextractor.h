/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef HYDRO_INTERFACES_LASERFEATUREEXTRACTOR_H
#define HYDRO_INTERFACES_LASERFEATUREEXTRACTOR_H

#include <string>
#include <vector>
#include <hydroutil/context.h>

namespace hydrofeatureobs {
    class FeatureObs;
}

namespace hydrointerfaces 
{

/*!
    @ingroup hydro_drivers
    @defgroup hydro_drivers_laserfeatureextractor LaserFeatureExtractor drivers
    @brief Implementations of LaserFeatureExtractor interface
*/
/*!
    @ingroup hydro_interfaces
    @defgroup hydro_interface_laserfeatureextractor LaserFeatureExtractor
    @brief 2D laser scanner hardware.

    @{
*/

//!
//! @brief Abstract class for a laser feature extraction algorithm.
//! 
//! @author Alex Brooks a.brooks at cas.edu.au
//!
class SOEXPORT LaserFeatureExtractor 
{

public:

    virtual ~LaserFeatureExtractor() {};

    //! Extracts features from the scan.  Memory is allocated for each feature,
    //! which must be deleted by the caller.
    virtual std::vector<hydrofeatureobs::FeatureObs*> 
        extractFeatures( const std::vector<float>         &ranges,
                         const std::vector<unsigned char> &intensities ) = 0;

private:

};

//! Helper class to instantiate drivers
class SOEXPORT LaserFeatureExtractorFactory {
public:
    virtual ~LaserFeatureExtractorFactory() {};
    //! Instantiates a driver and returns a pointer to it.
    virtual LaserFeatureExtractor *createDriver( double laserMaxRange,
                                                 double laserStartAngle,
                                                 double laserAngleIncrement,
                                                 int    laserNumReturns,
                                                 const hydroutil::Context &context ) const=0;
};

/*! @} */
} // namespace

// Function for dynamically instantiating drivers.
// A driver must have a function like so:
// extern "C" {
//     hydrointerfaces::LaserFeatureExtractorFactory *createLaserFeatureExtractorDriverFactory();
// }
typedef hydrointerfaces::LaserFeatureExtractorFactory *LaserFeatureExtractorDriverFactoryMakerFunc();

#endif
