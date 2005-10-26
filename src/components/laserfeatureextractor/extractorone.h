#ifndef ORCA2_EXTRACTOR_ONE_H
#define ORCA2_EXTRACTOR_ONE_H

#include <vector>
#include <list>

#include "section.h"
#include "featureextractorbase.h"
#include "configparameters.h"

// Stage values....Real Values
#define MAX_RANGE    79.99     // 7.99    79.99
#define NO_SECTION    -1
#define RANGE_DELTA     1.0    // 0.10    0.2
#define BREAK_DIST     0.5    // 0.4    0.2
#define MIN_POINTS_IN_LINE   6    // 6      6
#define ERROR_THRESHOLD   0.24    // 0.10    0.24
#define CORNER_BOUND     0.45    // 0.45    0.45
#define POSSIBLE_BOUND    0.2    // ???    0.2

/*!

\brief

\author Tobias Kaupp t.kaupp at cas.edu.au

*/
class ExtractorOne: public FeatureExtractorBase 
{

public:

    ExtractorOne();
    virtual ~ExtractorOne();
    
    int initialize( ConfigParameters *configParameters );
    int computeFeatures( const orca::LaserDataPtr scan, orca::PolarFeature2dDataPtr features);
    
private:
    
    bool extractReflectors_;
    bool extractForegroundPoints_;
    bool extractCorners_;
    bool extractDoors_;

    // Parameters for laser reflectors
    double backgroundRangeGate_;
    double featureRangeGate_;
    int minReturns_;
    int minBrightness_;

    // Parameters for foreground points
    double minForegroundWidth_;
    double maxForegroundWidth_;
    double minForegroundBackgroundSeparation_;
    
    double laserRange_;
    
    std::vector<Section> sections_;
  
    bool extractLaserReflectors(const orca::LaserDataPtr scan, orca::PolarFeature2dDataPtr features);
    bool extractForegroundPoints(const orca::LaserDataPtr scan, orca::PolarFeature2dDataPtr features);
    bool extractDoors(const orca::LaserDataPtr scan, orca::PolarFeature2dDataPtr features);
    bool extractCorners(const orca::LaserDataPtr scan, orca::PolarFeature2dDataPtr features);
    bool extractPossibleCorners(orca::PolarFeature2dDataPtr features);

    //
    // Corner detector routines
    //void calculatePos(const orca::LaserDataPtr scan);
    void connectSections(const orca::LaserDataPtr scan);
    void extractLines();
    void findBreakPoint(Section &s, double &maxDist, int &pos);
    void fitLine(Section &s);
    void printSections();

};

#endif
