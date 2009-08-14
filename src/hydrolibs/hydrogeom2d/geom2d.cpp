/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "geom2d.h"
#include <iostream>
#include <sstream>
#include <assert.h>
#include <gbxutilacfr/gbxutilacfr.h>

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

std::string toString( const Rect &rect )
{
    stringstream ss;
    ss << "{"<<rect.bottomLeft()<<", "<<rect.topRight()<<"}";
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

    // translate by -a1,-b1 to put the first circle on the origin
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

namespace {

    inline void setLineParams( const Point &p1,
                               const Point &p2,
                               double &a,
                               double &b,
                               double &c )
    {
        if ( p1.x()==p2.x() && p1.y()==p2.y() )
        {
            stringstream ss;
            ss << "Attempt to create line with coincident points at " << p1;
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }

        a = p2.y() - p1.y();
        b = p1.x() - p2.x();
        c = -a*p1.x() - b*p1.y();

        // Normalise
        double len = hypot(a,b);
        a /= len;
        b /= len;
        c /= len;
    }

}

Line::Line( const Point &p1, const Point &p2 )
{ setLineParams( p1, p2, a_, b_, c_ ); }

Line::Line( const Point &p, double vx, double vy )
{ setLineParams( p, Point( p.x()+vx, p.y()+vy ), a_, b_, c_ ); }

double dist( const Line &l, const Point &p )
{
    // The 'hypot' is unnecessary since the line is normalised
    return fabs( l.a()*p.x() + l.b()*p.y() + l.c() );
//    return fabs( l.a()*p.x() + l.b()*p.y() + l.c() ) / hypot( l.a(), l.b() );
}

Point 
closestPointOnLine( const Line &line, const Point &point )
{
    double unitVectorPerpendicular[2];
    line.getUnitVectorPerpendicular( unitVectorPerpendicular );
    Line perpLineThruPoint( point, unitVectorPerpendicular[0], unitVectorPerpendicular[1] );

    // Don't bother checking return value of lineLineIntersection, we know they intersect.
    Point p;
    lineLineIntersection( line, perpLineThruPoint, p );
    return p;
}

bool
lineLineIntersection( const Line &l1, const Line &l2, Point &p )
{
    // Copied from: http://www.topcoder.com/tc?module=Static&d1=tutorials&d2=geometry2

    double det = l1.a()*l2.b() - l2.a()*l1.b();
    if ( det == 0 )
    {
        // Lines are parallel
        return false;
    }

    p.x() = ( l1.b()*l2.c() - l2.b()*l1.c() ) / det;
    p.y() = ( l2.a()*l1.c() - l1.a()*l2.c() ) / det;
    return true;
}

void
convertToRhoAlpha( const Line &line, double &rho, double &alpha )
{
    Point p = closestPointToOrigin( line );
    rho   = hypot( p.y(), p.x() );
    alpha = atan2( p.y(), p.x() );
}

double
rho( const Line &line )
{
    Point p = closestPointToOrigin( line );
    return hypot( p.y(), p.x() );
}

double
alpha( const Line &line )
{
    Point p = closestPointToOrigin( line );
    return atan2( p.y(), p.x() );
}

std::string toString( const Line &l )
{
    stringstream ss;
    ss << l.a() << "x + " << l.b() << "y + " << l.c() << " = 0";
    return ss.str();
}

}

