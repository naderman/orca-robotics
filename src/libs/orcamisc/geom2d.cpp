#include "geom2d.h"
#include <iostream>
#include <cmath>
#include <orcaobj/mathdefs.h>

namespace orcamisc {

    void findLineEquation( double x1,
                           double y1,
                           double x2,
                           double y2,
                           double &a,
                           double &b,
                           double &c )
    {
        assert( !(x1==x2 && y1==y2) );

        double xDiff = x2-x1;
        double yDiff = y2-y1;

        // Two cases to avoid infinite slope issue
        if ( fabs(xDiff) > fabs(yDiff) )
        {
            b = 1;
            a = -yDiff/xDiff;
        }
        else
        {
            a = 1;
            b = -xDiff/yDiff;
        }
        c = -a*x1-b*y1;
    }

    void pointsToRhoAlpha( double x1,
                           double y1,
                           double x2,
                           double y2,
                           double &rho,
                           double &alpha )
    {
        // Find the line equation
        double a,b,c;
        findLineEquation( x1,x2,y1,y2, a, b, c );

        // Find the meeting between this line and its perpendicular bisector from the origin
        // Call this point (px,py).
        double d = a*a+b*b;
        double px = -a*c / d;
        double py = -b*c / d;

        // Convert to rho-alpha
        rho   = hypotf(py,px);
        alpha = atan2(py,px);
    }

    void projectPointOntoLine( double rho,
                               double alpha,
                               double &x,
                               double &y )
    {
        // Need a point and a vector defining (rho,alpha)
        // These are pt'=(rho,0) and v'=(0,1), rotated by alpha:
        double ptx = rho*cos(alpha);
        double pty = rho*sin(alpha);
        
        double vx = -sin(alpha);
        double vy =  cos(alpha);

        // Then project onto this line.
        double u = (x-ptx)*vx + (y-pty)*vy;

        x = ptx + u*vx;
        y = pty + u*vy;
    }

    void projectPointOntoLine( double rho,
                               double alpha,
                               double &x1,
                               double &y1,
                               double &x2,
                               double &y2 )
    {
        // Need a point and a vector defining (rho,alpha)
        // These are pt'=(rho,0) and v'=(0,1), rotated by alpha:
        double ptx = rho*cos(alpha);
        double pty = rho*sin(alpha);
        
        double vx = -sin(alpha);
        double vy =  cos(alpha);

        // Then project onto this line.
        double u1 = (x1-ptx)*vx + (y1-pty)*vy;
        x1 = ptx + u1*vx;
        y1 = pty + u1*vy;

        double u2 = (x2-ptx)*vx + (y2-pty)*vy;
        x2 = ptx + u2*vx;
        y2 = pty + u2*vy;
    }
}
