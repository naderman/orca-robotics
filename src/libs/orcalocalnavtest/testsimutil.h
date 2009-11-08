#ifndef ORCALOCALNAVTEST_TESTSIM_H
#define ORCALOCALNAVTEST_TESTSIM_H

#include <orcalocalnavtest/simulator.h>
#include <orca/pathfollower2d.h>
#include <hydroogmap/hydroogmap.h>

namespace orcalocalnavtest {

    orca::PathFollower2dData getTestPath( const hydroogmap::OgMap &ogMap,
                                          int numWaypoints,
                                          bool stressTiming,
                                          bool regularWaypointGrid,
                                          bool turnOnSpot,
                                          bool goThruDoor );

    hydroogmap::OgMap setupMap( double worldSize, 
                                double cellSize,
                                int    numObstacles,
                                bool   useRoom );

    void checkProgress( const orca::PathFollower2dData     &path, 
                        const hydrosim2d::VehicleSimulator &vehicleSim,
                        int                                 iterationNum,
                        int                                 iterationLimit,
                        int                                &wpI,
                        bool                               &pathCompleted,
                        bool                               &pathFailed );
                        
}

#endif
