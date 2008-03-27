#ifndef ORCALOCALNAVUTIL_BROSUTIL_H
#define ORCALOCALNAVUTIL_BROSUTIL_H

#include <orca/bros1.h>

namespace orcalocalnavutil {

    void setToZero( orca::Frame3d &f );
    bool isZero( const orca::Frame3d &f );

}

#endif
