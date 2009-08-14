/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef HYDROLFEXTRACT_REFLECTOREXTRACTOR__H
#define HYDROLFEXTRACT_REFLECTOREXTRACTOR__H

#include <vector>
#include <hydrofeatureobs/features.h>
#include <hydroscanutil/scanutil.h>

namespace hydrolfextract {

class Reflector {
public:
    int startI;
    int endI;
    int numReturns() const
        { return endI-startI+1; }
};
std::string toString( const Reflector &reflector );
inline std::ostream &operator<<( std::ostream &s, const Reflector &r )
{ return s << toString(r); }

//!
//! Extract the raw extents of the reflector
//!
class ReflectorExtractor
{

public: 

    struct Config {
        // Reasonable defaults
        Config( const hydroscanutil::ScannerConfig &scannerCfg )
            : scannerConfig(scannerCfg),
              maxDeltaRangeNearReflector(0.3),
              maxDeltaRangeWithinReflector(0.2),
              minReflectorBrightness(1)
            {}

        hydroscanutil::ScannerConfig scannerConfig;
        double maxDeltaRangeNearReflector;
        double maxDeltaRangeWithinReflector;
        int    minReflectorBrightness;

        bool isSane() const;
    };

    ////////////////////////////////////////

    ReflectorExtractor( const Config &config )
        : config_(config)
        { assert( config.isSane() ); }

    std::vector<Reflector> extractReflectors( const std::vector<float>         &ranges,
                                              const std::vector<unsigned char> &intensities ) const;

    const Config &config() const { return config_; }

private: 

    // 
    // Helper functions.  These assume that you're not asking for
    // info about the first or last return
    //

    // Returns the change in range between this return and the previous one.
    double deltaRange( const std::vector<float> &ranges,
                       int returnNum ) const
        { 
            assert( returnNum != 0 );
            return ranges[returnNum] - ranges[returnNum-1]; 
        }

    bool isSketchy( const std::vector<float> &ranges,
                    int returnNum ) const;

    mutable std::vector<Reflector> reflectors_;

    Config config_;    
};

//!
//! Extracts complete features
//!
class ReflectorFeatureExtractor {
public:
    ReflectorFeatureExtractor( const ReflectorExtractor::Config &config,
                               double rangeSd,
                               double bearingSd,
                               double pFalsePositive,
                               double pTruePositive )
        : config_(config),
          rangeSd_(rangeSd),
          bearingSd_(bearingSd),
          pFalsePositive_(pFalsePositive),
          pTruePositive_(pTruePositive)
        { 
            assert( pTruePositive_ > pFalsePositive_ ); 
            assert( rangeSd_ > 0 );
            assert( bearingSd_ > 0 );
        }

    std::vector<hydrofeatureobs::PointFeatureObs*> extractFeatures( const std::vector<float>     &ranges,
                                                                    const std::vector<Reflector> &reflectors ) const;
private:

    ReflectorExtractor::Config config_;
    double rangeSd_;
    double bearingSd_;
    double pFalsePositive_;
    double pTruePositive_;
};

//! Some dodgy heuristics: reflectors near big discontinuities are less reliable.
double calcReflectorPFalsePositive( const std::vector<float> &ranges,
                                    int start,
                                    int end,
                                    double bestPossiblePFalsePositive,
                                    double maxDeltaRangeNearReflector );

//! Reduces the reflector to a single point.
geom2d::PolarPoint
calcObsRB( const std::vector<float>           &ranges,
           const hydroscanutil::ScannerConfig &scannerConfig,
           const Reflector                    &reflector );                        

} // namespace

#endif
