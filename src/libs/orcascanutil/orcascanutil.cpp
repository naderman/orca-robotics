#include "orcascanutil.h"
#include <iostream>
#include <orcaifaceutil/rangescanner2d.h>
#include <gbxutilacfr/exceptions.h>
#include <hydrotime/time.h>

using namespace std;

namespace orcascanutil {

    hydroscanutil::ScannerConfig 
    convertConfig( const orca::RangeScanner2dDescription &descr )
    {
        return hydroscanutil::ScannerConfig( 
            descr.maxRange,
            descr.startAngle,
            hydroscanutil::calcAngleIncrement( descr.fieldOfView,
                                               descr.numberOfSamples ),
            descr.numberOfSamples );        
    }

    hydronavutil::Pose convertOffset( const orca::RangeScanner2dDescription &descr )
    {
        const double MAX_ANGLE = 5*180.0/M_PI;
            
        if ( descr.offset.o.r > MAX_ANGLE || descr.offset.o.p > MAX_ANGLE )
        {
            stringstream ss;
            ss << "Laser offset is non-planar, 2D assumption is really bad.  Offset: " << ifaceutil::toString(descr);
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }

        return hydronavutil::Pose( descr.offset.p.x, 
                                   descr.offset.p.y,
                                   descr.offset.o.y );
    }

    orca::RangeScanner2dDescription
    convert( const hydroscanutil::ScannerConfig &scannerConfig,
             const hydronavutil::Pose           &scannerOffset,
             const hydrotime::Time              &timeStamp )
    {
        orca::RangeScanner2dDescription orcaDescr;
        orcaDescr.timeStamp.seconds  = timeStamp.seconds();
        orcaDescr.timeStamp.useconds = timeStamp.useconds();

        orcaDescr.minRange        = 0.0;
        orcaDescr.maxRange        = scannerConfig.maxRange;
        orcaDescr.fieldOfView     = scannerConfig.angleIncrement*(scannerConfig.numReturns-1);
        orcaDescr.startAngle      = scannerConfig.startAngle;
        orcaDescr.numberOfSamples = scannerConfig.numReturns;

        orcaDescr.offset.p.x = scannerOffset.x();
        orcaDescr.offset.p.y = scannerOffset.y();
        orcaDescr.offset.p.z = 0.0;
        orcaDescr.offset.o.r = 0.0;
        orcaDescr.offset.o.p = 0.0;
        orcaDescr.offset.o.y = scannerOffset.theta();
        orcaDescr.size.l     = 0.1;
        orcaDescr.size.w     = 0.1;
        orcaDescr.size.h     = 0.1;

        return orcaDescr;
    }

}

