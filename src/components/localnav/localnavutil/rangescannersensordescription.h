/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef RANGESCANNERSENSORDESCRIPTION_H
#define RANGESCANNERSENSORDESCRIPTION_H

#include "isensordescription.h"
#include <orca/rangescanner2d.h>

namespace localnav {

//!
//! @author Alex Brooks
//!
class RangeScannerSensorDescription : public ISensorDescription
{

public: 

    ~RangeScannerSensorDescription(){};

    void setDescr( const orca::RangeScanner2dDescription rangeDescr ){ rangeDescr_ = rangeDescr; };
    orca::RangeScanner2dDescription rangeDescr(){ return rangeDescr_; };

private: 

    orca::RangeScanner2dDescription rangeDescr_;

};

}

#endif
