#ifndef ORCASCANUTIL_CONVERSIONS_H
#define ORCASCANUTIL_CONVERSIONS_H

#include <hydroscanutil/scanutil.h>
#include <hydronavutil/pose.h>
#include <orca/rangescanner2d.h>

namespace orcascanutil {

    hydroscanutil::ScannerConfig 
    convertConfig( const orca::RangeScanner2dDescription &descr );

    hydronavutil::Pose convertOffset( const orca::RangeScanner2dDescription &descr );
}

#endif
