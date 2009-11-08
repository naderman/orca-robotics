#ifndef ORCASCANUTIL_CONVERSIONS_H
#define ORCASCANUTIL_CONVERSIONS_H

#include <hydroscanutil/scanutil.h>
#include <hydronavutil/pose.h>
#include <orca/rangescanner2d.h>
#include <hydrotime/time.h>

namespace orcascanutil {

    hydroscanutil::ScannerConfig 
    convertConfig( const orca::RangeScanner2dDescription &descr );

    hydronavutil::Pose convertOffset( const orca::RangeScanner2dDescription &descr );

    orca::RangeScanner2dDescription
    convert( const hydroscanutil::ScannerConfig &scannerConfig,
             const hydronavutil::Pose           &scannerOffset,
             const hydrotime::Time              &timeStamp = hydrotime::Time(0,0) );
}

#endif
