/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef GEOM2D_H
#define GEOM2D_H

#include <string>
#include <cmath>
#include <iostream>

#include <hydroportability/sharedlib.h>

namespace geom2d {

    //!
    //! A point (represented internally with cartesian coordinates)
    //!
    class SOEXPORT Point {
    public:

        explicit Point() {}
        explicit Point( double x, double y )
            { p_[0] = x; p_[1] = y; }

        double x() const { return p_[0]; }
        double y() const { return p_[1]; }

        double &x() { return p_[0]; }
        double &y() { return p_[1]; }

        // allow treatment as a vector
        double operator[]( int i ) const { return p_[i]; }
        double &operator[]( int i ) { return p_[i]; }

        // convert to polar
        double range() const { return std::sqrt(y()*y()+x()*x()); }
        double bearing() const { return std::atan2(y(),x()); }

        std::string toString()   const;
        std::string toStringXY() const;
        std::string toStringRB() const;

    private:
        double p_[2];
    };

    //!
    //! A rectangle
    //!
    class Rect {
    public:

        Rect( const geom2d::Point &bottomLeft,
              const geom2d::Point &topRight )
            : bottomLeft_(bottomLeft), topRight_(topRight) {}

        geom2d::Point &bottomLeft() { return bottomLeft_; }
        geom2d::Point &topRight() { return topRight_; }
        const geom2d::Point &bottomLeft() const { return bottomLeft_; }
        const geom2d::Point &topRight() const { return topRight_; }

    private:
        // Extents of the rectangle
        geom2d::Point bottomLeft_, topRight_;
    };
    std::string toString( const Rect &rect );
    inline std::ostream &operator<<( std::ostream &s, const Rect &rect )
    { return s << toString(rect); }

    //!
    //! A point (represented internally with polar coordinates)
    //!
    class SOEXPORT PolarPoint {
    public:
        
        explicit PolarPoint() {}
        explicit PolarPoint( double r, double b )
            { p_[0] = r; p_[1] = b; }

        double range()   const { return p_[0]; }
        double bearing() const { return p_[1]; }

        double &range()   { return p_[0]; }
        double &bearing() { return p_[1]; }

        // allow treatment as a vector
        double operator[]( int i ) const { return p_[i]; }

        // convert to cartesian
        double x() const { return range()*std::cos(bearing()); }
        double y() const { return range()*std::sin(bearing()); }

        std::string toString()   const;
        std::string toStringXY() const;
        std::string toStringRB() const;

    private:
        double p_[2];
    };

    //! Distance between points
    inline double dist( const Point &p1, const Point &p2 )
    { return std::sqrt((p1.y()-p2.y())*(p1.y()-p2.y())+(p1.x()-p2.x())*(p1.x()-p2.x())); }
    //! Distance between points
    inline double dist( const PolarPoint &p1, const PolarPoint &p2 )
    { return std::sqrt((p1.y()-p2.y())*(p1.y()-p2.y())+(p1.x()-p2.x())*(p1.x()-p2.x())); }

    //! Squared distance between points
    inline double distSq( const Point &p1, const Point &p2 )
    { return ((p1.y()-p2.y())*(p1.y()-p2.y())+(p1.x()-p2.x())*(p1.x()-p2.x())); }
    //! Squared distance between points
    inline double distSq( const PolarPoint &p1, const PolarPoint &p2 )
    { return ((p1.y()-p2.y())*(p1.y()-p2.y())+(p1.x()-p2.x())*(p1.x()-p2.x())); }

    //! equality
    inline bool operator==( const Point &p1, const Point &p2 )
    { return (p1.x()==p2.x()) && (p1.y()==p2.y()); }
    //! equality
    inline bool operator!=( const Point &p1, const Point &p2 )
    { return !(p1==p2); }

    //! Rotate points about the origin
    // in place
    inline void rotate( Point &p, double theta )
    {
        const double ct = std::cos(theta);
        const double st = std::sin(theta);
        double xNew = p.x()*ct - p.y()*st;
        double yNew = p.x()*st + p.y()*ct;
        p.x()=xNew;
        p.y()=yNew;
    }
    //! Rotate points about the origin
    // not in place
    inline void rotate( const Point &p, double theta, Point &q )
    {
        const double ct = std::cos(theta);
        const double st = std::sin(theta);
        q.x() = p.x()*ct - p.y()*st;
        q.y() = p.x()*st + p.y()*ct;
    }

    //! add a transformation (x,y,theta) to a point
    //! (rotate then translate)
    // in place
    inline void addTransform( Point &p,
                              double x, double y, double theta )
    {
        rotate( p, theta );
        p.x() += x;
        p.y() += y;
    }
    //! add a transformation (x,y,theta) to a point
    //! (rotate then translate)
    // not in place
    inline void addTransform( const Point &p,
                              double x, double y, double theta,
                              Point &tp )
    {
        rotate( p, theta, tp );
        tp.x() += x;
        tp.y() += y;
    }

    //! subtract a transformation (x,y,theta) from a point
    //! (translate then rotate)
    // in place
    inline void subtractTransform( Point &p,
                                   double x, double y, double theta )
    {
        p.x() -= x;
        p.y() -= y;
        rotate( p, -theta );
    }
    //! subtract a transformation (x,y,theta) from a point
    //! (translate then rotate)
    // not in place
    inline void subtractTransform( const Point &p,
                                   double x, double y, double theta,
                                   Point &tp )
    {
        tp.x() = p.x()-x;
        tp.y() = p.y()-y;
        rotate( tp, -theta );
    }

    inline std::ostream &operator<<( std::ostream &s, const Point &p )
    { return s << p.toString(); }

    inline std::ostream &operator<<( std::ostream &s, const PolarPoint &p )
    { return s << p.toString(); }

    //! Find the intersection points of two circles.
    //! Returns false if they don't meet.
    //! Returns true if they do (and possibly a repeated intersection point if they meet at one point).
    //! 
    //! Circle equation for centre=(a,b), radius=r is:  (x-a)^2 + (y-b)^2 = r^2
    //!
    //! Throws exceptions (std::string) for problems such as
    //! negative radius or co-incident circles.
    //!
    SOEXPORT bool circleCircleIntersection( double a1,
                                            double b1,
                                            double r1,
                                            double a2,
                                            double b2,
                                            double r2,
                                            double &px,
                                            double &py,
                                            double &qx,
                                            double &qy );

    //! Convenience function. Circle centres are c1 and c2.
    inline bool circleCircleIntersection( const Point &c1,
                                          double       r1,
                                          const Point &c2,
                                          double       r2,
                                          Point  &p,
                                          Point  &q )
    {
        return circleCircleIntersection( c1.x(),c1.y(),r1, c2.x(),c2.y(),r2, p.x(),p.y(), q.x(),q.y() );
    }

    //!
    //! A line
    //!
    class SOEXPORT Line {
    public:

        //! Line defined by two points
        Line( const Point &p1, const Point &p2 );

        //! Line defined by a point and a vector
        Line( const Point &p, double vx, double vy );

        void getUnitVectorParallel( double v[2] ) const
            { v[0] = -b_; v[1] = a_; }
        void getUnitVectorPerpendicular( double v[2] ) const
            { v[0] = a_; v[1] = b_; }

       //! general equation form: ax + by + c = 0
        double a() const { return a_; }
        //! general equation form: ax + by + c = 0
        double b() const { return b_; }
        //! general equation form: ax + by + c = 0
        double c() const { return c_; }

    private:

        // Represent in general equation form: ax + by + c = 0
        // (normalised such that hypot(a,b) = 1)
        double a_, b_, c_;
    };
    std::string toString( const Line &l );
    inline std::ostream &operator<<( std::ostream &s, const Line &l )
    { return s << toString(l); }

    //! perpendicular distance from a point to an (infinite) line
    SOEXPORT double dist( const Line &l, const Point &p );

    //! Dot-product of two 2D vectors
    inline double dotProduct( double v1[2], double v2[2] )
    { return v1[0]*v2[0] + v1[1]*v2[1]; }

    //! Returns the point on the line that's closest to the origin.
    inline Point closestPointToOrigin( const Line &l )
    { return Point( -l.a()*l.c(), -l.b()*l.c() ); }

    //! Returns the closest point to 'point' that lies on 'line'.
    SOEXPORT Point closestPointOnLine( const Line &line, const Point &point );

    //! Intersection between two lines.
    //! Returns false if the lines don't intersect.
    SOEXPORT bool lineLineIntersection( const Line &l1,
                                        const Line &l2,
                                        Point &p );
    
    //! Polar coords: (rho,alpha) is the length and angle of a
    //! perpendicular bisector from the origin.
    SOEXPORT void convertToRhoAlpha( const Line &line, double &rho, double &alpha );

    //! Polar coords: (rho,alpha) is the length and angle of a
    //! perpendicular bisector from the origin.
    SOEXPORT double rho( const Line &line );

    //! Polar coords: (rho,alpha) is the length and angle of a
    //! perpendicular bisector from the origin.
    SOEXPORT double alpha( const Line &line );

}

#endif
