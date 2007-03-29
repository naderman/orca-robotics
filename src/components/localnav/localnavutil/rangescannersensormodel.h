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

#include <orcaifaceimpl/consumertypes.h>
#include <orcaice/ptrproxy.h>
#include <orca/rangescanner2d.h>

#include <localnavutil/isensormodel.h>
#include <localnavutil/isensordata.h>
#include <localnavutil/rangescannersensordata.h>
#include <localnavutil/rangescannersensordescription.h>

namespace localnav {

//!
//! @author Alex Brooks
//!

//! Specific implementation of ISensorModel for RangeScanner2d information
class RangeScannerSensorModel : public ISensorModel
{

public: 

    RangeScannerSensorModel( const orcaice::Context&    context );
    ~RangeScannerSensorModel();

    // returns 1 if successful, otherwise 0
    int connectToInterface();
    // returns 1 if successful, otherwise 0
    int subscribe();

    ISensorData* getNext( const int timeoutMs );

    bool isProxyEmpty();
    
    ISensorDescription& description();

    void setSimProxy( orcaice::Proxy<orca::RangeScanner2dDataPtr>*  obsProxy );

private: 

    orcaifaceimpl::proxiedRangeScanner2dConsumerPtr obsConsumer_;
    
    orcaice::Proxy<orca::RangeScanner2dDataPtr>* obsProxy_;
    
    orca::RangeScanner2dPrx obsPrx_;

    RangeScannerSensorDescription rangeScannerSensorDescription_;
    RangeScannerSensorData rangeScannerSensorData_;

    orca::RangeScanner2dDescription rangeDescr_;

    orcaice::Context context_;
};

}

#endif
