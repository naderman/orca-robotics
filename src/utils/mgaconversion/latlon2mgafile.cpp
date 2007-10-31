/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <fstream>
#include <iostream>
#include <iomanip>

#include <orcagpsutil/latlon2mga.h>

using namespace std;
using namespace orcagpsutil;

#define USAGE_ARGS " <inputfile> <ouputfile>"

int main( int argc, char **argv )
{
    if (argc<3) { 
        cout << "Usage: " << argv[0] << USAGE_ARGS << endl;
        exit(1);
    }
    
    string inputFile = argv[1];
    string outputFile = argv[2];
    double longitude, latitude;
    double easting, northing;
    int zone;
    
    ifstream in(inputFile.c_str());
    if (!in) {
        cout << "Error: Input file " << inputFile << " could not be opened" << endl;
        exit(1);
    }
    ofstream out(outputFile.c_str());
    if (!out) {
        cout << "Error: Output file " << outputFile << " could not be created" << endl;
    }
    out << setiosflags(ios::fixed) << setprecision(3);
        
    while ( in >> latitude)
    {
        in >> longitude;
        LatLon2MGA( latitude, longitude, northing, easting, zone );
        out << easting << " " << northing << "\n";
    }
    
    cout << "Successfully converted latitude/longitude values to MGA. See " << outputFile << " for results." << endl;
    
    return 0;
}
