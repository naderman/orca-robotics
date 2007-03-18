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

#include <orcaifaceimpl/ptrproxiedconsumerI.h>
#include <orcaice/ptrproxy.h>
#include <orca/rangescanner2d.h>

#include "isensormodel.h"
#include "isensordata.h"
#include "rangescannersensordata.h"
#include "rangescannersensordescription.h"

namespace localnav {

//!
//! @author Alex Brooks
//!
class RangeScannerSensorModel : public ISensorModel
{

public: 

    RangeScannerSensorModel( const orcaice::Context&    context );
    ~RangeScannerSensorModel();

    void subscribeForInfo();

    ISensorData* getNext( const int timeoutMs );

    bool isProxyEmpty();
    
    ISensorDescription* description();

    void setSimProxy( orcaice::PtrProxy<orca::RangeScanner2dDataPtr>*  obsProxy );

private: 

    orcaifaceimpl::PtrProxiedConsumerI<orca::RangeScanner2dConsumer,orca::RangeScanner2dDataPtr>* obsConsumer_;
    
    orcaice::PtrProxy<orca::RangeScanner2dDataPtr>* obsProxy_;

    orca::RangeScanner2dConsumerPrx obsConsumerPrx_;

    RangeScannerSensorDescription rangeScannerSensorDescription_;
    RangeScannerSensorData rangeScannerSensorData_;

    orca::RangeScanner2dDescription rangeDescr_;
    


    orcaice::Context context_;
};

}

#endif
