/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orca/featuremap2d.h>
#include <orcaice/orcaice.h>
#include "util.h"
#include "combineddriver.h"
#include <orcaobj/orcaobj.h>

using namespace std;
using namespace orcaice;

namespace laserfeatures {

////////////////////////////////////////////////////////////////////////////////

namespace {

hydrolfextract::ReflectorFeatureExtractor *
getReflectorExtractor( const orca::RangeScanner2dDescription &laserDescr,
                       double rangeSd,
                       double bearingSd,
                       const orcaice::Context &context )
{
    hydrolfextract::ReflectorExtractor::Config cfg(getScannerConfig(laserDescr));

    std::string prefix = context.tag() + ".Config.Reflectors.";
    Ice::PropertiesPtr prop = context.properties();

    cfg.maxDeltaRangeWithinReflector      =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxDeltaRangeWithinReflector", 0.2 );
    cfg.maxDeltaRangeNearReflector        =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxDeltaRangeNearReflector", 0.3 );
    cfg.minReflectorBrightness            =
        orcaice::getPropertyAsIntWithDefault(    prop, prefix+"MinBrightness", 1);

    double pFalsePositive           =
        orcaice::getPropertyAsDoubleWithDefault(    prop, prefix+"PFalsePositive", 0.1);
    double pTruePositive            =
        orcaice::getPropertyAsDoubleWithDefault(    prop, prefix+"PTruePositive", 0.6);

    return new hydrolfextract::ReflectorFeatureExtractor( cfg, rangeSd, bearingSd, pFalsePositive, pTruePositive );
}

hydrolfextract::ForegroundFeatureExtractor *
getForegroundExtractor( const orca::RangeScanner2dDescription &laserDescr,
                        double rangeSd,
                        double bearingSd,
                        const orcaice::Context &context )
{
    hydrolfextract::ForegroundExtractor::Config cfg(getScannerConfig(laserDescr));

    std::string prefix = context.tag() + ".Config.FGPoints.";
    Ice::PropertiesPtr prop = context.properties();

    cfg.minForegroundWidth                =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MinForegroundWidth", 0.1);
    cfg.maxForegroundWidth                =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxForegroundWidth", 0.5);
    cfg.minForegroundBackgroundSeparation =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MinForegroundBackgroundSeparation", 0.5);

    double pFalsePositive =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"PFalsePositive", 0.4);
    double pTruePositive =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"PTruePositive", 0.5);

    return new hydrolfextract::ForegroundFeatureExtractor( cfg,
                                                           rangeSd,
                                                           bearingSd,
                                                           pFalsePositive,
                                                           pTruePositive );
}

hydrolfextract::DoorFeatureExtractor *
getDoorExtractor( const orca::RangeScanner2dDescription &laserDescr,
                  double rangeSd,
                  double bearingSd,
                  const orcaice::Context &context )
{
    hydrolfextract::DoorExtractor::Config cfg(getScannerConfig(laserDescr));

    std::string prefix = context.tag() + ".Config.Doors.";
    Ice::PropertiesPtr prop = context.properties();

    cfg.minDoorWidth = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MinDoorWidth", 0.9 );
    cfg.maxDoorWidth = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxDoorWidth", 1.2 );
    
    cfg.minDeltaRangeThruDoor = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MinDeltaRangeThruDoor", 2.0 );

    cfg.minReturnsThruDoor = orcaice::getPropertyAsIntWithDefault( prop, prefix+"MinReturnsThruDoor", 3 );
    cfg.minReturnsBesideDoor = orcaice::getPropertyAsIntWithDefault( prop, prefix+"MinReturnsBesideDoor", 2 );

    cfg.maxDeltaRangeWallBesideDoor = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxDeltaRangeWallBesideDoor", 0.3 );
            
    const double P_TRUE_POSITIVE  = 0.6;
    const double P_FALSE_POSITIVE = 0.3;
    double pTruePositive  = P_TRUE_POSITIVE;
    double pFalsePositive = P_FALSE_POSITIVE;

    return new hydrolfextract::DoorFeatureExtractor( cfg,
                                                     rangeSd,
                                                     bearingSd,
                                                     pFalsePositive,
                                                     pTruePositive,
                                                     context.toHydroContext() );
}

hydrolfextract::LineAndCornerFeatureExtractor *
getLineAndCornerFeatureExtractor( const orca::RangeScanner2dDescription &laserDescr,
                                  bool extractLines,
                                  bool extractCorners,
                                  double rangeSd,
                                  double bearingSd,
                                  const orcaice::Context &context )
{
    hydrolfextract::LineExtractor::Config cfg( getScannerConfig(laserDescr) );

    std::string prefix = context.tag() + ".Config.Lines.";
    Ice::PropertiesPtr prop = context.properties();

    cfg.maxDeltaRangeInCluster = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxDeltaRangeInCluster", 0.5 );
    cfg.breakDistThreshold   = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"BreakDistThreshold", 0.2 );
    cfg.minPointsInLine      = orcaice::getPropertyAsIntWithDefault( prop, prefix+"MinPointsInLine", 6 );
    
    double minLineLengthForLineFeature = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MinLineLengthForLineFeature", 1.0 );
    bool rejectLikelyGroundObservations = orcaice::getPropertyAsIntWithDefault( prop, prefix+"RejectLikelyGroundObservations", true );
    double linePFalsePositive   = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"PFalsePositive", 0.4 );
    double linePFalsePositivePossibleGround = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"PFalsePositivePossibleGround", 0.55 );
    double linePTruePositive = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"PTruePositive", 0.6 );


    std::string cornerPrefix = context.tag() + ".Config.Corners.";
    double cornerPFalsePositive   = orcaice::getPropertyAsDoubleWithDefault( prop, cornerPrefix+"PFalsePositive", 0.2 );
    double cornerPTruePositive = orcaice::getPropertyAsDoubleWithDefault( prop, cornerPrefix+"PTruePositive", 0.6 );

    const double minLineLengthForVisibleEndpoints = 0.5;

    return new hydrolfextract::LineAndCornerFeatureExtractor( cfg,
                                                              extractLines,
                                                              extractCorners,
                                                              minLineLengthForLineFeature,
                                                              minLineLengthForVisibleEndpoints,
                                                              rangeSd,
                                                              bearingSd,
                                                              rejectLikelyGroundObservations,
                                                              linePFalsePositive,
                                                              linePFalsePositivePossibleGround,
                                                              linePTruePositive,
                                                              cornerPFalsePositive,
                                                              cornerPTruePositive,
                                                              context.toHydroContext() );
}

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

CombinedDriver::CombinedDriver( const orca::RangeScanner2dDescription &laserDescr,
                                const orcaice::Context                &context )
{
    std::string prefix = context.tag() + ".Config.";
    Ice::PropertiesPtr prop = context.properties();
    double pointTargetRangeSd =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"PointTargetRangeSd", 0.2);
    double pointTargetBearingSd =
        (M_PI/180.0)*orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"PointTargetBearingSd", 5.0);

    bool extractReflectors                 =
        orcaice::getPropertyAsIntWithDefault(    prop, prefix+"ExtractReflectors", 1);
    bool extractForegroundPoints           =
        orcaice::getPropertyAsIntWithDefault(    prop, prefix+"ExtractForegroundPoints", 0);
    bool extractLines                      =
        orcaice::getPropertyAsIntWithDefault(    prop, prefix+"ExtractLines", 0);
    bool extractCorners                    =
        orcaice::getPropertyAsIntWithDefault(    prop, prefix+"ExtractCorners", 0);
    bool extractDoors                      =
        orcaice::getPropertyAsIntWithDefault(    prop, prefix+"ExtractDoors", 0);
    
    if ( extractReflectors )
    {
        reflectorExtractor_.reset( getReflectorExtractor( laserDescr,pointTargetRangeSd,pointTargetBearingSd,context) );
    }
    if ( extractForegroundPoints )
    {
        foregroundExtractor_.reset( getForegroundExtractor( laserDescr,pointTargetRangeSd,pointTargetBearingSd,context) );
    }
    if ( extractDoors )
    {
        doorExtractor_.reset( getDoorExtractor(laserDescr,pointTargetRangeSd*3.0/2.0,pointTargetBearingSd,context) );
    }
    if ( extractCorners || extractLines ) 
    {
        lineAndCornerExtractor_.reset( getLineAndCornerFeatureExtractor( laserDescr,
                                                                         extractLines,
                                                                         extractCorners,
                                                                         pointTargetRangeSd,
                                                                         pointTargetBearingSd,
                                                                         context ) );
    }
}

int
CombinedDriver::computeFeatures( const orca::LaserScanner2dDataPtr &laserDataPtr,
                                 orca::PolarFeature2dData          &featureData )
{
    assert( laserDataPtr != 0 );
    featureData.features.clear();

    if ( reflectorExtractor_.get() )
    {
        std::vector<hydrofeatureobs::PointFeatureObs> hydroFeatures =
            reflectorExtractor_->extractFeatures( laserDataPtr->ranges, laserDataPtr->intensities );
        for ( uint i=0; i < hydroFeatures.size(); i++ )
            featureData.features.push_back( convert( hydroFeatures[i] ) );
    }

    if ( foregroundExtractor_.get() )
    {
        std::vector<hydrofeatureobs::PointFeatureObs> hydroFeatures =
            foregroundExtractor_->extractFeatures( laserDataPtr->ranges );
        for ( uint i=0; i < hydroFeatures.size(); i++ )
            featureData.features.push_back( convert( hydroFeatures[i] ) );        
    }

    if ( doorExtractor_.get() )
    {
        std::vector<hydrofeatureobs::PointFeatureObs> hydroFeatures =
            doorExtractor_->extractFeatures( laserDataPtr->ranges );
        for ( uint i=0; i < hydroFeatures.size(); i++ )
            featureData.features.push_back( convert( hydroFeatures[i] ) );
    }

    if ( lineAndCornerExtractor_.get() )
    {        
        std::vector<hydrofeatureobs::FeatureObs*> hydroFeatures =
            lineAndCornerExtractor_->extractFeatures( laserDataPtr->ranges );
        for ( uint i=0; i < hydroFeatures.size(); i++ )
        {
            // cout<<"TRACE(combineddriver.cpp): Adding f: " << *(hydroFeatures[i]) << endl;
            featureData.features.push_back( convert( *(hydroFeatures[i]) ) );
        }
        for ( uint i=0; i < hydroFeatures.size(); i++ )
            delete hydroFeatures[i];
    }

    // Check pFalse/pTrue
    for ( unsigned int i=0; i < featureData.features.size(); i++ )
    {
        if ( featureData.features[i]->pTruePositive <= featureData.features[i]->pFalsePositive )
        {
            stringstream ss;
            ss << "Bad pTrue/pFalse for feature: " << orcaobj::toString(featureData.features[i]);
            throw gbxsickacfr::gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }
    }

    return 0;
}

}
