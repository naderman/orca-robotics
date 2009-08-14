/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include "driver.h"
#include <hydroutil/hydroutil.h>
#include <sstream>
#include <gbxutilacfr/exceptions.h>

using namespace std;

namespace lfecombined {

////////////////////////////////////////////////////////////////////////////////

hydrolfextract::ReflectorExtractor *
getReflectorExtractor( const hydroscanutil::ScannerConfig &scannerConfig,
                       const hydroutil::Context &context )
{
    hydrolfextract::ReflectorExtractor::Config cfg(scannerConfig);

    std::string prefix = "Reflectors.";
    hydroutil::Properties prop = context.properties();

    cfg.maxDeltaRangeWithinReflector      =
        prop.getPropertyAsDoubleWithDefault( prefix+"MaxDeltaRangeWithinReflector", 0.2 );
    cfg.maxDeltaRangeNearReflector        =
        prop.getPropertyAsDoubleWithDefault( prefix+"MaxDeltaRangeNearReflector", 0.3 );
    cfg.minReflectorBrightness            =
        prop.getPropertyAsIntWithDefault(    prefix+"MinBrightness", 1);
    return new hydrolfextract::ReflectorExtractor( cfg );
}
hydrolfextract::ReflectorFeatureExtractor *
getReflectorFeatureExtractor( const hydrolfextract::ReflectorExtractor::Config &cfg,
                              double rangeSd,
                              double bearingSd,
                              const hydroutil::Context &context )
{
    std::string prefix = "Reflectors.";
    hydroutil::Properties prop = context.properties();    

    double pFalsePositive           =
        prop.getPropertyAsDoubleWithDefault(    prefix+"PFalsePositive", 0.1);
    double pTruePositive            =
        prop.getPropertyAsDoubleWithDefault(    prefix+"PTruePositive", 0.6);

    return new hydrolfextract::ReflectorFeatureExtractor( cfg, rangeSd, bearingSd, pFalsePositive, pTruePositive );
}

// ................................................................................

hydrolfextract::ForegroundExtractor *
getForegroundExtractor( const hydroscanutil::ScannerConfig &scannerConfig,
                        const hydroutil::Context &context )
{
    hydrolfextract::ForegroundExtractor::Config cfg(scannerConfig);

    std::string prefix = "FGPoints.";
    hydroutil::Properties prop = context.properties();

    cfg.minForegroundWidth                =
        prop.getPropertyAsDoubleWithDefault( prefix+"MinForegroundWidth", 0.1);
    cfg.maxForegroundWidth                =
        prop.getPropertyAsDoubleWithDefault( prefix+"MaxForegroundWidth", 0.5);
    cfg.minForegroundBackgroundSeparation =
        prop.getPropertyAsDoubleWithDefault( prefix+"MinForegroundBackgroundSeparation", 0.5);
    cfg.clearForegroundBackgroundSeparation =
        prop.getPropertyAsDoubleWithDefault( prefix+"ClearForegroundBackgroundSeparation", 4.0);
    cfg.reliableScannerRange =
        prop.getPropertyAsDoubleWithDefault( prefix+"ReliableScannerRange", 5.0);

    return new hydrolfextract::ForegroundExtractor( cfg );
}
hydrolfextract::ForegroundFeatureExtractor *
getForegroundFeatureExtractor( const hydrolfextract::ForegroundExtractor::Config &cfg,
                               double rangeSd,
                               double bearingSd,
                               const hydroutil::Context &context )
{
    std::string prefix = "FGPoints.";
    hydroutil::Properties prop = context.properties();

    double pFalsePositive =
        prop.getPropertyAsDoubleWithDefault( prefix+"PFalsePositive", 0.4);
    double pTruePositive =
        prop.getPropertyAsDoubleWithDefault( prefix+"PTruePositive", 0.5);

    return new hydrolfextract::ForegroundFeatureExtractor( cfg,
                                                           rangeSd,
                                                           bearingSd,
                                                           pFalsePositive,
                                                           pTruePositive );
}

// ................................................................................

hydrolfextract::DoorExtractor *
getDoorExtractor( const hydroscanutil::ScannerConfig &scannerConfig,
                  const hydroutil::Context &context )
{
    hydrolfextract::DoorExtractor::Config cfg(scannerConfig);

    std::string prefix = "Doors.";
    hydroutil::Properties prop = context.properties();

    cfg.minDoorWidth = prop.getPropertyAsDoubleWithDefault( prefix+"MinDoorWidth", 0.9 );
    cfg.maxDoorWidth = prop.getPropertyAsDoubleWithDefault( prefix+"MaxDoorWidth", 1.2 );
    
    cfg.minDeltaRangeThruDoor = prop.getPropertyAsDoubleWithDefault( prefix+"MinDeltaRangeThruDoor", 2.0 );

    cfg.minReturnsThruDoor = prop.getPropertyAsIntWithDefault( prefix+"MinReturnsThruDoor", 3 );
    cfg.minReturnsBesideDoor = prop.getPropertyAsIntWithDefault( prefix+"MinReturnsBesideDoor", 2 );

    cfg.maxDeltaRangeWallBesideDoor = prop.getPropertyAsDoubleWithDefault( prefix+"MaxDeltaRangeWallBesideDoor", 0.3 );
            
    return new hydrolfextract::DoorExtractor( cfg, context );
}
hydrolfextract::DoorFeatureExtractor *
getDoorFeatureExtractor( const hydrolfextract::DoorExtractor::Config &cfg,
                         double rangeSd,
                         double bearingSd,
                         const hydroutil::Context &context )
{
    std::string prefix = "Doors.";
    hydroutil::Properties prop = context.properties();

    const double P_TRUE_POSITIVE  = 0.6;
    const double P_FALSE_POSITIVE = 0.3;
    double pTruePositive  = P_TRUE_POSITIVE;
    double pFalsePositive = P_FALSE_POSITIVE;

    return new hydrolfextract::DoorFeatureExtractor( cfg,
                                                     rangeSd,
                                                     bearingSd,
                                                     pFalsePositive,
                                                     pTruePositive );
}

// ................................................................................

hydrolfextract::LineExtractor *
getLineExtractor( const hydroscanutil::ScannerConfig &scannerConfig,
                  const hydroutil::Context &context )
{
    hydrolfextract::LineExtractor::Config cfg( scannerConfig );

    std::string prefix = "Lines.";
    hydroutil::Properties prop = context.properties();

    cfg.maxDeltaRangeInCluster = prop.getPropertyAsDoubleWithDefault( prefix+"MaxDeltaRangeInCluster", 0.5 );
    cfg.maxDeltaInLine         = prop.getPropertyAsDoubleWithDefault( prefix+"MaxDeltaInLine", 0.2 );
    cfg.minPointsInLine      = prop.getPropertyAsIntWithDefault( prefix+"MinPointsInLine", 6 );
    cfg.minLineLengthForVisibleEndpoints = prop.getPropertyAsDoubleWithDefault( prefix+"MinLineLengthForVisibleEndpoints", 0.5 );

    return new hydrolfextract::LineExtractor( cfg, context );
}
hydrolfextract::LineFeatureExtractor *
getLineFeatureExtractor( const hydrolfextract::LineExtractor::Config &cfg,
                         double rangeSd,
                         double bearingSd,
                         const hydroutil::Context &context )
{
    std::string prefix = "Lines.";
    hydroutil::Properties prop = context.properties();

    double minLineLengthForLineFeature = prop.getPropertyAsDoubleWithDefault( prefix+"MinLineLengthForLineFeature", 1.0 );
    double pFalsePositive   = prop.getPropertyAsDoubleWithDefault( prefix+"PFalsePositive", 0.4 );
    double pTruePositive = prop.getPropertyAsDoubleWithDefault( prefix+"PTruePositive", 0.6 );

    return new hydrolfextract::LineFeatureExtractor( minLineLengthForLineFeature,
                                                     rangeSd,
                                                     bearingSd,
                                                     pFalsePositive,
                                                     pTruePositive );
}
hydrolfextract::CornerFeatureExtractor *
getCornerFeatureExtractor( const hydrolfextract::LineExtractor::Config &cfg,
                           double rangeSd,
                           double bearingSd,
                           const hydroutil::Context &context )
{
    hydroutil::Properties prop = context.properties();
    std::string prefix = "Corners.";
    double pFalsePositive   = prop.getPropertyAsDoubleWithDefault( prefix+"PFalsePositive", 0.2 );
    double pTruePositive = prop.getPropertyAsDoubleWithDefault( prefix+"PTruePositive", 0.6 );

    return new hydrolfextract::CornerFeatureExtractor( rangeSd,
                                                       bearingSd,
                                                       pFalsePositive,
                                                       pTruePositive );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Driver::Driver( const hydroscanutil::ScannerConfig &scannerConfig,
                const hydroutil::Context            &context )
{
    std::string prefix = "";
    hydroutil::Properties prop = context.properties();
    double pointTargetRangeSd =
        prop.getPropertyAsDoubleWithDefault( prefix+"PointTargetRangeSd", 0.2);
    double pointTargetBearingSd =
        (M_PI/180.0)*prop.getPropertyAsDoubleWithDefault( prefix+"PointTargetBearingSd", 5.0);

    bool extractReflectors                 =
        prop.getPropertyAsIntWithDefault(    prefix+"ExtractReflectors", 1);
    bool extractForegroundPoints           =
        prop.getPropertyAsIntWithDefault(    prefix+"ExtractForegroundPoints", 0);
    bool extractLines                      =
        prop.getPropertyAsIntWithDefault(    prefix+"ExtractLines", 0);
    bool extractCorners                    =
        prop.getPropertyAsIntWithDefault(    prefix+"ExtractCorners", 0);
    bool extractDoors                      =
        prop.getPropertyAsIntWithDefault(    prefix+"ExtractDoors", 0);
    
    if ( extractReflectors )
    {
        reflectorExtractor_.reset( getReflectorExtractor( scannerConfig, context) );
        reflectorFeatureExtractor_.reset( getReflectorFeatureExtractor( reflectorExtractor_->config(),
                                                                        pointTargetRangeSd,
                                                                        pointTargetBearingSd,
                                                                        context) );
    }
    if ( extractForegroundPoints )
    {
        foregroundExtractor_.reset( getForegroundExtractor( scannerConfig,context) );
        foregroundFeatureExtractor_.reset( getForegroundFeatureExtractor( foregroundExtractor_->config(),
                                                                          pointTargetRangeSd,
                                                                          pointTargetBearingSd,
                                                                          context) );
    }
    if ( extractDoors )
    {
        doorExtractor_.reset( getDoorExtractor(scannerConfig,context) );
        doorFeatureExtractor_.reset( getDoorFeatureExtractor(doorExtractor_->config(),
                                                             pointTargetRangeSd*3.0/2.0,
                                                             pointTargetBearingSd,
                                                             context) );
    }
    if ( extractCorners || extractLines ) 
    {
        lineExtractor_.reset( getLineExtractor( scannerConfig, context ) );
        if ( extractLines )
            lineFeatureExtractor_.reset( getLineFeatureExtractor( lineExtractor_->config(),
                                                                  pointTargetRangeSd,
                                                                  pointTargetBearingSd,
                                                                  context ) );
        if ( extractCorners )
            cornerFeatureExtractor_.reset( getCornerFeatureExtractor( lineExtractor_->config(),
                                                                      pointTargetRangeSd,
                                                                      pointTargetBearingSd,
                                                                      context ) );
    }
}

std::vector<hydrofeatureobs::FeatureObs*> 
Driver::extractFeatures( const std::vector<float>         &ranges,
                         const std::vector<unsigned char> &intensities )
{
    std::vector<hydrofeatureobs::FeatureObs*> features;

    if ( reflectorExtractor_.get() )
    {
        std::vector<hydrolfextract::Reflector> reflectors = 
            reflectorExtractor_->extractReflectors( ranges, intensities );
        std::vector<hydrofeatureobs::PointFeatureObs*> hydroFeatures =
            reflectorFeatureExtractor_->extractFeatures( ranges, reflectors );
        features.insert( features.end(), hydroFeatures.begin(), hydroFeatures.end() );
    }

    if ( foregroundExtractor_.get() )
    {
        std::vector<hydrolfextract::AnnotatedPole> fgPoints = 
            foregroundExtractor_->extractForegroundPoints( ranges );
        std::vector<hydrofeatureobs::PointFeatureObs*> hydroFeatures =
            foregroundFeatureExtractor_->extractFeatures( ranges, fgPoints );
        features.insert( features.end(), hydroFeatures.begin(), hydroFeatures.end() );
    }

    if ( doorExtractor_.get() )
    {
        std::vector<hydrolfextract::Door> doors =
            doorExtractor_->extractDoors( ranges );
        std::vector<hydrofeatureobs::PointFeatureObs*> hydroFeatures =
            doorFeatureExtractor_->extractFeatures( ranges, doors );
        features.insert( features.end(), hydroFeatures.begin(), hydroFeatures.end() );
    }

    if ( lineExtractor_.get() )
    {        
        hydrolfextract::AnnotatedLines annLines = lineExtractor_->extractLines( ranges );

        if ( lineFeatureExtractor_.get() )
        {
            std::vector<hydrofeatureobs::LineFeatureObs*> hydroFeatures =
                lineFeatureExtractor_->extractFeatures( annLines );
            features.insert( features.end(), hydroFeatures.begin(), hydroFeatures.end() );
        }
        if ( cornerFeatureExtractor_.get() )
        {
            std::vector<hydrofeatureobs::PointFeatureObs*> hydroFeatures =
                cornerFeatureExtractor_->extractFeatures( annLines );
            features.insert( features.end(), hydroFeatures.begin(), hydroFeatures.end() );
        }
    }

    // Check pFalse/pTrue
    for ( unsigned int i=0; i < features.size(); i++ )
    {
        if ( features[i]->pTruePositive() <= features[i]->pFalsePositive() )
        {
            stringstream ss;
            ss << "Bad pTrue/pFalse for feature: " << *(features[i]);
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }
    }

    return features;
}

} // namespace

extern "C" {
    hydrointerfaces::LaserFeatureExtractorFactory *createDriverFactory()
    { return new lfecombined::Factory; }
}
