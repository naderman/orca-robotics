#include <iostream>
#include <driverthread/driverthread.h>
#include <gbxutilacfr/trivialtracer.h>
#include <hydroiceutil/localstatus.h>

using namespace std;
using namespace segwayrmpdriverthread;

int main( int argc, char **argv )
{
    DriverThread::Config config;
    config.driveInReverse = false;
    config.isMotionEnabled = false;
    config.maxForwardAcceleration = 1.0;
    config.maxReverseAcceleration = 1.0;

    int debug=0, info=9, warn=9, error=9;
    gbxutilacfr::TrivialTracer tracer( debug, info, warn, error );
    hydroutil::Properties props;
    hydroiceutil::LocalStatus status(tracer,props);

//     DriverThread driverThread( config, 
//                                segwayRmp,
//                                tracer,
//                                status, 
//                                callback );

    return 0;
}
