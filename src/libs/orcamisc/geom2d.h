#ifndef ORCA_GEOM2D_H
#define ORCA_GEOM2D_H

namespace orcamisc {

    //
    // A handful of functions for dealing with points and lines.
    // 
    // @author Alex Brooks
    //

    //! convert from a pair of points to a rho-theta parameterisation
    void pointsToRhoAlpha( double x1,
                           double x2,
                           double y1,
                           double y2,
                           double &rho,
                           double &alpha );

    //! project a single point onto a line
    void projectPointOntoLine( double rho,
                               double alpha,
                               double &x,
                               double &y );

    //! Cheaper to do two at once
    void projectPointOntoLine( double rho,
                               double alpha,
                               double &x1,
                               double &y1,
                               double &x2,
                               double &y2 );

    //!
    //! From two points, (x1,y1) and (x2,y2), find the line equation
    //! ax + by + c = 0
    //!
    void findLineEquation( double x1,
                           double y1,
                           double x2,
                           double y2,
                           double &a,
                           double &b,
                           double &c );

}

#endif
