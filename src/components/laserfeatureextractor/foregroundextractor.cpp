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
#include <assert.h>
#include <cmath>

#include <orca/featuremap2d.h>
#include <orcaice/orcaice.h>
#include "foregroundextractor.h"
#include "polefinder.h"

using namespace std;

namespace laserfeatures {

namespace {

    double dist( int i, int j, const std::vector<float> &ranges, double angleIncrement )
    {
        assert( i >= 0 && i < (int)(ranges.size()) );
        assert( j >= 0 && j < (int)(ranges.size()) );

        double xi = ranges[i];
        double yi = 0;
        double xj = ranges[j]*cos((j-i)*angleIncrement);
        double yj = ranges[j]*sin((j-i)*angleIncrement);

        const double distance = hypot( yi-yj, xi-xj );
        return distance;
    }

    bool tooCloseToClutter( const orca_polefinder::Pole &pole, 
                            double poleRange,
                            const std::vector<float> &ranges,
                            double startAngle,
                            double angleIncrement,
                            double minDistToClutter )
    {
        // If we're too close to the pole, maybe there's clutter just behind us
        if ( poleRange < minDistToClutter )
            return true;

        // Check an arc of minDistToClutter either side
        // (straight-line distance is probably better, but it's close enough)
        double angleEitherSide = minDistToClutter / poleRange;
        int numScansEitherSide = (int)(ceil(angleEitherSide / angleIncrement));

        // cout<<"TRACE(foregroundextractor.cpp): poleRange: " << poleRange << ", numScansEitherSide: " << numScansEitherSide << endl;

        // If the pole's too close to the edge of the scan, maybe
        // there's clutter just outside our field of view
        if ( pole.startI < numScansEitherSide )
            return true;
        if ( pole.endI >= (int)(ranges.size())-numScansEitherSide )
            return true;
        
        // Check for clutter near the pole
        for ( int i=pole.startI-numScansEitherSide; i < pole.startI; i++ )
        {
            if ( dist( i, pole.startI, ranges, angleIncrement ) < minDistToClutter )
                return true;
        }
        for ( int i=pole.endI+1; i <= pole.endI+numScansEitherSide; i++ )
        {
            if ( dist( i, pole.endI, ranges, angleIncrement ) < minDistToClutter )
                return true;
        }

        // cout<<"TRACE(foregroundextractor.cpp): not too close to clutter." << endl;
        return false;
    }
}

ForegroundExtractor::ForegroundExtractor( const orcaice::Context & context, double laserMaxRange )
    : laserMaxRange_( laserMaxRange )
{
    std::string prefix = context.tag() + ".Config.FGPoints.";
    Ice::PropertiesPtr prop = context.properties();

    minForegroundWidth_                =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MinForegroundWidth", 0.1);
    maxForegroundWidth_                =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxForegroundWidth", 0.5);
    minForegroundBackgroundSeparation_ =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MinForegroundBackgroundSeparation", 0.5);

    pFalsePositive_ =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"PFalsePositive", 0.3);
    pTruePositive_ =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"PTruePositive", 0.5);

    prefix = context.tag() + ".Config.";
    prop = context.properties();
    rangeSd_   = orcaice::getPropertyAsDoubleWithDefault(    prop, prefix+"PointTargetRangeSd", 0.2);
    bearingSd_ = (M_PI/180.0)*orcaice::getPropertyAsDoubleWithDefault(    prop, prefix+"PointTargetBearingSd", 5.0);
}

void ForegroundExtractor::addFeatures( const orca::LaserScanner2dDataPtr &laserData, 
                                       orca::PolarFeature2dData &features )
{
    assert( laserMaxRange_ > 0.0 );

    double angleIncrement = orcaice::calcAngleIncrement( laserData->fieldOfView,
                                                         laserData->ranges.size() );
    std::vector<orca_polefinder::Pole> poles;
    double startAngleFromDodge = DEG2RAD( 2.0 );

    int numPoles = orca_polefinder::detect_poles(
            laserData->ranges,
            laserData->startAngle,
            angleIncrement,
            laserMaxRange_,
            minForegroundWidth_,
            maxForegroundWidth_,
            minForegroundBackgroundSeparation_,
            startAngleFromDodge,
            poles );

    for ( int i=0; i < numPoles; i++ )
    {
        double range   = (laserData->ranges[poles[i].startI]+laserData->ranges[poles[i].endI])/2.0;
        double bearing = ((poles[i].startI+poles[i].endI)/2.0)*angleIncrement + laserData->startAngle;

        if ( tooCloseToClutter( poles[i],
                                range,
                                laserData->ranges,
                                laserData->startAngle,
                                angleIncrement,
                                minForegroundBackgroundSeparation_ ) )
            continue;

        orca::PointPolarFeature2dPtr pp = new orca::PointPolarFeature2d;
        pp->type = orca::feature::FOREGROUNDPOINT;
        pp->p.r  = range;
        pp->p.o  = bearing;
        pp->rangeSd = rangeSd_;
        pp->bearingSd = bearingSd_;
        if ( poles[i].startI == poles[i].endI )
        {
            // Pole consists of just a single point...  Don't trust it so much.
            pp->pFalsePositive = 0.45;
            pp->pTruePositive = 0.55;
        }
        else
        {
            pp->pFalsePositive = pFalsePositive_;
            pp->pTruePositive = pTruePositive_;
        }
        features.features.push_back(pp);
    }

}

}
