#ifndef TESTSIM_H
#define TESTSIM_H

#include <testsim/simulator.h>
#include <pathfollower2dI.h>

namespace localnav {

    void getTestPath( orca::PathFollower2dData &d, int numWaypoints );

    void checkProgress( const orca::PathFollower2dData     &path,
                        const hydrosim2d::VehicleSimulator &vehicleSim,
                        int                                 iterationNum );
                        
}

#endif
