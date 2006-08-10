/*
 *  Bruce2 - part of the Bruce Project
 *  
 *  Australian Centre for Field Robotics
 *  Sydney, Australia
 *  www.acfr.usyd.edu.au
 *  
 */

#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <sys/time.h>
#include <unistd.h>

#include <orca/bros1.h>

#include <orcaice/orcaice.h>
#include <orcaobj/mathdefs.h>

#include <orcapathplan/orcapathplan.h>
#include <orcaogfusion/orcaogfusion.h>

#include <orcamisc/cov2d.h>

#include "laser2og.h"
#include "ogsensormodel.h"
#include "raytrace.h"

using namespace std;
using namespace orcaogmap;
using namespace orcapathplan;
using namespace orca;
using namespace laser2og;
using namespace orcamisc;

static float laserScanBearing(const orca::RangeScannerData& scan, const int i)
{
    return (scan.startAngle+scan.angleIncrement*i);
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
    laserRange_ = sensorConfig.rangeMax;

    // logical model of occupancy
    sensorModel_ = new OgSensorModel( sensorConfig );
    //sensorModel_->print();
}

Laser2Og::~Laser2Og()
{ 
    delete sensorModel_;
}


// NOTE: assumes both POSE in map coordinate system
int Laser2Og::process( const orca::Localise2dData &sensorPose, const orca::RangeScannerData & scan )
{
    if ( scan.ranges.size() == 0 )
    {
        //cout << "Laser2Og::process(): empty laser scan" <<endl;
        return -1;
    }
    if(sensorPose.hypotheses.size()!=1)
    {
        //cout << "Laser2Og::process(): too many hypotheses." <<endl;
        return -1;
    }

    // Ellipse of position uncertainty
    Cov2d posEll(sensorPose.hypotheses[0].cov.xx,
		 sensorPose.hypotheses[0].cov.xy,
                 sensorPose.hypotheses[0].cov.yy );

    double a,b,t;
    posEll.ellipse(a,b,t);

    // Find larger of two components
    double posStDev = a > b ? sqrt(a) : sqrt(b);

    if(posStDev > sensorModel_->posStDevMax() )
    {
	//cout << "Laser2Og::process(): position variance too great." <<endl;
        return -1;
    }

    // chech heading uncertainty
    double hedStdDev = sqrt(sensorPose.hypotheses[0].cov.tt);

    if( hedStdDev > sensorModel_->hedStDevMax() )
    {
	cout << "Laser2Og::process(): heading Std Dev too great." <<endl;
	cout << "Laser2Og::process(): heading Std Dev: " << hedStdDev << endl;
        return -1;
    }


    //! @todo where to get these settings from?
    const double RAY_EXTENSION = 1.25;
    
    // for each return
    for ( int i=0; i<(int)scan.ranges.size(); i++ )
    {
        // limit useful range. Note: negative range should be considered an error, but here it's just clipped to 0.
        double rangeEff = CHECK_LIMITS( laserRange_, scan.ranges[i], 0.0 );
        
        // find (x,y) coordinates of laser return in global
	// NOTE!!! : arbitrary 25% overshoot on range.
	orcaogmap::CartesianPoint2d sensorPosePoint;

	sensorPosePoint.x=sensorPose.hypotheses[0].mean.p.x;
	sensorPosePoint.y=sensorPose.hypotheses[0].mean.p.y;

	orcaogmap::CartesianPoint2d returnPoint;
        double rayBearing = sensorPose.hypotheses[0].mean.o+laserScanBearing(scan, i );
	returnPoint.x = sensorPose.hypotheses[0].mean.p.x +
	    RAY_EXTENSION*rangeEff*cos( rayBearing );

	returnPoint.y = sensorPose.hypotheses[0].mean.p.y +
	    RAY_EXTENSION*rangeEff*sin( rayBearing );
        
        // find cells crossed by the vehicle-to-return_point ray
        std::vector<Cell2D> ray = rayTrace( sensorPosePoint, returnPoint, mapOriginX_, mapOriginY_, mapResX_, mapResY_ );

        //cout<<"Laser2Og::process: traced a ray with "<<ray.size()<<" cells. "<<
        //    "from ("<<sensorPosePoint.x<<","<<sensorPosePoint.y<<") to ("<<returnPoint.x<<","<<returnPoint.y<<")"<<endl;
        //cout << "with bearing of "<< laserScanBearing(scan, i )<<endl;

        Cell2D currentCell;
	OgCellLikelihood currentFeature;

        // for each traversed cell
        for ( uint j=0; j<ray.size(); j++ )
        {
            // cell along the ray
            currentCell = ray[j];  

	    // Check if it is inside map
            if( currentCell.x() < 0 || currentCell.x() > mapSizeX_ || 
                currentCell.y() < 0 || currentCell.y() > mapSizeY_ )
            {
                continue;
            }

            // Distance to a particular cell along the ray.
            // NOTE: use distance from midcell to robot location instead of distance btw cell centers.
            double dist = orcapathplan::euclideanDistance( sensorPosePoint,  cell2point(currentCell, mapOriginX_, mapOriginY_, mapResX_, mapResY_) );
            
            // cell index
            int currentCellIndex = orcapathplan::sub2ind( currentCell, mapSizeX_, mapSizeY_ );
            
            //cout<<"Laser2Og::process: looking up cell ("<<currentCell.x()<<","<<currentCell.y()<<")="<<currentCellIndex<<endl;
            
            // =========== LIKELIHOOD ====================
            // get position varienace in direction of ray
	    //double SPos = sqrt(posEll.rotCov(rayBearing,a,b,t));
	    //double Pzx = sensorModel_->likelihood( dist, rangeEff, SPos , hedStdDev );

	    double Pzx = sensorModel_->likelihood( dist, rangeEff, 0.0 , 0.0 );

            //cout << Pzx <<" ";
            //cout<<"C:"<<currentCell<<"I:"<<currentCellIndex<<"L:"<<Pzx<<endl;
            
	    currentFeature.likelihood=Pzx;
	    currentFeature.x=currentCell.x();
	    currentFeature.y=currentCell.y();

            buffer_.insertAdd(currentCellIndex, currentFeature );
        
        }  // end for each traversed cell
    
    }  // end for each return
    
    
    //cout<<"Laser2Og::process: current buffer size "<<buffer_.size()<<endl;

    return 0;
}

int Laser2Og::getObs( orca::OgObservation &obs )
{
    // empty referenced object
    obs.clear();

    //sequence.setSize( buffer_.size() );
    OgCellLikelihood feature;

    while ( !buffer_.isEmpty() )
    {
        // pop feature from buffer
        //buffer_.popFront( &feature );
        feature = buffer_.begin();
        
        obs.push_back(feature);

        // erase feature from buffer
        buffer_.eraseFront();
    }

    return 0;
}
