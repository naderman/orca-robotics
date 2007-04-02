/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef RANGESCANNERSENSORDATA_H
#define RANGESCANNERSENSORDATA_H

#include "isensordata.h"
#include <orca/rangescanner2d.h>

namespace localnav {

//!
//! @author Alex Brooks
//!

//! Specific implementation of ISensorData for RangeScanner2dData
class RangeScannerSensorData : public ISensorData
{

public: 

    ~RangeScannerSensorData(){};

    //! get the RangeScanner2dData timestamp
    virtual const orca::Time &timeStamp() const{ return rangeData_->timeStamp; };

    //! Record the RangeScanner2dData in a member variable
    void setData( const orca::RangeScanner2dDataPtr rangeData ){ rangeData_ = rangeData; };
    //! Get the RangeScanner2dData
    const orca::RangeScanner2dDataPtr &rangeData() const { return rangeData_; };

private: 

    orca::RangeScanner2dDataPtr rangeData_;

};

}

#endif
