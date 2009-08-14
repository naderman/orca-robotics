/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <cmath>
#include <iostream>
#include <assert.h>
#include <sstream>
#include "polefinder.h"

using namespace std;

namespace hydrolfextract {

namespace {

    double calcPoleWidth( const std::vector<float> &ranges,
                          int startI,
                          int endI,
                          const hydroscanutil::ScannerConfig &scannerCfg )
    {
        const double range    = (ranges[startI]+ranges[endI]) / 2.0;
        const double dBearing = scannerCfg.angleIncrement * (endI-startI+1);
        const double width = range*sin(dBearing);
        return width;
    }

    std::string rangeInfoStr( int i,
                              const std::vector<float> &ranges,
                              const hydroscanutil::ScannerConfig &scannerCfg )
    {
        stringstream ss;
        ss << "i="<<i<<", r="<<ranges[i]<<", b="<<scanBearing(scannerCfg,i)*180.0/M_PI<<"deg";
        return ss.str();
    }

}

std::vector<Pole>
detectPoles( const std::vector<float> &ranges,
             const hydroscanutil::ScannerConfig &scannerCfg,
             double minWidth,
             double maxWidth,
             double minDistanceToBackground,
             bool debug )
{
    std::vector<Pole> poles;

    bool startedCluster = false;
    int clusterStart    = -1;

    for ( int i = 1; i < (int)ranges.size(); i++ )
    {
        if ( startedCluster )
        {
            if ( ranges[i]-ranges[i-1] > minDistanceToBackground )
            {
                if ( debug )
                    cout<<__func__<<": stepped back by " << ranges[i]-ranges[i-1] 
                        << " at " << rangeInfoStr(i,ranges,scannerCfg) << endl;

                // Range steps back: end of cluster.
                // Have we just passed over a pole?
                double poleWidth = calcPoleWidth(ranges, clusterStart, i-1, scannerCfg);
                if ( poleWidth >= minWidth &&
                     poleWidth <= maxWidth )
                {
                    // We have a pole!
                    const double range   = (ranges[clusterStart]+ranges[i-1]) / 2.0;
                    const double bearing = (scanBearing(scannerCfg,clusterStart)+scanBearing(scannerCfg,i-1)) / 2.0;
                    poles.push_back( Pole( clusterStart, i-1, range, bearing, poleWidth ) );
                }
                else
                {
                    if ( debug )
                        cout<<__func__<<": pole at "<<rangeInfoStr(i,ranges,scannerCfg)<<" too skinny: w="<<poleWidth<< endl;

                }
                startedCluster = false;
            }
            else if ( calcPoleWidth( ranges, clusterStart, i, scannerCfg ) > maxWidth )
            {
                // Too big.
                if ( debug )
                    cout<<__func__<<": pole too big at " << rangeInfoStr(i,ranges,scannerCfg) 
                        << ": poleWidthSoFar: " <<  calcPoleWidth( ranges, clusterStart, i, scannerCfg )
                        << " (clusterStart="<<clusterStart<<")" << endl;
                    
                startedCluster = false;
            }
            else if ( fabs( ranges[i] - ranges[clusterStart] ) > maxWidth )
            {
                // Exceeds the width in range
                if ( debug )
                    cout<<__func__<<": pole too big in range at " << rangeInfoStr(i,ranges,scannerCfg) 
                        << " (clusterStart="<<clusterStart<<")"<< endl;
                startedCluster = false;
            }
        }

        // Look for steps forward
        if ( ranges[i]-ranges[i-1] < -minDistanceToBackground )
        {
            if ( debug )
                cout<<__func__<<": started cluster at "<<rangeInfoStr(i,ranges,scannerCfg) << endl;
            startedCluster = true;
            clusterStart = i;
        }

        // Watch for invalid returns
        if ( !isValidRange( ranges[i], scannerCfg ) )
        {
            if ( debug && startedCluster )
                cout<<__func__<<": stopping cluster due to invalid range at " << rangeInfoStr(i,ranges,scannerCfg) << endl;
                
            startedCluster = false;
        }
    }
    return poles;
}

std::string toString( const Pole &p )
{
    stringstream ss;
    ss << "startI=" << p.startI << ", endI=" << p.endI << ", r=" << p.range << ", b=" << p.bearing*180.0/M_PI << "deg, w=" << p.width;
    return ss.str();
}

}
