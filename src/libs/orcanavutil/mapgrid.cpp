/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <stdlib.h>
#include <cmath>
// M_PI is not defined after including cmath for the MS visual studio compiler?
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "mapgrid.h"

const ellipsoid WGS84 = {6378137.0 , 298.257223563};
const ellipsoid AGD84 = {6378160.0 , 298.25};
const ellipsoid GRS80 = {6378137.0 , 298.257222101};


mapgrid::mapgrid()
{
    setEllipsoid(WGS84);
}

mapgrid::~mapgrid()
{
}

void mapgrid::setEllipsoid(ellipsoid eps)
{
    // semi-major axis
    a =  eps.semiMajorAxis;
    //inverse flattening
    f_i = eps.inverseFlattening;
    // flattening
    f = 1.0/f_i;
    // semi-minor axis
    b =  a*(1.0-f);

    // eccentricity squared
    //e2 = (a*a - b*b)/(a*a);
    e2 = (2.0 - f) * f;
    // eccentricity
    e = sqrt(e2);

}

mgaMapgrid::mgaMapgrid()
{
    centralScaleFactor=0.9996;
    falseEasting=500000.0;
    falseNorthing=10000000.0;
    zoneWidth=6.0;   //degrees
    // Longitude of initial central meridian (Zone one)
    cmz1=-177;
    setEllipsoid(GRS80);

}

mgaMapgrid::~mgaMapgrid()
{
}

int mgaMapgrid::getGridCoords(double latitude, double longitude,
                              double& easting, double& northing)
{
    int zone=getZone(latitude,  longitude);
    if(zone<0)
	return -1;
    else{
        int ret;
        ret=getZoneGridCoords(latitude, longitude, zone,
			      easting,  northing);
        return ret;
    }

}

int mgaMapgrid::getZone(double latitude,  double longitude)
{
    // calculate zone
    int zone = int((longitude-cmz1+zoneWidth/2.0)/zoneWidth) + 1;

    if(zone<0)
	return -1;
    if(zone>(int)(360.0/zoneWidth))
        return -1;
    return zone;
}

int mgaMapgrid::getZoneGridCoords(double latitude, double longitude, int zone,
                                  double& easting, double& northing)
{
    //check if in neighbouring zone
    int true_zone=getZone(latitude,  longitude);
    if(abs(zone-true_zone) > 1)
	return -1;

    double deg2rad = M_PI/180.0;

    double lat = latitude*deg2rad;

    // Meridian distance
    double e4 = e2*e2;
    double e6 = e4*e2;
    double A0 = 1-(e2/4.0)-(3.0*e4/64.0)-(5.0*e6/256.0);
    double A2 = (3.0/8.0)*(e2+e4/4.0+15.0*e6/128.0);
    double A4 = (15.0/256.0)*(e4+3.0*e6/4.0);
    double A6 = 35.0*e6/3072.0;
    double s = sin(lat);
    double s2 = sin(2.0*lat);
    double s4 = sin(4.0*lat);
    double s6 = sin(6.0*lat);
    double m = a*(A0*lat-A2*s2+A4*s4-A6*s6);

    // Radii of curvature.
    double rho = a*(1-e2)/pow(1.0-e2*s*s,3.0/2.0);
    double nu = a/sqrt(1-e2*s*s);
    double psi = nu / rho;
    double psi2 = psi*psi;
    double psi3 = psi*psi2;
    double psi4 = psi*psi3;

    // Geographical to Grid
    // longitude of central meridian of zone (degrees)
    double LongCMZ = (double)(zone - 1) * zoneWidth + cmz1;
    // the arc distance from central meridian (radians)
    double w = (longitude - LongCMZ)*deg2rad;
    double w2 = w*w;
    double w3 = w*w2;
    double w4 = w*w3;
    double w5 = w*w4;
    double w6 = w*w5;
    double w7 = w*w6;
    double w8 = w*w7;

    double c = cos(lat);
    double c2 = c*c;
    double c3 = c*c2;
    double c4 = c*c3;
    double c5 = c*c4;
    double c6 = c*c5;
    double c7 = c*c6;

    double t = tan(lat);
    double t2 = t*t;
    double t3 = t*t2;
    double t4 = t*t3;
    double t5 = t*t4;
    double t6 = t*t5;

    // Northing
    double term1 = w2*c/2.0;
    double term2 = w4*c3*(4.0*psi2+psi-t2)/24.0;
    double term3 = w6*c5*(8.0*psi4*(11.0-24.0*t2)-28*psi3*(1-6.0*t2)+psi2*(1-32*t2)-psi*(2.0*t2)+t4)/720.0;
    double term4 = w8*c7*(1385.0-3111.0*t2+543.0*t4-t6)/40320.0;
    northing = centralScaleFactor*(m+nu*s*(term1+term2+term3+term4))+falseNorthing;

    // Easting
    term1 = w*c;
    term2 = w3*c3*(psi-t2)/6.0;
    term3 = w5*c5*(4.0*psi3*(1.0-6.0*t2)+psi2*(1.0+8.0*t2)-psi*(2.0*t2)+t4)/120.0;
    term4 = w7*c7*(61.0-479.0*t2+179.0*t4-t6)/5040.0;
    easting = nu*centralScaleFactor*(term1+term2+term3+term4)+falseEasting;

    return zone;
}
