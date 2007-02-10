/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <iomanip>
#include <sstream>

#include <orcanavutil/mapgrid.h>


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
    
    mgaMapgrid mgaGrid;
    double easting, northing;
    int ret = mgaGrid.getGridCoords( latitude, longitude, easting, northing );
    
    if (ret==-1) {
        cout << "Can't convert. Wrong zone" << endl;
        exit(1);
    }
    
    cout    << setiosflags(ios::fixed) << setprecision(3)
            << "Latitude:\t" << latitude << endl
            << "Longitude:\t" << longitude << endl
            << "Easting:\t" << easting << endl
            << "Northing:\t" << northing << endl;
    
    return 0;
}
