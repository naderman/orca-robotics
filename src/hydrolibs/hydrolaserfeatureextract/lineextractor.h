/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef HYDROLFEXTRACT_LINEEXTRACTOR__H
#define HYDROLFEXTRACT_LINEEXTRACTOR__H

#include <hydrofeatureobs/features.h>
#include <hydroscanutil/scanutil.h>
#include <hydrolaserfeatureextract/annotatedlines.h>
#include <hydroutil/context.h>
#include <assert.h>

namespace hydrolfextract {

////////////////////////////////////////////////////////////////////////////////

//!
//! Extracts raw line information
//!
//! @author Alex Brooks
//!
class LineExtractor
{

public: 

    struct Config {
        Config( const hydroscanutil::ScannerConfig &scannerCfg )
            : scannerConfig(scannerCfg),
              maxDeltaRangeInCluster(0.5),
              maxDeltaInLine(0.2),
              minPointsInLine(6),
              minLineLengthForVisibleEndpoints(0.5)
            {}

        hydroscanutil::ScannerConfig scannerConfig;

        // config
        double maxDeltaRangeInCluster;
        double maxDeltaInLine;
        int    minPointsInLine;
        double minLineLengthForVisibleEndpoints;

        bool isSane() const;
    };

    ////////////////////////////////////////

    LineExtractor( const Config &config,
                   const hydroutil::Context &context )
        : config_(config),
          context_(context)
        { assert( config.isSane() ); }

    AnnotatedLines extractLines( const std::vector<float> &ranges ) const;

    const Config &config() const { return config_; }

private:

    Config             config_;
    hydroutil::Context context_;
};

////////////////////////////////////////////////////////////////////////////////

class LineFeatureExtractor {
public:

    LineFeatureExtractor( double minLineLength,
                          double rangeSd,
                          double bearingSd,
                          double pFalsePositive,
                          double pTruePositive )
        : minLineLength_(minLineLength),
          rangeSd_(rangeSd),
          bearingSd_(bearingSd),
          pFalsePositive_(pFalsePositive),
          pTruePositive_(pTruePositive)
        {}

    std::vector<hydrofeatureobs::LineFeatureObs*> extractFeatures( const AnnotatedLines &annLines ) const;

private:

    double minLineLength_;
    double rangeSd_;
    double bearingSd_;
    double pFalsePositive_;
    double pTruePositive_;
};

class CornerFeatureExtractor {
public:

    CornerFeatureExtractor( double rangeSd,
                            double bearingSd,
                            double pFalsePositive,
                            double pTruePositive )
        : rangeSd_(rangeSd),
          bearingSd_(bearingSd),
          pFalsePositive_(pFalsePositive),
          pTruePositive_(pTruePositive)
        {}

    std::vector<hydrofeatureobs::PointFeatureObs*> extractFeatures( const AnnotatedLines &annLines ) const;

private:

    double rangeSd_;
    double bearingSd_;
    double pFalsePositive_;
    double pTruePositive_;
};
}

#endif
