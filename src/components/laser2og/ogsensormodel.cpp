/***************************************************************************
                          ogsensormodel.cpp  -  description
                             -------------------
    begin                : Tue Mar 4 2003
    copyright            : (C) 2003 by Alex Makarenko
    email                : a.makarenko@acfr.usyd.edu.au
    description          : ACFR Red Robot project
    :
    versions             : v0. code extracted from MapOccGrid class
    : 10/08/03 added laser2motion model
 ***************************************************************************/


#include <iostream>
#include <orcaobj/mathdefs.h>

#include "ogsensormodel.h"

using namespace std;
using namespace laser2og;


OgSensorModel::OgSensorModel( const OgLaserModelConfig & config ) :
    config_( config )
{
    const double UNKNOWN = 0.500;
    const double OCCUP = config_.occupMax;
    const double EMPTY = config_.emptyMax;

    // length of array = 2*halfSize+1
    int halfSize = (config_.size-1 ) / 2;
    
    // normal distribution for config_.rangeStDev = 1 (not to scale!!!)
    std::vector<double> N;
    for ( int i=0; i<2*halfSize+1; ++i )
    {
        r_.push_back( -config_.rangeStDevMax + config_.rangeStDevMax*i/halfSize );
        N.push_back( exp( -0.5 * r_[i]*r_[i] )  ) ;
    }
    
    // within range
    for ( int i=0; i<halfSize; i++ )
    {
        pIn_.push_back( EMPTY + (OCCUP-EMPTY) * N[i] );
    }
    for ( int i=halfSize; i<2*halfSize+1; i++ )
    {
        pIn_.push_back( UNKNOWN + (OCCUP-UNKNOWN) * N[i] );
    }
    // fix boundaries
    pIn_[0] = EMPTY;
    pIn_[2*halfSize] = UNKNOWN;
    
    // maxed-out range
    for ( int i=0; i<halfSize; i++ )
    {
        pOut_.push_back( EMPTY + (UNKNOWN-EMPTY) * N[i] );
    }
    for ( int i=halfSize; i<2*halfSize+1; i++ )
    {
        pOut_.push_back( UNKNOWN );
    }
    // fix boundaries
    pOut_[0] = EMPTY;
    pOut_[2*halfSize] = UNKNOWN;
    
    
    cout << "OgSensorModel: Sensor model initialized" << endl;
}

OgSensorModel::~OgSensorModel()
{
}

/*!
    PZX = LIKELIHOOD( D, R, Sp, Sh )
    @param D -  distance from vehicle to cell to be updated (not the cell from which sensor_ return came from )
    @param R  - range of the return
    @param Sp - vehicle position std deviation from covariance rotated in the direction of the return
    @param Sh - vehicle heading std deviation

    @todo clean up this function.
*/
double
OgSensorModel::likelihood( const double rangeToCell, const double rangeToReturn, const double Sp, const double Sh)
{
    //cout<<"OgSensorModel:likelihood: range to cell:"<<rangeToCell<<" range to return:"<<rangeToReturn<<" std:"<<Sp<<endl;
    
    // effective variance on observation combining sensor error (Ss) and position error (Sp)
    // config_.rangeStDev = sqrt( Ss^2 + Sp^2 )
    double SrEff = sqrt( config_.rangeStDev*config_.rangeStDev + Sp*Sp );
    
    // distance between return range and cell range from the vehicle,
    // normalized by the sensor variance
    double ro = (rangeToCell - rangeToReturn) / SrEff;
    
    // look up sensor model
    int halfSize = (config_.size-1 ) / 2;
    double r = CHECK_LIMITS( 2.0*halfSize, (ro/config_.rangeStDevMax+1.0) * (double)halfSize, 0.0 );
    int ri = (int) floor( r );
    double d = r - ri;
    
    //cout<<"OgSensorModel:likelihood: looking up element "<<ri<<"("<<ro<<" sigmas)"<<endl;

    double pzx;

    if ( rangeToReturn < config_.rangeMax )
    {
        //pzx = pIn_[ri];
        pzx = pIn_[ri] * (1-d) + pIn_[ri+1] * d;
    }
    else
    {
        //pzx = pOut_[ri];
        pzx = pOut_[ri] * (1-d) + pOut_[ri+1] * d;
    }
    
    //if ( pzx>0.5) std::cout<<"OgSensorModel:likelihood: I see obstacle "<<pzx<<std::endl;

    // squish towards 0.5 for bad bearing uncertainty
    // only for observations with heading 2 sigma bounds
    // greater than the angleIncrement of the scans

    double Sh2 = 2.0 * Sh;
    if(Sh2 > config_.angleIncrement ){
        // how many SD's are we worse by ?
        double d=(Sh2-config_.angleIncrement)/Sh2;
        // gaussian decay towards zero
        double squish=exp(-d*d);
        // squish towards 0.5
        pzx = (pzx-0.5)*squish + 0.5;
    }

    return pzx;
}

void
OgSensorModel::print()
{
    printf("range [std] \tL in range \tL out of range\n");
    for ( uint i=0; i<r_.size(); ++i )
    {
        printf("%4.2f\t%8.6F\t%4.2F\n",r_[i],pIn_[i],pOut_[i]);
    }
}
