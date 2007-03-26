/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef OGMAPSENSORMODEL_H
#define OGMAPSENSORMODEL_H

#include <orcaifaceimpl/proxiedconsumerI.h>
#include <orcaice/proxy.h>
#include <orca/ogmap.h>

#include <localnavutil/isensormodel.h>
#include <localnavutil/isensordata.h>
#include <localnavutil/ogmapsensordata.h>
#include <localnavutil/ogmapsensordescription.h>

namespace localnav {

//!
//! @author Alex Brooks
//!

//! Specific implementation of ISensorModel for OgMap information
class OgMapSensorModel : public ISensorModel
{

public: 

    OgMapSensorModel( const orcaice::Context&    context );
    ~OgMapSensorModel();

    // returns 1 if successful, otherwise 0
    int connectToInterface();
    // returns 1 if successful, otherwise 0
    int subscribe();

    ISensorData* getNext( const int timeoutMs );

    bool isProxyEmpty();
    
    ISensorDescription& description();

    void setSimProxy( orcaice::Proxy<orca::OgMapData>*  obsProxy );

private: 

    orcaifaceimpl::ProxiedConsumerI<orca::OgMapConsumer,orca::OgMapData>* obsConsumer_;
    
    orcaice::Proxy<orca::OgMapData>* obsProxy_;
    
    orca::OgMapPrx obsPrx_;

    orca::OgMapConsumerPrx obsConsumerPrx_;

    OgMapSensorDescription ogMapSensorDescription_;
    OgMapSensorData ogMapSensorData_;

    // orca::OgMapDescription ogMapDescr_;
    
    orcaice::Context context_;
};

}

#endif
