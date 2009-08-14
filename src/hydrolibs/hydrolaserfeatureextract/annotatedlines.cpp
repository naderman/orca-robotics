#include "annotatedlines.h"
#include <iostream>
#include <sstream>
#include <gbxutilacfr/mathdefs.h>
#include <hydroutil/hydroutil.h>
#include <gbxutilacfr/exceptions.h>

using namespace std;

namespace hydrolfextract {

std::string
toString( EndpointVisibility x )
{
    switch ( x )
    {
    case NotVisible:                  return "NotVisible";
    case VisibleInternalLineJunction: return "VisibleInternalLineJunction";
    case VisibleExternalLineJunction: return "VisibleExternalLineJunction";
    case VisibleFreeStandingLine:     return "VisibleFreeStandingLine";
    }
    stringstream ss;
    ss << "Unknown EndpointVisibility: "<< ((int)x);
    throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
}

std::string
toString( const AnnotatedCluster &a )
{
    stringstream ss;
    ss << a.cluster() << " (also notes)";
    return ss.str();
}

std::string
toString( const AnnotatedLines &a )
{
    stringstream ss;
    ss << "\n";
    for ( size_t i=0; i < a.numClusters(); i++ )
    {
        ss << "  Cluster " << i << ":" << endl;
        ss << a.clusters()[i] << endl;
    }
    return ss.str();
}

AnnotatedLines::AnnotatedLines( const Lines &lines )
{
    for ( size_t i=0; i < lines.numClusters(); i++ )
    {
        annotatedClusters_.push_back( AnnotatedCluster( lines.clusters()[i] ) );
    }
}

AnnotatedCluster::AnnotatedCluster( const Cluster &cluster )
    : cluster_(cluster),
      notes_(cluster.numLineSegments())
{
}

////////////////////////////////////////////////////////////////////////////////

namespace {

double 
calcRangeBeyondExtremity( const Cluster &cluster,
                          int lineI,
                          StartOrEnd startOrEnd,
                          const std::vector<float> &ranges )
{
    const int indexOfEndpoint = cluster.lineExtremity(lineI,startOrEnd).returnI();
    if ( startOrEnd == Start )
    {
        const int i = indexOfEndpoint - 1;
        assert( i >= 0 );
        return ranges[i];
    }
    else
    {
        const int i = indexOfEndpoint + 1;
        assert( i < (int)(ranges.size()) );
        return ranges[i];            
    }
}

//
// 'adjacentLineI==-1' means 'no adjacent line'.
//
EndpointVisibility
endpointVisibility( const Cluster            &cluster,
                    int                       lineI,
                    int                       adjacentLineI,
                    StartOrEnd                startOrEnd,
                    const std::vector<float> &ranges,
                    double                    minLineLengthForVisibleEndpoints )
{
//     cout<<"TRACE(lineextractor.cpp): --------- "<<__func__<<" ---------" << endl;

    if ( cluster.lineLength(lineI) < minLineLengthForVisibleEndpoints )
        return NotVisible;
    
    // Check for lineSegment at edge of scan
    const int indexOfEndpoint = cluster.lineExtremity(lineI,startOrEnd).returnI();
    if ( (startOrEnd == Start && indexOfEndpoint == 0) ||
         (startOrEnd == End   && indexOfEndpoint == (int)(ranges.size())-1) )
    {
        return NotVisible;
    }

    const double alphaThis = geom2d::alpha( cluster.lines()[lineI].bestFitLine() );

    // Check for junction of two lines
    const double MIN_LINE_LENGTH_ADJACENT_LINE = minLineLengthForVisibleEndpoints/3.0;
    if ( adjacentLineI != -1 &&
         cluster.lineLength(adjacentLineI) > MIN_LINE_LENGTH_ADJACENT_LINE )
    {
        double alphaAdjacent = geom2d::alpha( cluster.lines()[adjacentLineI].bestFitLine() );
        double meetAngle = alphaThis-alphaAdjacent;
        NORMALISE_ANGLE(meetAngle);
        
        const double MEET_ANGLE_THRESHOLD = 45.0*M_PI/180.0;
        if ( meetAngle > MEET_ANGLE_THRESHOLD )
        {
            if ( startOrEnd==Start ) return VisibleInternalLineJunction;
            else                     return VisibleExternalLineJunction;
        }
        else if ( meetAngle < -MEET_ANGLE_THRESHOLD )
        {
            if ( startOrEnd==Start ) return VisibleExternalLineJunction;
            else                     return VisibleInternalLineJunction;
        }
    }

    // Is it a free-standing line that we can see the end of?
    // Find the angle (external to the endpoint) 
    // between the infinite line and the endpoint
    double angleToExtremity;
    if ( startOrEnd == Start )
        angleToExtremity = M_PI/2.0 - (cluster.firstPointInLine(lineI).bearing() - alphaThis);
    else
        angleToExtremity = M_PI/2.0 + (cluster.lastPointInLine(lineI).bearing() - alphaThis);

    const double ANGLE_THRESHOLD = 60*M_PI/180.0;
    if ( angleToExtremity > ANGLE_THRESHOLD )
    {
        // cout<<"TRACE(lineextractor.cpp): can't see around corner" << endl;
        return NotVisible;
    }

    double rangeToExtremity = cluster.lineExtremity(lineI,startOrEnd).range();
    double rangeBeyondExtremity = calcRangeBeyondExtremity( cluster, lineI, startOrEnd, ranges );
    const double MIN_DELTA_RANGE_FOR_LINE_ENDPOINT = 0.2;
    if ( rangeBeyondExtremity < ( rangeToExtremity + MIN_DELTA_RANGE_FOR_LINE_ENDPOINT ) )
    {
        // cout<<"TRACE(lineextractor.cpp): obscured by fg obstacle" << endl;
        return NotVisible;
    }

    return VisibleFreeStandingLine;
}

double calcMaxLineFitError( const Cluster &c,
                            int lineI )
{
    double maxErr=-1;
    for ( int i=0; i < c.lines()[lineI].numPoints(); i++ )
    {
        double err = geom2d::dist( c.lines()[lineI].bestFitLine(), c.nthPointInLine( lineI, i ).point() );
        if ( err > maxErr )
        {
            maxErr = err;
        }
    }
    assert( maxErr >= 0 );
    return maxErr;
}

void
calcLineFitUncertainty( const AnnotatedCluster &annCluster,
                        int lineI,
                        double &lineFitRhoSd,
                        double &lineFitAlphaSd )
{
    // Errors in line fitting can:
    // (a) push the line back and forth,
    lineFitRhoSd = annCluster.maxLineFitError(lineI);
    // (b) rotate the line.  This effect is reduced for longer lines
    //     Assume the worst: the line could be off by maxLineFitError on the ends.
    lineFitAlphaSd = 2 * atan( annCluster.maxLineFitError(lineI) / annCluster.lineLength(lineI) );
}

}

////////////////////////////////////////////////////////////////////////////////

AnnotatedLines
getAnnotatedLines( const Lines              &lines,
                   const std::vector<float> &ranges,
                   double                    minLineLengthForVisibleEndpoints )
{
    AnnotatedLines annLines( lines );

    for ( size_t clusterI=0; clusterI < annLines.numClusters(); clusterI++ )
    {
        AnnotatedCluster &annCluster = annLines.clusters()[clusterI];

        for ( size_t lineI=0; lineI < annCluster.numLineSegments(); lineI++ )
        {
            int prevLineI=-1;
            int nextLineI=-1;

            // Check if adjacent lines are connected
            if ( lineI != 0 )
            {
                if ( annCluster.lastPointInLine(lineI-1).returnI() ==
                     annCluster.firstPointInLine(lineI).returnI() )
                {
                    prevLineI = lineI-1;
                }
            }
            if ( lineI != annCluster.numLineSegments()-1 )
            {
                if ( annCluster.firstPointInLine(lineI+1).returnI() ==
                     annCluster.lastPointInLine(lineI).returnI() )
                {
                    nextLineI = lineI+1;
                }
            }

            annCluster.setStartVisibility( lineI, endpointVisibility( annCluster.cluster(),
                                                                      lineI,
                                                                      prevLineI,
                                                                      Start,
                                                                      ranges,
                                                                      minLineLengthForVisibleEndpoints ) );
            annCluster.setEndVisibility( lineI, endpointVisibility( annCluster.cluster(),
                                                                    lineI,
                                                                    nextLineI,
                                                                    End,
                                                                    ranges,
                                                                    minLineLengthForVisibleEndpoints ) );

            annCluster.setMaxLineFitError( lineI, calcMaxLineFitError( annCluster.cluster(), lineI ) );
            double rhoSd, alphaSd;
            calcLineFitUncertainty( annCluster.cluster(), lineI, rhoSd, alphaSd );
            annCluster.setLineFitUncertainty( lineI, rhoSd, alphaSd );
        }
    }
    return annLines;
}

}

