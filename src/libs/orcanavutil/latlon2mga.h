/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Matthew Ridley
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_NOVATELSPAN_INSGPS_LATLON2MGA_H
#define ORCA2_NOVATELSPAN_INSGPS_LATLON2MGA_H

namespace insgps{
    
// enumeration of geod models
    enum EGeodModel { GM_UNDEFINED=-1, GM_WGS84 =0, GM_GDA94, GM_WGS72, GM_AGD84, GM_NSWC_9Z2, GM_Clarke, GM_NoOptions, GM_GRS80=GM_GDA94, GM_ANS=GM_AGD84 };

    typedef struct {
        double a;                                       // Semi major axis (m)
        double b;                                       // Semi minor axis (m)
        double f;                                       // flatening
        double e;                                       // Eccentricity
        double e2;                                      // Eccentricity^2
        double A0, A2, A4, A6;  // Meridian Distance calculation parameters
    } TGeoModelData;

    // LatLon_2_MGA convert (lat,lon) in degrees to (Northing, Easting) in meters
    void LatLon2MGA(const double& lat, const double& lon, double& Northing, double& Easting, int& Zone, EGeodModel geodmodel = GM_WGS84);

    // MGA_2_LatLon convert (Northing, Easting) in meters to (lat,lon) in degrees
    void MGA2LatLon(const double& Northing, const double& Easting, const int& Zone, double& lat, double& lon, EGeodModel geodmodel = GM_WGS84);

} //namespace

#endif
