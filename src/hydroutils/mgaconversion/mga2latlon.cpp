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

#define USAGE_ARGS "<northing> <easting> <zone>"

int main( int argc, char **argv )
{
    if (argc<4) { 
        cout << "Usage: " << argv[0] << USAGE_ARGS << endl;
        exit(1);
    }
    
    stringstream ss;
    double northing, easting;
    int zone;
    ss << argv[1] << " " << argv[2] << " " << argv[3];
    ss >> northing >> easting >> zone;
    
    double lat, lon;
    hydrogpsutil::MGA2LatLon( northing, easting, zone, lat, lon );
    
    cout    << setiosflags(ios::fixed) << setprecision(16)
            << lat << " " << lon << endl;
    
    return 0;
}
