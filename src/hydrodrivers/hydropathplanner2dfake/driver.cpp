#include "driver.h"
#include <iostream>
#include <sstream>

using namespace std;

namespace pathplanner2dfake {

Driver::Driver( const hydrointerfaces::PathPlanner2d::Config &config,
                const hydroutil::Context                     &context )
{
    // read driver-specific properties
    std::string prefix = "Fake.";

    numInsertedPoints_ = context.properties().getPropertyAsIntWithDefault( prefix+"InsertPoints", 0 );
    assert( numInsertedPoints_>=0 && "Number of inserted points must be non-negative" );
}

std::vector<hydrointerfaces::PathPlanner2d::Point>
Driver::computePath( const Point &start,
                     const Point &goal )
{
    std::vector<hydrointerfaces::PathPlanner2d::Point> path;
    path.push_back( start );
    
    for ( int i=0; i<numInsertedPoints_; ++i )
    {
        Point middle;
        middle.x = start.x + (goal.x-start.x) * (i+1)/(numInsertedPoints_+1);
        middle.y = start.y + (goal.y-start.y) * (i+1)/(numInsertedPoints_+1);

        path.push_back( middle );
    }
    
    path.push_back( goal );

    return path;
}

} // namespace

extern "C" {
    hydrointerfaces::PathPlanner2dFactory *createPathPlanner2dDriverFactory()
    { return new pathplanner2dfake::Factory; }
}
