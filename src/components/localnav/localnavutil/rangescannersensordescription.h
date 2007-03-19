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

//! Specific implementation of ISensorDescription for RangeScanner2d. Note that
class RangeScannerSensorDescription : public ISensorDescription
{

public: 

    ~RangeScannerSensorDescription(){};

    //! record RangeScanner2dDescription in a member variable
    void setDescr( const orca::RangeScanner2dDescription rangeDescr ){ rangeDescr_ = rangeDescr; };
    //! get the RangeScanner2dDescription
    orca::RangeScanner2dDescription rangeDescr(){ return rangeDescr_; };

private: 

    orca::RangeScanner2dDescription rangeDescr_;

};

}

#endif
