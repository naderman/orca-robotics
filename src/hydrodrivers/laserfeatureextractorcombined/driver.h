/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef LFECOMBINED_DRIVER_H
#define LFECOMBINED_DRIVER_H

#include <memory>
#include <hydrointerfaces/laserfeatureextractor.h>
#include <hydrolaserfeatureextract/hydrolaserfeatureextract.h>

namespace lfecombined
{

class Driver: public hydrointerfaces::LaserFeatureExtractor
{

public:

    Driver( const hydroscanutil::ScannerConfig &scannerConfig,
            const hydroutil::Context &context );

    std::vector<hydrofeatureobs::FeatureObs*> 
    extractFeatures( const std::vector<float>         &ranges,
                     const std::vector<unsigned char> &intensities );
    
private:

    std::auto_ptr<hydrolfextract::ReflectorExtractor>         reflectorExtractor_;
    std::auto_ptr<hydrolfextract::ReflectorFeatureExtractor>  reflectorFeatureExtractor_;

    std::auto_ptr<hydrolfextract::ForegroundExtractor>        foregroundExtractor_;
    std::auto_ptr<hydrolfextract::ForegroundFeatureExtractor> foregroundFeatureExtractor_;

    std::auto_ptr<hydrolfextract::DoorExtractor>              doorExtractor_;
    std::auto_ptr<hydrolfextract::DoorFeatureExtractor>       doorFeatureExtractor_;

    std::auto_ptr<hydrolfextract::LineExtractor>              lineExtractor_;
    std::auto_ptr<hydrolfextract::LineFeatureExtractor>       lineFeatureExtractor_;
    std::auto_ptr<hydrolfextract::CornerFeatureExtractor>     cornerFeatureExtractor_;
};

//////////////////////////////////////////////////////////////////////

hydrolfextract::ReflectorExtractor *
getReflectorExtractor( const hydroscanutil::ScannerConfig &scannerConfig,
                       const hydroutil::Context &context );
hydrolfextract::ReflectorFeatureExtractor *
getReflectorFeatureExtractor( const hydrolfextract::ReflectorExtractor::Config &cfg,
                              double rangeSd,
                              double bearingSd,
                              const hydroutil::Context &context );

hydrolfextract::ForegroundExtractor *
getForegroundExtractor( const hydroscanutil::ScannerConfig &scannerConfig,
                        const hydroutil::Context &context );
hydrolfextract::ForegroundFeatureExtractor *
getForegroundFeatureExtractor( const hydrolfextract::ForegroundExtractor::Config &cfg,
                               double rangeSd,
                               double bearingSd,
                               const hydroutil::Context &context );

hydrolfextract::DoorExtractor *
getDoorExtractor( const hydroscanutil::ScannerConfig &scannerConfig,
                  const hydroutil::Context &context );
hydrolfextract::DoorFeatureExtractor *
getDoorFeatureExtractor( const hydrolfextract::DoorExtractor::Config &cfg,
                         double rangeSd,
                         double bearingSd,
                         const hydroutil::Context &context );

hydrolfextract::LineExtractor *
getLineExtractor( const hydroscanutil::ScannerConfig &scannerConfig,
                  const hydroutil::Context &context );
hydrolfextract::LineFeatureExtractor *
getLineFeatureExtractor( const hydrolfextract::LineExtractor::Config &cfg,
                         double rangeSd,
                         double bearingSd,
                         const hydroutil::Context &context );
hydrolfextract::CornerFeatureExtractor *
getCornerFeatureExtractor( const hydrolfextract::LineExtractor::Config &cfg,
                           double rangeSd,
                           double bearingSd,
                           const hydroutil::Context &context );

//////////////////////////////////////////////////////////////////////

// Used for dynamically loading driver
class Factory : public hydrointerfaces::LaserFeatureExtractorFactory
{
public:
    hydrointerfaces::LaserFeatureExtractor*
    createDriver( double laserMaxRange,
                  double laserStartAngle,
                  double laserAngleIncrement,
                  const hydroutil::Context &context ) const
    {
        hydroscanutil::ScannerConfig scannerConfig( laserMaxRange, laserStartAngle, laserAngleIncrement );
        return new Driver( scannerConfig, context );
    }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    hydrointerfaces::LaserFeatureExtractorFactory *createDriverFactory();
}

#endif
