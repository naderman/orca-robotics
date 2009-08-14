/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include "lines.h"
#include <iostream>
#include <assert.h>
#include <sstream>

using namespace std;

namespace hydrolfextract {

namespace {

    // Returns: true if the points should be broken,
    //          false if they're all in a line
    bool findBreakPoint( const std::vector<Point> &points,
                         int                       firstPointI,
                         int                       lastPointI,
                         double                    breakDistThreshold,
                         int                      &breakPoint )
    {
        assert( lastPointI - firstPointI > 0 );

        // Fit a line to the points, _using_the_endpoints_.
        // (a best-fit can be bad: for an L-shaped cluster, we'd like
        //  to break on the corner of the L, not the end).
        geom2d::Line line( points[firstPointI].point(), points[lastPointI].point() );

        double maxDist = -1;
        breakPoint = -1;
        for ( int i=firstPointI; i <= lastPointI; i++ )
        {
            double perpDistToLine = geom2d::dist( line, points[i].point() );
            if ( perpDistToLine > maxDist )
            {
                maxDist = perpDistToLine;
                breakPoint = i;
            }
        }

        if ( maxDist < 0 )
        {
            cout<<"TRACE(linesegments.cpp): maxDist: " << maxDist << endl;
            cout<<"TRACE(linesegments.cpp): line: " << line << endl;
        }
        assert( maxDist >= 0 );
        return ( maxDist > breakDistThreshold );
    }

    // Only operates on the upper triangle of cov.
    void eig2d( const double cov[2][2],
                double eigVect[2][2],
                double eigVal[2] )
    {
        const double trace = cov[0][0] + cov[1][1];
        const double det = cov[0][0]*cov[1][1] - cov[0][1]*cov[0][1];

        const double traceBit = trace/2.0;
        const double sqrtBit = sqrt( trace*trace/4.0 - det );

        eigVal[0] = traceBit + sqrtBit;
        eigVal[1] = traceBit - sqrtBit;

        if ( cov[0][1] == 0 )
        {
            eigVect[0][0] = 1.0;
            eigVect[0][1] = 0.0;

            eigVect[1][0] = 0.0;
            eigVect[1][1] = 1.0;
        }
        else
        {
            eigVect[0][0] = eigVal[0]-cov[1][1];
            eigVect[0][1] = cov[0][1];

            eigVect[1][0] = eigVal[1]-cov[1][1];
            eigVect[1][1] = cov[0][1];
        }
    }


    geom2d::Line
    computeBestFitLine( const std::vector<Point> &points,
                        int firstPointI,
                        int lastPointI )
    {
        //
        // Find the mean (ie centroid)
        //
        int numPoints = lastPointI-firstPointI+1;
        assert( numPoints > 1 );
        double mean[2];
        mean[0] = 0.0; mean[1] = 0.0;
        for ( int pointI=firstPointI; pointI <= lastPointI; pointI++ )
        {
            for ( int i=0; i < 2; i++ )
                mean[i] += points[pointI].point()[i];
        }
        mean[0] /= (double)numPoints;
        mean[1] /= (double)numPoints;

        //
        // Find the covariance (upper triangle only)
        //
        double cov[2][2];
        for ( size_t i=0; i < 2; i++ )
            for ( size_t j=i; j < 2; j++ )
                cov[i][j] = 0.0;

        for ( int pointI=firstPointI; pointI <= lastPointI; pointI++ )
        {
            for ( size_t i=0; i < 2; i++ )
            {
                const double diffI = points[pointI].point()[i]-mean[i];
                for ( size_t j=i; j < 2; j++ )
                {
                    const double diffJ = points[pointI].point()[j]-mean[j];
                    cov[i][j] += diffI*diffJ;
                }
            }
        }
        cov[0][0] /= (double)numPoints;
        cov[0][1] /= (double)numPoints;
        cov[1][1] /= (double)numPoints;

        //
        // Find the eigenvectors/eigenvalues
        //
        double eigVect[2][2];
        double eigVal[2];
        eig2d( cov, eigVect, eigVal );

        // The covariance gives an ellipse over the spread of points.
        // The 'line' is the major axis of this ellipse.
        int largestEigI = 0;
        if ( eigVal[1] > eigVal[0] )
            largestEigI = 1;

        return geom2d::Line( geom2d::Point( mean[0], mean[1] ),
                             eigVect[largestEigI][0],
                             eigVect[largestEigI][1] );
    }

    std::vector<Point> createPoints( const std::vector<float>           &ranges,
                                     const hydroscanutil::ScannerConfig &scannerConfig,
                                     int                                 firstRangeI,
                                     int                                 lastRangeI )
    {
        std::vector<Point> points;
        for ( int i=firstRangeI; i <= lastRangeI; i++ )
        {
            const double range   = ranges[i];
            const double bearing = scanBearing( scannerConfig, i );
            const double x = range*cos(bearing);
            const double y = range*sin(bearing);

            points.push_back( Point( x, y, i ) );

            assert( isValidRange( range, scannerConfig ) );
        }
        return points;
    }

    void fitLineSegments( const std::vector<Point> &points,
                          int                       firstPointI,
                          int                       lastPointI,
                          int                       minPointsInLine,
                          double                    breakDistThreshold,
                          std::vector<LineSegment> &lineSegments )
    {
        assert( lastPointI > firstPointI );
        assert( lastPointI-firstPointI+1 >= minPointsInLine );

        int breakPoint;
        bool shouldBreak = findBreakPoint( points,
                                           firstPointI,
                                           lastPointI,
                                           breakDistThreshold,
                                           breakPoint );

        if ( !shouldBreak )
        {
            // The whole thing is a valid line
            lineSegments.push_back( LineSegment( points, firstPointI, lastPointI ) );
        }
        else
        {
            // Split.  Note that breakPoint ends up on both sides.
            if ( breakPoint-firstPointI+1 >= minPointsInLine )
                fitLineSegments( points, firstPointI, breakPoint, minPointsInLine, breakDistThreshold, lineSegments );
            if ( lastPointI-breakPoint+1 >= minPointsInLine )
                fitLineSegments( points, breakPoint, lastPointI, minPointsInLine, breakDistThreshold, lineSegments );
        }
    }

    Cluster createCluster( const std::vector<float>           &ranges,
                           const hydroscanutil::ScannerConfig &scannerConfig,
                           int                                 firstRangeI,
                           int                                 lastRangeI,
                           int                                 minPointsInLine,
                           double                              maxDeltaInLine )
    {
        // Create the points
        std::vector<Point> points = createPoints( ranges, scannerConfig, firstRangeI, lastRangeI );
        
        // Fit lines
        std::vector<LineSegment> lineSegments;
        fitLineSegments( points, 0, points.size()-1, minPointsInLine, maxDeltaInLine, lineSegments );

//        return Cluster( points, lineSegments );


        Cluster cluster( points, lineSegments );
        return cluster;
    }
}

////////////////////////////////////////////////////////////////////////////////


std::string toString( const Cluster &c )
{
    stringstream ss;
    for ( size_t i=0; i < c.numLineSegments(); i++ )
    {
        ss << "    start="<<c.firstPointInLine(i).point()<<", end="<<c.lastPointInLine(i).point();
        if ( i != c.numLineSegments()-1 ) ss << endl;
    }
    return ss.str();
}

LineSegment::LineSegment( const std::vector<Point> &points,
                          int                       firstPointI,
                          int                       lastPointI )
    : firstPointI_(firstPointI),
      lastPointI_(lastPointI),
      bestFitLine_(computeBestFitLine(points,firstPointI,lastPointI))
{    
}

double
Cluster::lineLength( int lineI ) const
{
    const double len = hypot( lastPointInLine(lineI).y() - firstPointInLine(lineI).y(),
                              lastPointInLine(lineI).x() - firstPointInLine(lineI).x() );
    return len;
}

std::string toString( const LineSegment &l )
{
    stringstream ss;
    ss << "{start=" << l.firstPointI() << ", end=" << l.lastPointI() << "}";
    return ss.str();
}

std::string toString( const Lines &lines )
{
    stringstream ss;
    ss << "\n";
    for ( size_t i=0; i < lines.numClusters(); i++ )
    {
        ss << "  Cluster " << i << ":" << endl;
        ss << lines.clusters()[i] << endl;
    }
    return ss.str();
}

////////////////////////////////////////////////////////////////////////////////

Lines
extractLines( const std::vector<float>           &ranges,
              const hydroscanutil::ScannerConfig &scannerConfig,
              int                                 minPointsInLine,
              double                              maxDeltaRangeInCluster,
              double                              maxDeltaInLine )
{
    Lines lines;

    int clusterStart = 0;
    for ( size_t i=1; i < ranges.size(); i++ )
    {
        double deltaRange = ranges[i]-ranges[i-1];
        if ( !isValidRange( ranges[i], scannerConfig ) ||
             !isValidRange( ranges[i-1], scannerConfig ) ||
             fabs(deltaRange) > maxDeltaRangeInCluster )
        {
            // Found a break in the scan.  Create a cluster behind us?
            if ( (int)i-clusterStart >= minPointsInLine )
            {
                Cluster cluster = createCluster( ranges,
                                                 scannerConfig,
                                                 clusterStart,
                                                 i-1,
                                                 minPointsInLine,
                                                 maxDeltaInLine );
                if ( cluster.numLineSegments() > 0 )
                    lines.addCluster( cluster );
            }
            clusterStart = i;
        }
    }

    // Finish off the last cluster
    if ( (int)(ranges.size())-clusterStart >= minPointsInLine )
    {
        Cluster cluster = createCluster( ranges,
                                         scannerConfig,
                                         clusterStart,
                                         ranges.size()-1,
                                         minPointsInLine,
                                         maxDeltaInLine );
        if ( cluster.numLineSegments() > 0 )
            lines.addCluster( cluster );
    }

    return lines;
}



}

