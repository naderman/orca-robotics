/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
// Ensure that assertions are compiled in
#undef NDEBUG

#include <iostream>
#include <cstdlib>
#include <hydropathplan/hydropathplan.h>

using namespace std;
using namespace hydropathplan;

int main( int argc, char **argv )
{
    const int sx = 10;
    const int sy = 10;
    
    for ( int y=0; y < sy; y++ )
    {
        for ( int x=0; x < sx; x++ )
        {
            cout<<"-----------------------------------" << endl;
            Cell2D c(x,y);
            int s2i = latombeutil::sub2ind( c, sx, sy );
            Cell2D i2s = latombeutil::ind2sub( s2i, sx, sy );
            int andback = latombeutil::sub2ind( i2s, sx, sy );
            cout<<"TRACE(testconversions.cpp): Cell:  " << Cell2D(x,y) << endl;
            cout<<"TRACE(testconversions.cpp): s2i:   " << s2i << endl;
            cout<<"TRACE(testconversions.cpp): i2s:   " << i2s << endl;
            cout<<"TRACE(testconversions.cpp): andback: " << andback << endl;       
    
            if ( (c != i2s) || ( s2i != andback ) ) 
            {
                cout << "ERROR(testconversions.cpp): sub2ind and/or ind2sub do the wrong thing!" << endl;
                exit(1); 
            }
        }
    }
    
    cout << endl << "TRACE(testconversions.cpp): Everything went well. Test passed." << endl;
    
    return 0;
}

