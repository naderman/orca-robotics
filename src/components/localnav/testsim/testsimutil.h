#ifndef TESTSIM_H
#define TESTSIM_H

#include <testsim/simulator.h>
#include <pathfollower2dI.h>
#include <hydroogmap/hydroogmap.h>

namespace localnav {

    orca::PathFollower2dData getTestPath( const hydroogmap::OgMap &ogMap,
                                          int numWaypoints,
                                          bool stressTiming,
                                          bool turnOnSpot,
                                          bool goThruDoor );

    hydroogmap::OgMap setupMap( double worldSize, 
                                double cellSize,
                                int    numObstacles,
                                bool   useRoom );

    void checkProgress( const orca::PathFollower2dData     &path, 
                        const hydrosim2d::VehicleSimulator &vehicleSim,
                        int                                 iterationNum,
                        int                                &wpI,
                        bool                               &pathCompleted,
                        bool                               &pathFailed );
                        
}

#endif
