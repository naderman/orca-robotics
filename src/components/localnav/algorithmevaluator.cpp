#include "algorithmevaluator.h"
#include <iostream>

using namespace std;

namespace localnav {

namespace {

    // The sigmoid function
    inline double sgm( double x )
    {
        return 1.0/(1.0+exp(-x));
    }

    double obstacleCost( double distanceToNearestObstacle,
                         double linearSpeed,
                         double speedFor1MClearance,
                         double robotRadius )
    {
        const double allowedClearance = linearSpeed / speedFor1MClearance;
        const double actualClearance = distanceToNearestObstacle-robotRadius;

        if ( actualClearance < allowedClearance )
        {
            // This is unsafe: disallow
            return 1000;
        }
        else
        {
            return 1.0-sgm( (actualClearance-allowedClearance)/allowedClearance );
        }        
    }

    double distanceToNearestObstacle( const hydroogmap::OgMap &ogMap,
                                      const hydronavutil::Pose &pose )
    {
        // Loop through the entire og map

        double minDistSq = 1e9;
        for ( int x=0; x < ogMap.numCellsX(); x++ )
        {
            for ( int y=0; y < ogMap.numCellsY(); y++ )
            {
                if ( ogMap.gridCell(x,y) > hydroogmap::CELL_OCCUPIED )
                {
                    double xm = ogMap.worldXCoord(x);
                    double ym = ogMap.worldYCoord(y);
                    double xd = pose.x()-xm;
                    double yd = pose.y()-ym;
                    double distSq = xd*xd + yd*yd;
                    if ( distSq < minDistSq )
                        minDistSq = distSq;
                }
            }
        }
        double dist = sqrt( minDistSq );

        // Include distances from borders
        
        double distL = pose.x()-ogMap.offset().p.x;
        double distR = ogMap.offset().p.x+ogMap.worldSizeX()-pose.x();
        double distB = pose.y()-ogMap.offset().p.y;
        double distT = ogMap.offset().p.y+ogMap.worldSizeY()-pose.y();
        if ( distL < dist )
            dist = distL;
        if ( distR < dist )
            dist = distR;
        if ( distB < dist )
            dist = distB;
        if ( distT < dist )
            dist = distT;
        return dist;
    }

    double obstacleCost( const Simulator &simulator )
    {
        const double speedFor1MClearance = 3.0;
        const double linearSpeed = simulator.velocity().lin();
        const double robotRadius = simulator.vehicleSimulator().config().robotRadius;
        const double distToNearestObstacle = distanceToNearestObstacle( simulator.ogMap(),
                                                                            simulator.vehicleSimulator().pose() );

        return obstacleCost( distToNearestObstacle,
                             linearSpeed,
                             speedFor1MClearance,
                             robotRadius );
    }

}

AlgorithmEvaluator::~AlgorithmEvaluator()
{
    cout<<"TRACE(algorithmevaluator.cpp): DESTRUCTOR" << endl;
    const double avgObstacleCost       = cumObstacleCost_ / (double)numIterations_;
    const double avgTimeToMakeDecision = cumDecisionTime_ / (double)numIterations_;

    cout << "  avgTimeToMakeDecision: " << avgTimeToMakeDecision*1000.0 << "ms" << endl;
    cout << "  avgObstacleCost:       " << avgObstacleCost << endl;
    cout << "  numIterations:         " << numIterations_ << endl;

    FILE *f = fopen( saveFile_.c_str(), "w" );
    assert( f != NULL );
    {
        fprintf( f, "%d %f %f\n", numIterations_, avgTimeToMakeDecision, avgObstacleCost );
    }
    fclose(f);
}

void
AlgorithmEvaluator::evaluate( double timeToMakeDecision,
                              const Simulator &simulator )
{
    numIterations_++;
    const double obsCost = obstacleCost( simulator );
    cumObstacleCost_ += obsCost;
    cumDecisionTime_ += timeToMakeDecision;

//     cout<<"TRACE(algorithmevaluator.cpp): timeToMakeDecision: " << timeToMakeDecision*1000.0 << "ms" << endl;
//     cout<<"TRACE(algorithmevaluator.cpp): obsCost: " << obsCost << endl;
}

}

