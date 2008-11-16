#include <iostream>
#include <orcapotrace/orcapotrace.h>
#include <hydromapload/hydromapload.h>
#include <stdlib.h>

using namespace std;

int main( int argc, char **argv )
{
    if ( argc != 2 )
    {
        cout << "USAGE: " << argv[0] << " <map_filename>" << endl;
        exit(1);
    }
    std::string mapFileName = argv[1];

    //
    // Load a map file
    //
    hydroogmap::OgMap ogMap;
    const double offsetX=-1.0, offsetY=-10.0, offsetTheta=0, metresPerCellX=0.1, metresPerCellY=0.1;
    hydromapload::loadMap( mapFileName,
                           ogMap,
                           false,
                           offsetX,
                           offsetY,
                           offsetTheta,
                           metresPerCellX,
                           metresPerCellY  );

    //
    // Trace it!
    //
    orcapotrace::PotraceBitmap bitmap( ogMap );

    cout<<"TRACE(simple.cpp): test PASSED" << endl;
    return 0;
}
