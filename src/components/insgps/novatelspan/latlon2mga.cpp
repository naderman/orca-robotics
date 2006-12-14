/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Matthew Ridley
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <cmath>
using std::sqrt;
using std::floor;
using std::cos;
using std::sin;
using std::tan;
using std::pow;

#include <iostream>

#include "latlon2mga.h"

namespace insgps{

// LatLon_2_MGA convert (lat,lon) in degrees to (Northing, Easting) in meters
void LatLon2MGA(const double& lat, const double& lon, double& Northing, double& Easting, int& Zone, EGeodModel geodmodel) {
	const double Lon_WE_Z0 = -186.0;								// [°], Longitude of western edge of zone zero
	const double ZoneWidth = 6.0;									// [°], inverse zone width
	const double inv_ZoneWidth = 1.0/ZoneWidth;				// [1/°], inverse zone width
	const double Lon_cM_Z0 = Lon_WE_Z0 + 0.5*ZoneWidth;	// [°], Longitude of the central meridian of zone 0
	const double False_Easting = 500000.0;						// [m], False Easting 
	const double False_Northing = 10000000.0;					// [m], False Northing 
	const double K0 = 0.9996;							    		// Central Scale Factor

	const double inv_6 = 1.0/6.0;
	const double inv_24 = 1.0/24.0;
	const double inv_120 = 1.0/120.0;
	const double inv_720 = 1.0/720.0;
	const double inv_5040 = 1.0/5040.0;
	const double inv_40320 = 1.0/40320.0;

	const double D2R = M_PI/180.0;								// degrees -> radians conversion factor

	static EGeodModel GeodModel = GM_UNDEFINED;	// Geod model
	
	static double a;					// [m], Semi major axis
	static double b;					// [m], Semi minor axis
	static double f;					// flatening
	static double e;					// Eccentricity
	static double e2, e4, e6;		// Eccentricity powers
	static double A0, A2, A4, A6;	// Meridian Distance calculation parameters

	double LonCM;				// Longitude of the central meridian of calculation zone
	double cLat, cLat2, cLat4, cLat6;
	double sLat, sLat2;
	double tLat, tLat2, tLat4, tLat6;
	double omega, omega2, omega4, omega6, omega8;
	double m, nu, Lat, Lon;
	// double rho; 
    double t1, t2, t3, t4;
	double psi, psi2, psi3, psi4;
	
	if (GeodModel != geodmodel) {
		GeodModel = geodmodel;
		
		// Get Elipsoid Parameters, a = Semi major axis, f = flatening
		switch(GeodModel) {
			case GM_WGS84:
				a = 6378137.0;
				f = 1.0/298.257223563;
      	break;
      	
			case GM_GDA94:
				a = 6378137.0;
				f = 1.0/298.257222101;
      	break;
      	
			case GM_AGD84:
				a = 6378160.0;
				f = 1.0/298.25;
      	break;
      	
			case GM_WGS72:
				a = 6378135.0;
				f = 1.0/298.26;
      	break;
      	
			case GM_NSWC_9Z2:
				a = 6378145.0;
				f = 1.0/298.25;
      	break;
      	
			case GM_Clarke:
				a = 20926348.0*0.3048;
				f = 1.0/294.26;
      	break;
            default:
                std::cout << "The parameters for this GeoModel type have not been implemented" << std::endl;
        break;                  
      }

		// Semi minor axis (m)
		b = a*(1.0-f);

		// Eccentricity
		e = sqrt(2.0*f - f*f);
		e2 = e*e;
		e4 = e2*e2;
		e6 = e4*e2;

		// Coeficients
		A0 = 1.0 - (e2/4.0) - (3.0*e4/64.0) - (5.0*e6/256.0);
		A2 = (3.0/8.0)*(e2 + e4/4.0 + 15*e6/128.0);
		A4 = (15.0/256.0)*(e4 + 3.0*e6/4.0);
		A6 = 35.0*e6/3072.0;
	}

	// calculate zone parameters
	Zone = (int)floor((lon - Lon_WE_Z0)*inv_ZoneWidth);
	LonCM = (Lon_cM_Z0 + ZoneWidth*(double)Zone)*D2R;

	// Convert to radian
	Lat = lat*D2R;
	Lon = lon*D2R;

	// Convert to MGA 
	cLat = cos(Lat);
	sLat = sin(Lat);
	tLat = tan(Lat);

	omega = Lon - LonCM;

	// Powers
  	cLat2 = cLat*cLat;
	cLat4 = cLat2*cLat2;
	cLat6 = cLat4*cLat2;
	sLat2 = sLat*sLat;
	tLat2 = tLat*tLat;
	tLat4 = tLat2*tLat2;
	tLat6 = tLat4*tLat2;
	omega2 = omega*omega;
	omega4 = omega2*omega2;
	omega6 = omega4*omega2;
	omega8 = omega4*omega4;

  	// Meridian Radius
	m = a*(A0*Lat - A2*sin(2.0*Lat) + A4*sin(4.0*Lat) - A6*sin(6.0*Lat));

  	// Radius of Curvature
  	t1 = 1.0 - e2*sLat2;
//	rho = a*(1.0 - e2)/(pow((1.0 - e2*sLat2),1.5));
	nu =  a/(pow(t1,0.5));	// = a/(pow((1.0 - e2*sLat2),0.5));
	psi = t1/(1.0 - e2);		// = nu/rho;
	psi2 = psi*psi;
	psi3 = psi2*psi;
	psi4 = psi2*psi2;

	// Easting
	t1 = (inv_6*omega2)*cLat2*(psi - tLat2);
	t2 = (inv_120*omega4)*cLat4*(4.0*psi3*(1.0 - 6.0*tLat2) + psi2*(1.0 + 8.0*tLat2) - 2.0*psi*tLat2 + tLat4);
	t3 = (inv_5040*omega6)*cLat6*(61.0 - 479.0*tLat2 + 179.0*tLat4 - tLat6);

//	double Ehat = (K0*nu*omega*cLat)*(1.0 + t1 + t2 + t3);
//	Easting = Ehat + False_Easting;
	Easting = (K0*nu*omega*cLat)*(1.0 + t1 + t2 + t3) + False_Easting;

	// Northing
  	t1 = (0.5*omega2);
	t2 = (inv_24*omega4)*cLat2*(4.0*psi2 + psi - tLat2);
	t3 = (inv_720*omega6)*cLat4*(8.0*psi4*(11.0 - 24.0*tLat2) - 28.0*psi3*(1.0-6.0*tLat2) + psi2*(1.0-32.0*tLat2) - psi*(2.0*tLat2) + tLat4);
	t4 = (inv_40320*omega8)*cLat6*(1385.0 - 3111.0*tLat2 + 543.0*tLat4 - tLat6);

//	double Nhat = K0*(m + nu*sLat*cLat*(t1 + t2 + t3 + t4));
//	Northing = Nhat + False_Northing;
	Northing = K0*(m + nu*sLat*cLat*(t1 + t2 + t3 + t4)) + False_Northing;
}

// MGA_2_LatLon convert (Northing, Easting) in meters to (lat,lon) in degrees
void MGA2LatLon(const double& Northing, const double& Easting, const int& Zone, double& lat, double& lon, EGeodModel geodmodel) {
	const double Lon_WE_Z0 = -186.0;								// [°], Longitude of western edge of zone zero
	const double ZoneWidth = 6.0;									// [°], inverse zone width
//	const double inv_ZoneWidth = 1.0/ZoneWidth;				// [1/°], inverse zone width
	const double Lon_cM_Z0 = Lon_WE_Z0 + 0.5*ZoneWidth;	// [°], Longitude of the central meridian of zone 0
	const double False_Easting = 500000.0;						// [m], False Easting
	const double False_Northing = 10000000.0;					// [m], False Northing
	const double K0 = 0.9996;							    		// Central Scale Factor

	const double c9_4 = 9.0/4.0;
	const double c225_64 = 225.0/64.0;
	const double c27_32 = 27.0/32.0;
	const double c21_16 = 21.0/16.0;
	const double c55_32 = 55.0/32.0;
	const double c151_96 = 151.0/96.0;
	const double c1097_512 = 1097.0/512.0;

	const double inv_6 = 1.0/6.0;
	const double inv_24 = 1.0/24.0;
	const double inv_120 = 1.0/120.0;
	const double inv_720 = 1.0/720.0;
	const double inv_5040 = 1.0/5040.0;
	const double inv_40320 = 1.0/40320.0;

	const double D2R = M_PI/180.0;
	const double R2D = 180.0/M_PI;

	static EGeodModel GeodModel = GM_UNDEFINED;	// Geod model
	
	static double a;					// [m], Semi major axis
	static double b;					// [m], Semi minor axis
	static double f;					// flatening
	static double e;					// Eccentricity
	static double e2, e4, e6;		// Eccentricity powers
	static double n, n2, n3, n4, G;

	double LonCM;				// Longitude of the central meridian of calculation zone
	double psip, psip2, psip3, psip4;
	double tp, tp2, tp4, tp6;
	double m, sigma, phip, sphip2, rhop, nup, Ep, EtKr, Secphip;
	// double sphip;
    double x, x3, x5, x7;
	double t1, t2, t3, t4;
	double Lat, Lon;
	
	if (GeodModel != geodmodel) {
		GeodModel = geodmodel;

		// Get Elipsoid Parameters, a = Semi major axis, f = flatening
		switch(GeodModel) {
			case GM_WGS84:
				a = 6378137.0;
				f = 1.0/298.257223563;
      	break;
      	
			case GM_GDA94:
				a = 6378137.0;
				f = 1.0/298.257222101;
      	break;
      	
			case GM_AGD84:
				a = 6378160.0;
				f = 1.0/298.25;
      	break;
      	
			case GM_WGS72:
				a = 6378135.0;
				f = 1.0/298.26;
      	break;
      	
			case GM_NSWC_9Z2:
				a = 6378145.0;
				f = 1.0/298.25;
      	break;
      	
			case GM_Clarke:
				a = 20926348.0*0.3048;
				f = 1.0/294.26;
      	break;
            default:
                std::cout << "The parameters for this GeoModel type have not been implemented" << std::endl;
        break;                  
		}
      
		// Semi minor axis (m)
		b = a*(1.0-f);

		// Eccentricity
		e = sqrt(2.0*f - f*f);
		e2 = e*e;
		e4 = e2*e2;
		e6 = e4*e2;

		n = f/(2.0 - f);	// = (a-b)/(a+b)
		n2 = n*n;
		n3 = n*n2;
		n4 = n2*n2;
		G = a*(1.0 - n)*(1.0 - n2)*(1.0 + c9_4*n2 + c225_64*n4)*D2R;
	}

	// calculate zone parameters
	LonCM = (Lon_cM_Z0 + ZoneWidth*(double)Zone)*D2R;

  	// Meridian Radius
//	Np = Northing - False_Northing;
	m = (Northing - False_Northing)/K0;

	// Foot-point Latitude
	sigma = m/G*D2R;
	phip = sigma + (1.5*n - c27_32*n3)*sin(2.0*sigma) + (c21_16*n2 - c55_32*n4)*sin(4.0*sigma) + c151_96*n3*sin(6.0*sigma) + c1097_512*n4*sin(8.0*sigma);
	
  	// Radius of Curvature
  	sphip2 = sin(phip);
  	sphip2 *= sphip2;
	rhop = a*(1.0 - e2)/(pow((1.0 - e2*sphip2),1.5));
	nup =  a/(pow((1.0 - e2*sphip2),0.5));
	psip = nup/rhop;
	psip2 = psip*psip;
	psip3 = psip2*psip;
	psip4 = psip2*psip2;
  	tp = tan(phip);
	tp2 = tp*tp;
	tp4 = tp2*tp2;
	tp6 = tp2*tp4;

	// Latitude
	Ep = Easting - False_Easting;
	EtKr = Ep*tp/(K0*rhop);
	x = Ep/(K0*nup);
	x7 = x*x;	// used as temp, x2
	x3 = x7*x;
	x5 = x3*x7;
	x7 *= x5;
	
	t1 = 0.5*x*EtKr;
	t2 = inv_24*x3*EtKr*(-4.0*psip2 + 9.0*psip*(1.0 - tp2) + 12.0*tp2);
	t3 = inv_720*x5*EtKr*(8.0*psip4*(11.0 - 24.0*tp2) - 12.0*psip3*(21.0 - 71.0*tp2) + 15.0*psip2*(15.0 - 98.0*tp2 + 15.0*tp4) + 180.0*psip*(5.0*tp2 - 3.0*tp4) + 360.0*tp4);
	t4 = inv_40320*x7*EtKr*(1385.0 + 3633.0*tp2 + 4095.0*tp4 + 1575.0*tp6);
    // The following line use to be:	
    // lat = phip - t1 + t2 - t3 + t4;
    // which didn't make sense as lat was assigned two different values twice. Changed it so
    // that compiler warnings were removed but hasn't been tested         
    Lat = phip - t1 + t2 - t3 + t4;
	lat = Lat*R2D;

	// Longitude
	Secphip = 1.0/cos(phip);
  	t1 = x*Secphip;
	t2 = inv_6*x3*Secphip*(psip + 2.0*tp2);
	t3 = inv_120*x5*Secphip*(psip3*(-4.0 + 24.0*tp2) + psip2*(9.0 - 68.0*tp2) + 72.0*psip*tp2 + 24.0*tp4);
	t4 = inv_5040*x7*Secphip*(61.0 + 662.0*tp2 + 1320.0*tp4 + 720.0*tp6);
    // The following line use to be:    
    // lon = LonCM + t1 - t2 + t3 - t4;
    // which didn't make sense as lon was assigned two different values twice. Changed it so
    // that compiler warnings were removed but hasn't been tested         
    Lon = LonCM + t1 - t2 + t3 - t4;
	lon = Lon*R2D;
}

} //namespace
