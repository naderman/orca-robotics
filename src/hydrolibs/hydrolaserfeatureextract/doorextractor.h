/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef HYDROLFEXTRACT_DOOREXTRACTOR__H
#define HYDROLFEXTRACT_DOOREXTRACTOR__H

#include <vector>
#include <hydrofeatureobs/features.h>
#include <hydroscanutil/scanutil.h>
#include <hydroutil/context.h>

namespace hydrolfextract {

class DoorJamb {
public:
    DoorJamb( const hydroscanutil::ScannerConfig &scannerConfig,
              const std::vector<float>           &ranges,
              int                                 pReturnI );

    int returnI;
    double range;
    double bearing;
    double x;
    double y;
};
inline std::ostream &operator<<( std::ostream &s, const DoorJamb &d )
{ return s << "[i,r,b = "<<d.returnI<<", "<<d.range<<", "<<d.bearing*180.0/M_PI<<"deg]"; }

class Door {
public:
    Door( const DoorJamb &doorStart, const DoorJamb &doorEnd )
        : start(doorStart),
          end(doorEnd)
        {}

    DoorJamb start;
    DoorJamb end;
};
std::string toString( const Door &door );
inline std::ostream &operator<<( std::ostream &s, const Door &d )
{ return s << toString(d); }

//!
//! Extract raw door information
//!
class DoorExtractor
{

public: 

    struct Config {
        
        Config( const hydroscanutil::ScannerConfig &scannerCfg )
            : scannerConfig(scannerCfg),
              minDoorWidth(0.9),
              maxDoorWidth(1.2),
              minDeltaRangeThruDoor(2.0),
              minReturnsThruDoor(3),
              minReturnsBesideDoor(2),
              maxDeltaRangeWallBesideDoor(0.3)
            {}

        hydroscanutil::ScannerConfig scannerConfig;
        double minDoorWidth;
        double maxDoorWidth;
        double minDeltaRangeThruDoor;
        int    minReturnsThruDoor;
        int    minReturnsBesideDoor;
        double maxDeltaRangeWallBesideDoor;        

        bool isSane() const;
    };

    ////////////////////////////////////////

    DoorExtractor( const Config &c, const hydroutil::Context &context );

    std::vector<Door> extractDoors( const std::vector<float> &ranges ) const;
    
    const Config &config() const { return config_; }

private: 

    void findPossibleDoorJambs( const std::vector<float> &ranges,
                                std::vector<DoorJamb>    &possibleStarts,
                                std::vector<DoorJamb>    &possibleEnds ) const;

    void findActualDoors( const std::vector<float>    &ranges,
                          const std::vector<DoorJamb> &possibleStarts,
                          const std::vector<DoorJamb> &possibleEnds,
                          std::vector<Door>           &actualDoors ) const;

    double minDoorWidthSq_;
    double maxDoorWidthSq_;

    Config             config_;
    hydroutil::Context context_;
};

//!
//! Extracts doors as feature observations
//!
class DoorFeatureExtractor {
public:

    DoorFeatureExtractor( DoorExtractor::Config cfg,
                          double rangeSd,
                          double bearingSd,
                          double pFalsePositive,
                          double pTruePositive )
        : config_(cfg),
          rangeSd_(rangeSd),
          bearingSd_(bearingSd),
          pFalsePositive_(pFalsePositive),
          pTruePositive_(pTruePositive)
        {
            assert( pTruePositive_ > pFalsePositive_ ); 
            assert( rangeSd_ > 0 );
            assert( bearingSd_ > 0 );            
        }

    std::vector<hydrofeatureobs::PointFeatureObs*> extractFeatures( const std::vector<float> &ranges,
                                                                    const std::vector<Door>  &doors ) const;

private:

    DoorExtractor::Config config_;
    double rangeSd_;
    double bearingSd_;
    double pFalsePositive_;
    double pTruePositive_;

};

}

#endif
