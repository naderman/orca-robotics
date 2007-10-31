/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <sys/time.h>
#include <unistd.h>

#include <orcaice/orcaice.h>
#include <orcapathplan/orcapathplan.h>

#include "laser2og.h"
#include "ogsensormodel.h"
#include "raytrace.h"

using namespace std;
using namespace orcapathplan;
using namespace orca;
using namespace laser2og;

static float laserScanBearing(const orca::RangeScanner2dData& scan, const int i)
{
    double angleIncrement = scan.fieldOfView / double(scan.ranges.size()+1);
    return (scan.startAngle + angleIncrement*i);
}


Laser2Og::Laser2Og(ogfusion::MapConfig& mapConfig, OgLaserModelConfig& sensorConfig)
{    
    mapSizeX_=mapConfig.mapSizeX;
    mapSizeY_=mapConfig.mapSizeY;
    mapResX_=mapConfig.mapResX;
    mapResY_=mapConfig.mapResY;
    mapOriginX_=mapConfig.mapOriginX;
    mapOriginY_=mapConfig.mapOriginY;

    //copy this other one
    maxLaserRange_ = sensorConfig.rangeMax;

    // logical model of occupancy
    sensorModel_ = new OgSensorModel( sensorConfig );
    //sensorModel_->print();
}

Laser2Og::~Laser2Og()
{ 
    delete sensorModel_;
}


// NOTE: assumes both POSE in map coordinate system
orca::OgObservation
Laser2Og::process( const hydronavutil::Pose &sensorPose, const orca::RangeScanner2dData &scan )
{
    // @todo where to get these settings from?
    const double RAY_EXTENSION = 1.25;

    // holds the set of cell likelihoods 
    ogfusion::OgBuffer buffer;
       
    // for each return
    for ( int i=0; i<(int)scan.ranges.size(); i++ )
    {
        // Ignore non-returns
        if ( scan.ranges[i] >= maxLaserRange_-1e-3 ) continue;

        // limit useful range. Note: negative range should be considered an error, but here it's just clipped to 0.
        double rangeEff = scan.ranges[i];
        CLIP_TO_LIMITS( 0.0, rangeEff, maxLaserRange_ );
        
        // find (x,y) coordinates of laser return in global
	    // NOTE!!! : arbitrary 25% overshoot on range.
        orcaogmap::CartesianPoint2d returnPoint;
        double rayBearing = sensorPose.theta() + laserScanBearing(scan, i );
        returnPoint.x = sensorPose.x() +
            RAY_EXTENSION*rangeEff*cos( rayBearing );

        returnPoint.y = sensorPose.y() +
            RAY_EXTENSION*rangeEff*sin( rayBearing );
        
        // find cells crossed by the vehicle-to-return_point ray
        orcaogmap::CartesianPoint2d sensorPosePoint;
        sensorPosePoint.x = sensorPose.x();
        sensorPosePoint.y = sensorPose.y();
        std::vector<Cell2D> ray = rayTrace( sensorPosePoint, returnPoint, mapOriginX_, mapOriginY_, mapResX_, mapResY_ );

        //cout<<"Laser2Og::process: traced a ray with "<<ray.size()<<" cells. "<<
        //    "from ("<<sensorPosePoint.x<<","<<sensorPosePoint.y<<") to ("<<returnPoint.x<<","<<returnPoint.y<<")"<<endl;
        //cout << "with bearing of "<< laserScanBearing(scan, i )<<endl;

        Cell2D currentCell;
        OgCellLikelihood currentFeature;

        // for each traversed cell
        for ( unsigned int j=0; j<ray.size(); j++ )
        {
            // cell along the ray
            currentCell = ray[j];  

            // Check if it is inside map
            if( currentCell.x() < 0 || currentCell.x() >= mapSizeX_ || 
                currentCell.y() < 0 || currentCell.y() >= mapSizeY_ )
            {
                continue;
            }

            // Distance to a particular cell along the ray.
            // NOTE: use distance from midcell to robot location instead of distance btw cell centers.
            double dist = orcapathplan::euclideanDistance( sensorPosePoint,
                                                           cell2point(currentCell,
                                                                      mapOriginX_,
                                                                      mapOriginY_,
                                                                      mapResX_,
                                                                      mapResY_) );
            
            // =========== LIKELIHOOD ====================
            const double posSD=0, headSD=0;
            double Pzx = sensorModel_->likelihood( dist, rangeEff, posSD, headSD );

            currentFeature.likelihood=Pzx;
            currentFeature.x=currentCell.x();
            currentFeature.y=currentCell.y();

            // cell index
            int currentCellIndex = orcapathplan::sub2ind( currentCell, mapSizeX_, mapSizeY_ );
            buffer.insert( currentCellIndex,
                           currentFeature,
//                           ogfusion::OgBuffer::PolicyAddLikelihood );
                           ogfusion::OgBuffer::PolicyMaxLikelihood );
        
        }  // end for each traversed cell
    
    }  // end for each return
    
    
    return getObs( buffer );
}

orca::OgObservation
Laser2Og::getObs( const ogfusion::OgBuffer &buffer )
{
    orca::OgObservation obs( buffer.size() );

    const std::map<int,orca::OgCellLikelihood> &bufferAsMap = buffer.bufferAsMap();

    int i=0;
    for ( std::map<int,orca::OgCellLikelihood>::const_iterator it = bufferAsMap.begin();
          it != bufferAsMap.end();
          it++ )
    {
        obs[i++] = it->second;
    }

    return obs;
}
