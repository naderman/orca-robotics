/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <hydrogpsutil/latlon2mga.h>

using namespace std;

#define USAGE_ARGS "<latitude> <longitude>"

int main( int argc, char **argv )
{
    if (argc<3) { 
        cout << "Usage: " << argv[0] << USAGE_ARGS << endl;
        exit(1);
    }
    
    stringstream ss;
    double latitude, longitude;
    ss << argv[1] << " " << argv[2];
    ss >> latitude >> longitude;
    
    double easting, northing;
    int zone;
    hydrogpsutil::LatLon2MGA( latitude, longitude, northing, easting, zone );
    
    cout    << setiosflags(ios::fixed) << setprecision(16)
            << "Latitude:\t" << latitude << endl
            << "Longitude:\t" << longitude << endl
            << "Easting:\t" << easting << endl
            << "Northing:\t" << northing << endl;
    
    return 0;
}
