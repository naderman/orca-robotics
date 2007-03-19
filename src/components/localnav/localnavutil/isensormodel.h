/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ISENSORMODEL_H
#define ISENSORMODEL_H

#include <localnavutil/isensordata.h>
#include <localnavutil/isensordescription.h>
#include <orcaice/ptrproxy.h>
#include <orca/rangescanner2d.h>


namespace localnav {

//!
//! @author Alex Brooks
//!
class ISensorModel
{

public: 

    virtual ~ISensorModel(){};

    // returns 1 if successful, otherwise 0
    virtual int connectToInterface()=0;
    // returns 1 if successful, otherwise 0
    virtual int subscribe()=0;
    
    // Allocates memory for ISensorData.
    // Returns NULL if there's a problem.
    virtual ISensorData* getNext( int timeoutMs )=0;

    // TODO: return of a reference
    virtual ISensorDescription& description()=0;
    
    // returns true if the proxy is empty
    virtual bool isProxyEmpty()=0;
    
    // this only works for range data at the moment.
    //TODO: need to make it more general
    virtual void setSimProxy( orcaice::PtrProxy<orca::RangeScanner2dDataPtr>*  obsProxy )=0;
    
private: 


};

enum SensorModelType
{
    rangeModel = 0,
    ogMapModel = 1
};

}

#endif
