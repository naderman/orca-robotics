#ifndef GEOM2D_H
#define GEOM2D_H

#include <string>
#include <cmath>
#include <iostream>

namespace geom2d {

    class Point {
    public:

        explicit Point() {}
        explicit Point( double x, double y )
            : x_(x),y_(y) {}

        double x() const { return x_; }
        double y() const { return y_; }

        double &x() { return x_; }
        double &y() { return y_; }

        // convert to polar
#ifdef __linux
        double range() const { return hypotf(y_,x_); }
#else
        double range() const { return std::hypotf(y_,x_); }
#endif
        double bearing() const { return std::atan2(y_,x_); }

        std::string toString()   const;
        std::string toStringXY() const;
        std::string toStringRB() const;

    private:
        double x_, y_;
    };

    class PolarPoint {
    public:
        
        explicit PolarPoint() {}
        explicit PolarPoint( double r, double b )
            : r_(r),b_(b) {}

        double range()   const { return r_; }
        double bearing() const { return b_; }

        double &range()   { return r_; }
        double &bearing() { return b_; }

        // convert to cartesian
        double x() const { return r_*std::cos(b_); }
        double y() const { return r_*std::sin(b_); }

        std::string toString()   const;
        std::string toStringXY() const;
        std::string toStringRB() const;

    private:
        double r_, b_;
    };

    // Distance between points
#ifdef __linux
    inline double dist( const Point &p1, const Point &p2 )
    { return hypotf(p1.y()-p2.y(),p1.x()-p2.x()); }
    inline double dist( const PolarPoint &p1, const PolarPoint &p2 )
    { return hypotf(p1.y()-p2.y(),p1.x()-p2.x()); }
#else
    inline double dist( const Point &p1, const Point &p2 )
    { return std::hypotf(p1.y()-p2.y(),p1.x()-p2.x()); }
    inline double dist( const PolarPoint &p1, const PolarPoint &p2 )
    { return std::hypotf(p1.y()-p2.y(),p1.x()-p2.x()); }
#endif

    // Rotate points about the origin
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
    // not in place
    inline void rotate( const Point &p, double theta, Point &q )
    {
        const double ct = std::cos(theta);
        const double st = std::sin(theta);
        q.x() = p.x()*ct - p.y()*st;
        q.y() = p.x()*st + p.y()*ct;
    }

    inline std::ostream &operator<<( std::ostream &s, const Point &p )
    { return s << p.toString(); }

    inline std::ostream &operator<<( std::ostream &s, const PolarPoint &p )
    { return s << p.toString(); }

    // Find the intersection points of two circles.
    // Returns false if they don't meet.
    // Returns true if they do (and possibly a repeated intersection point if they meet at one point).
    // 
    // Circle equation for centre=(a,b), radius=r is:  (x-a)^2 + (y-b)^2 = r^2
    //
    // Throws exceptions (std::string) for problems such as
    // negative radius or co-incident circles.
    //
    bool circleCircleIntersection( double a1,
                                   double b1,
                                   double r1,
                                   double a2,
                                   double b2,
                                   double r2,
                                   double &px,
                                   double &py,
                                   double &qx,
                                   double &qy );

    //
    // Convenience function. Circle centres are c1 and c2.
    //
    inline bool circleCircleIntersection( const Point &c1,
                                          double       r1,
                                          const Point &c2,
                                          double       r2,
                                          Point  &p,
                                          Point  &q )
    {
        return circleCircleIntersection( c1.x(),c1.y(),r1, c2.x(),c2.y(),r2, p.x(),p.y(), q.x(),q.y() );
    }
}

#endif
