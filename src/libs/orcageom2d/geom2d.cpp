#include "geom2d.h"
#include <iostream>
#include <sstream>
#include <assert.h>

#ifdef WIN32
	#define _USE_MATH_DEFINES
	#include <math.h>
#endif

using namespace std;

namespace geom2d {

std::string
Point::toString() const
{
    return toStringXY();
}

std::string
Point::toStringXY() const
{
    stringstream ss;
    ss << "("<<x()<<","<<y()<<")";
    return ss.str();
}

std::string
Point::toStringRB() const
{
    stringstream ss;
    ss << "("<<range()<<","<<bearing()*180.0/M_PI<<"deg)";
    return ss.str();
}

std::string
PolarPoint::toString() const
{
    return toStringRB();
}

std::string
PolarPoint::toStringXY() const
{
    stringstream ss;
    ss << "("<<x()<<","<<y()<<")";
    return ss.str();
}

std::string
PolarPoint::toStringRB() const
{
    stringstream ss;
    ss << "("<<range()<<","<<bearing()*180.0/M_PI<<"deg)";
    return ss.str();
}

bool circleCircleIntersection( double a1,
                               double b1,
                               double r1,
                               double a2,
                               double b2,
                               double r2,
                               double &px,
                               double &py,
                               double &qx,
                               double &qy )
{
    if ( r1 < 0 || r2 < 0 )
        throw( std::string("Circle had zero radius!") );
    if ( a1==a2 && b1==b2 && r1==r2 )
        throw( std::string("Co-Incident circles!") );

    // translate by -a1,-b1 to puc the first circle on the origin
    a2 -= a1;
    b2 -= b1;
    {
        // The line through the meeting points can be obtained by subtracting the two
        // equations.  Call this line vx*x + vy*y + vc.
        double vx = -2*a2;
        double vy = -2*b2;
        double vc = a2*a2 + b2*b2 + r1*r1 - r2*r2;

        // DEBUG
        double norm = sqrt(vx*vx+vy*vy);
        vx /= norm;
        vy /= norm;
        vc /= norm;
        // end DEBUG

//        cout<<"TRACE(geom2d.cpp): line: "<<vx<<"x + "<<vy<<"y + "<<vc << endl;

//         // DEBUG
//         // Vector between circle centres
//         Point l( a2-a1, b2-b1 );
//         norm = sqrt(l.x()*l.x() + l.y()*l.y());
//         l.x() /= norm;
//         l.y() /= norm;
//         cout<<"TRACE(geom2d.cpp): vector along: " << l << endl;
//         // end DEBUG
                  
        //
        // Now find the intersection between a circle (the first) and the line.
        // 

        // Avoid divide-by-zero with an 'if'
        if ( fabs(vx) > fabs(vy) )
        {
            // Quadratic in y: a*y^2 + b*y + c = 0
            double a = 1.0; // vx*vx + vy*vy;
            double b = 2*vy*vc;
            double c = vc*vc - vx*vx*r1*r1;

//             cout<<"TRACE(geom2d.cpp): a: " << a << endl;
//             cout<<"TRACE(geom2d.cpp): b: " << b << endl;
//             cout<<"TRACE(geom2d.cpp): c: " << c << endl;

            double discriminant = b*b - 4*a*c;

            if ( discriminant < 0 )
                return false;

            py = ( -b + sqrt(discriminant) ) / 2; // *a;
            qy = ( -b - sqrt(discriminant) ) / 2; // *a;

            // Solve for x from the line equation
            px = (-vy*py-vc)/vx;
            qx = (-vy*qy-vc)/vx;
        }
        else
        {
            // Quadratic in x: a*x^2 + b*y^2 + c = 0
            double a = 1.0; // vx*vx + vy*vy;
            double b = 2*vx*vc;
            double c = vc*vc - vy*vy*r1*r1;

            double discriminant = b*b - 4*a*c;

            if ( discriminant < 0 )
                return false;

            px = ( -b + sqrt(discriminant) ) / 2; // *a;
            qx = ( -b - sqrt(discriminant) ) / 2; // *a;
        
            // Solve for y from line equation
            py = (-vx*px-vc)/vy;
            qy = (-vx*qx-vc)/vy;
        }

// #ifndef NDEBUG
//         cout<<"TRACE(geom2d.cpp): p: " << px << ", " << py << endl;
//         cout<<"TRACE(geom2d.cpp): q: " << qx << ", " << qy << endl;

//         // Sanity check
//         // Check line equation
//         double eq;
//         eq = px*vx + py*vy + vc;  assert( NEAR( eq, 0, 1e-3 ) );
//         eq = qx*vx + qy*vy + vc;  assert( NEAR( eq, 0, 1e-3 ) );
// #endif

    }
    // translate back 
    px += a1;
    py += b1;
    qx += a1;
    qy += b1;

    return true;
}

}
