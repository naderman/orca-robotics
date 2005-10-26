#include "extractorone.h"

#include <orcaiceutil/objutils.h>
#include <iostream>
#include <orca/mathdefs.h>

#include "svd.h"
#include "polefinder.h"
#include "featureextraction.h"

using namespace orca;
using namespace std;
using namespace orcaiceutil;

ExtractorOne::ExtractorOne()
{
    laserRange_ = MAX_RANGE;
}

ExtractorOne::~ExtractorOne()
{
}

int ExtractorOne::initialize( ConfigParameters *configParameters )
{   
    backgroundRangeGate_ = configParameters -> backgroundRangeGate;
    featureRangeGate_  = configParameters -> targetRangeGate;  
    minReturns_  = configParameters -> minReturnNumber;
    minBrightness_ = configParameters -> minBrightness;
    extractReflectors_ = configParameters -> extractReflectors;
    extractForegroundPoints_ = configParameters -> extractForegroundPoints;
    extractCorners_ = configParameters -> extractCorners;
    extractDoors_ = configParameters -> extractDoors;
    minForegroundWidth_ = configParameters -> minForegroundWidth;
    maxForegroundWidth_ = configParameters -> maxForegroundWidth;
    minForegroundBackgroundSeparation_ = configParameters -> minForegroundBackgroundSeparation;
    
    return 0;
}

int ExtractorOne::computeFeatures( const LaserDataPtr scan, PolarFeature2dDataPtr features)
{
    features -> features.clear();

    if (extractReflectors_)
        extractLaserReflectors(scan, features);
    if (extractForegroundPoints_)
        extractForegroundPoints(scan, features);
    if (extractDoors_)
        extractDoors(scan, features);
    if (extractCorners_)
        extractCorners(scan, features);
  
    return 0;
}


bool ExtractorOne::extractLaserReflectors(const orca::LaserDataPtr scan, orca::PolarFeature2dDataPtr features)
{
    bool buildingTarget = false;

    int featureNumPnts = 0;
    double featureRange = -1.0;
    double featureBearing = -1.0;
    //int featureIntensity = -1;

    // for each return
    for ( int i=1; i<scan->ranges.size()-1; i++ )
    {
        //if ( scan.intensity(i) >= minBrightness_ && fabs( scan.range(i-1) - scan.range(i) ) < backgroundRangeGate_ )
        // George:
        // include laser range and check on both sides of the return for 'possible doggy' range
        // for a range to be doggy it usually has to have both both neighbouring ranges far away. 
        if ( scan->intensities[i] >= minBrightness_ &&
             scan->ranges[i] < laserRange_ &&
             ( fabs( scan->ranges[i-1] - scan->ranges[i] ) < backgroundRangeGate_ ||
               fabs( scan->ranges[i+1] - scan->ranges[i] ) < backgroundRangeGate_  ) )
        {
            // start building a new cluster of high intensity returns
            if ( buildingTarget == false )
            {
                buildingTarget = true;
                featureNumPnts = 1;
                featureRange = scan->ranges[i];
                featureBearing = M_PI*i/(scan->ranges.size()-1) - M_PI/2;
                continue; // go to the next return
            }

            // if the new return is close enough to the current average include it in the estimate
            if ( fabs(scan->ranges[i] - featureRange/featureNumPnts) < featureRangeGate_ )
            {
                ++featureNumPnts;

                featureRange += scan->ranges[i];
                featureBearing += M_PI*i/(scan->ranges.size()-1) - M_PI/2;
            }
        }
        else
        {
            // if we have reached the end of a cluster, process a new observation
            if ( buildingTarget == true )
            {
                buildingTarget = false;
                
                // George:
                // not sure whether this is suppose to stop laser beacons that are on a corner
                // from not becoming foreground features as well?? if it is, I don't think it
                // works, so will disable it and just check minReturns_.
                
                // again make sure that the feature is not a foreground point
                //if ( fabs( scan.range(i) -  scan.range(i+1) ) < backgroundRangeGate_ && featureNumPnts >= minReturns_ )

                if ( featureNumPnts >= minReturns_ )
                {                 
//                     orca::PolarPointFeature2D feature( featureRange / featureNumPnts, 
//                                                        featureBearing / featureNumPnts,
//                                                        slamfeatures::LASER_REFLECTOR );

//                     features.addElement( feature );

                    orca::PolarPoint2d pp;
                    pp.r = featureRange / featureNumPnts;
                    pp.o = featureBearing / featureNumPnts;
                    
                    features->features.push_back( pp );
                    features->featureTypes.push_back( slamfeatures::LASER_REFLECTOR );
                }
            } // end of end-of-cluster if

        }     // end for each return
    }
    return 0;  
}

bool ExtractorOne::extractForegroundPoints(const orca::LaserDataPtr scan, orca::PolarFeature2dDataPtr)
{
    return 0;

}

bool ExtractorOne::extractDoors(const orca::LaserDataPtr scan, orca::PolarFeature2dDataPtr)
{
    return 0;

}

bool ExtractorOne::extractCorners(const orca::LaserDataPtr scan, orca::PolarFeature2dDataPtr)
{
    return 0;

}

bool ExtractorOne::extractPossibleCorners(orca::PolarFeature2dDataPtr)
{
    return 0;

}

void ExtractorOne::connectSections(const orca::LaserDataPtr scan)
{

}

void ExtractorOne::extractLines()
{

}

void ExtractorOne::findBreakPoint(Section &s, double &maxDist, int &pos)
{

}

void ExtractorOne::fitLine(Section &s)
{

}

void ExtractorOne::printSections()
{

}

    