/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef _MAPGRID_H_
#define _MAPGRID_H_
/*!
 Base class for Map Grid conversions

 In particular, Map Grids that are based on the WGS84/GRS80 ellipsoid and a
 Transverse Mercator projection.

 e.g. The Map Grid of Australia (MGA) based on the
 Geocentric Datum of Australia '94 (GDA94) ellipsoid which is
 more or less equivalent to the WGS84 ellipsoid.

 http://www.icsm.gov.au/icsm/gda/index.html

 http://www.deh.gov.au/erin/tools/mga2geo-gda.html

 */

typedef struct ellipsoid{
    double semiMajorAxis;
    double inverseFlattening;
}ellipsoid;

extern const ellipsoid WGS84;
extern const ellipsoid AGD84;
extern const ellipsoid GRS80;


class mapgrid{
public:
    mapgrid();
    virtual ~mapgrid();

    void setEllipsoid(ellipsoid eps);
    virtual int getZone(double latitude,  double longitude)=0;

    /*! return the calculated zone after setting easting and northing */
    virtual int getGridCoords( double latitude,  double longitude,
		      double& easting, double& northing)=0;
    /*! set easting and northing for a specific zone
     return -1 if point not in adjacent zone */
    virtual int getZoneGridCoords(double latitude,  double  longitude, int zone,
				  double& easting, double& northing)=0;
protected:
    // The ellipsoid parameters
    // semi-major axis
    double a;
    //inverse flattening
    double f_i;
    // semi-minor axis
    double b;
    //flattening
    double f;
    // eccentricity squared
    double e2;
    // eccentricity
    double e;

};


class mgaMapgrid: public mapgrid{
public:
    mgaMapgrid();
    ~mgaMapgrid();

    /*! return the calculated zone after setting easting and northing */
    virtual int getGridCoords( double latitude, double longitude,
		      double& easting, double& northing);
    /*! set easting and northing for a specific zone
     return -1 if point not in adjacent zone */
    virtual int getZoneGridCoords(double latitude, double longitude, int zone,
				  double& easting, double& northing);
    /*! calculate zone for lat lon
        return -1 if wrong;
     */
    virtual int getZone(double latitude, double longitude);
private:
    //map parameters
    double  centralScaleFactor;
    double  falseEasting;
    double  falseNorthing;
    double  zoneWidth;
    // Longitude of initial central meridian (Zone one)
    double  cmz1;

};

#endif
