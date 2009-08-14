#include <iostream>
#include <hydronavutil/cov3d.h>
#include <cmath>
#include <cstdlib>

using namespace std;
using namespace hydronavutil;

bool close( double a, double b )
{
    return (fabs(a-b) < 1e-2);
}

int main()
{
    Cov3d c( 2, 1, 0.5, 3, 0.75, 4 );
    cout<<"TRACE(testcov3d.cpp): c: " << endl << c << endl;

    double d = c.det();
    cout<<"TRACE(testcov3d.cpp): det: " << d << endl;
    if ( !close( d, 18.875 ) )
    {
        cout << "ERROR(testcov3d.cpp): failed det." << endl;
        exit(1);
    }

    Cov3d inv = c.inverse();
    cout<<"TRACE(testcov3d.cpp): inv: " << endl << inv << endl;
    if ( ! ( close(inv.xx(),0.6060) &&
             close(inv.xy(),-0.1921) &&
             close(inv.xt(),-0.0397) &&
             close(inv.yy(),0.4106) &&
             close(inv.yt(),-0.0530) &&
             close(inv.tt(),0.2649) ) )
    {
        cout << "ERROR(testcov3d.cpp): inv failed" << endl;
        cout << "close(inv.xx(),0.6060): " <<  close(inv.xx(),0.6060) << endl;
        cout << "close(inv.xy(),-0.1921):" << close(inv.xy(),-0.1921)<< endl;
        cout << "close(inv.xt(),-0.0397):" << close(inv.xt(),-0.0397)<< endl;
        cout << "close(inv.yy(),0.4106): " <<  close(inv.yy(),0.4106) << endl;
        cout << "close(inv.yt(),-0.0530):" << close(inv.yt(),-0.0530)<< endl;
        cout << "close(inv.tt(),0.2649): " << close(inv.tt(),0.2649) << endl;
        exit(1);
    }
             
    double g = c.gauss(1,2,3);
    cout<<"TRACE(testcov3d.cpp): gauss(1,2,3): " << g << endl;
    if ( !close(g,0.0033) )
    {
        cout << "ERROR(testcov3d.cpp): Failed gauss test." << endl;
        exit(1);
    }

    cout<<"TRACE(testcov3d.cpp): test PASSED" << endl;

    return 0;
}
