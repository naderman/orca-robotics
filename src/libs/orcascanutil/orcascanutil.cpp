#include "orcascanutil.h"
#include <iostream>
#include <orcaifaceutil/rangescanner2d.h>
#include <gbxutilacfr/exceptions.h>

using namespace std;

namespace orcascanutil {

    hydroscanutil::ScannerConfig 
    convertConfig( const orca::RangeScanner2dDescription &descr )
    {
        return hydroscanutil::ScannerConfig( 
            descr.maxRange,
            descr.startAngle,
            hydroscanutil::calcAngleIncrement( descr.fieldOfView,
                                               descr.numberOfSamples ) );        
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

}

