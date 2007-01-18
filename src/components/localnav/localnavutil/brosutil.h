#ifndef BROSUTIL_H
#define BROSUTIL_H

#include <orca/bros1.h>

namespace localnav {

    void setToZero( orca::Frame3d &f );
    bool isZero( const orca::Frame3d &f );

}

#endif
