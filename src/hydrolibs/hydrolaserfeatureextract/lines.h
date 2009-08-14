/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef HYDROLFEXTRACT_LINESEGMENTS_H
#define HYDROLFEXTRACT_LINESEGMENTS_H

#include <vector>
#include <hydrogeom2d/geom2d.h>
#include <hydroscanutil/scanutil.h>

namespace hydrolfextract {

//
// A Point is a single return from the scan.
//
class Point {
public:
    Point( double x, double y, int returnI )
        : p_(x,y),
          returnI_(returnI)
        {}

    double x() const { return p_.x(); }
    double y() const { return p_.y(); }

    double &x() { return p_.x(); }
    double &y() { return p_.x(); }

    const geom2d::Point &point() const { return p_; }

    // Convert to polar
    double range()   const { return p_.range(); }
    double bearing() const { return p_.bearing(); }

    int returnI() const { return returnI_; }

private:

    geom2d::Point p_;

    // The index of the return this point represents
    int returnI_;
};

enum StartOrEnd {
    Start,
    End,
};

// A line segment is a set of points such that no point is more than a certain distance
// from the line.
//
class LineSegment {
public:

    LineSegment( const std::vector<Point> &points,
                 int                       firstPointI,
                 int                       lastPointI );

    int firstPointI() const { return firstPointI_; }
    int lastPointI() const { return lastPointI_; }

    const geom2d::Line &bestFitLine() const { return bestFitLine_; }

    int numPoints() const { return lastPointI_-firstPointI_+1; }

private:

    int          firstPointI_;
    int          lastPointI_;
    geom2d::Line bestFitLine_;
};
std::string toString( const LineSegment &lineSegment );
inline std::ostream &operator<<( std::ostream &s, const LineSegment &l )
{ return s << toString(l); }

// A cluster is a set of points such that adjacent points are no more than 
// a certain distance apart.
// It is further sub-divided into line segments.
class Cluster {
public:

    // Create a cluster from firstRangeI to lastRangeI inclusive.
    Cluster( const std::vector<Point>       &points,
             const std::vector<LineSegment> &lineSegments )
        : points_(points),
          lines_(lineSegments)
        {}

    const std::vector<Point> &points() const { return points_; }
    const std::vector<LineSegment> &lines() const { return lines_; }
    unsigned int numLineSegments() const { return lines_.size(); }

    const Point &firstPointInLine( int lineI ) const { return points_[lines_[lineI].firstPointI()]; }
    const Point &lastPointInLine( int lineI ) const { return points_[lines_[lineI].lastPointI()]; }
    const Point &nthPointInLine( int lineI, int n ) const { return points_[lines_[lineI].firstPointI()+n]; }

    const Point &lineExtremity( int lineI, StartOrEnd startOrEnd ) const
        {
            if ( startOrEnd == Start ) return firstPointInLine(lineI);
            else return lastPointInLine(lineI);
        }    

    double lineLength( int lineI ) const;

private:

    std::vector<Point>       points_;
    std::vector<LineSegment> lines_;
};
std::string toString( const Cluster &c );
inline std::ostream &operator<<( std::ostream &s, const Cluster &c )
{ return s << toString(c); }

class Lines {
public:

    unsigned int numClusters() const { return clusters_.size(); }

    void addCluster( const Cluster &cluster ) { clusters_.push_back( cluster ); }
    const std::vector<Cluster> &clusters() const { return clusters_; }

private:
    std::vector<Cluster> clusters_;
};
std::string toString( const Lines &lines );
inline std::ostream &operator<<( std::ostream &s, const Lines &lines )
{ return s << toString(lines); }

Lines
extractLines( const std::vector<float>           &ranges,
              const hydroscanutil::ScannerConfig &scannerConfig,
              int                                 minPointsInLine,
              double                              maxDeltaRangeInCluster,
              double                              maxDeltaInLine );

}

#endif
