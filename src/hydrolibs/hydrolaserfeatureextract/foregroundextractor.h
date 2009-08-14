/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef HYDROLFEXTRACT_FOREGROUND_EXTRACTOR_H
#define HYDROLFEXTRACT_FOREGROUND_EXTRACTOR_H

#include <vector>
#include <hydrofeatureobs/features.h>
#include <hydroscanutil/scanutil.h>
#include <hydrolaserfeatureextract/polefinder.h>

namespace hydrolfextract {

struct AnnotatedPole {
    AnnotatedPole( const Pole &p, double pDistToClutter )
        : pole(p), distToClutter(pDistToClutter)
        {}

    Pole   pole;
    double distToClutter;
};
std::string toString( const AnnotatedPole &p );

class ForegroundExtractor
{

public: 

    struct Config {
        Config( const hydroscanutil::ScannerConfig &scannerCfg )
            : scannerConfig(scannerCfg),
              minForegroundWidth(0.1),
              maxForegroundWidth(0.5),
              minForegroundBackgroundSeparation(1.0),
              clearForegroundBackgroundSeparation(4.0),
              reliableScannerRange(5.0)
            {}

        hydroscanutil::ScannerConfig scannerConfig;
        double minForegroundWidth;
        double maxForegroundWidth;
        double minForegroundBackgroundSeparation;
        double clearForegroundBackgroundSeparation;
        double reliableScannerRange;

        bool isSane() const;
    };

    ForegroundExtractor( const Config &config )
        : config_(config)
        { assert( config.isSane() ); }

    std::vector<AnnotatedPole> extractForegroundPoints( const std::vector<float> &ranges,
                                                        bool debug=false ) const;

    const Config &config() const { return config_; }
    
private: 

    mutable std::vector<Pole> possiblePoles_;

    Config config_;
};
std::string toString( const ForegroundExtractor::Config &config );

class ForegroundFeatureExtractor
{

public: 

    ForegroundFeatureExtractor( const ForegroundExtractor::Config &config,
                                double rangeSd,
                                double bearingSd,
                                double pFalsePositive,
                                double pTruePositive )
        : config_(config),
          rangeSd_(rangeSd),
          bearingSd_(bearingSd),
          pFalsePositive_(pFalsePositive),
          pTruePositive_(pTruePositive)
        {}

    std::vector<hydrofeatureobs::PointFeatureObs*> extractFeatures( const std::vector<float>          &ranges,
                                                                    const std::vector<AnnotatedPole>  &fgPoints ) const;

private: 

    ForegroundExtractor::Config config_;

    double rangeSd_;
    double bearingSd_;
    double pFalsePositive_;
    double pTruePositive_;
};

//
// A collection of dodgy hacks for determining pTruePositive/pFalsePositive.
//
void
calcForegroundPointReliability( const std::vector<float>          &ranges,
                                const AnnotatedPole               &fgPoint,
                                const ForegroundExtractor::Config &config,
                                double                             basePTruePositive,
                                double                             basePFalsePositive,
                                double                            &pTruePositive,
                                double                            &pFalsePositive );

} // namespace

#endif
