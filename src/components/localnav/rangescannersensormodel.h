/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef RANGESCANNERSENSORMODEL_H
#define RANGESCANNERSENSORMODEL_H

#include "isensormodel.h"
#include "isensordata.h"

namespace localnav {

//!
//! @author Alex Brooks
//!
class RangeScannerSensorModel : public ISensorModel
{

public: 

    RangeScannerSensorModel();
    ~RangeScannerSensorModel();

    void subscribeForInfo();

    ISensorData *getNext( int timeoutMs );

private: 


};

}

#endif
