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
//! @author Alex Brooks and Ben Upcroft
//!

//! This is an abstract class for sensor information (such as laser data or an og map) useful for path planning.
//! This class wraps up the connections and subscriptions to interfaces specific to certain sensor types. It also
//! provides abstract methods for getting sensor type data and sensor type descriptions.
//!
//! All sensor information types should inherit from this class. Rather than implementing connections/subscriptions 
//! to interfaces directly in mainloop.cpp, all connections/subscriptions should be implemented in the child classes.
//!
class ISensorModel
{

public: 

    virtual ~ISensorModel(){};

    //! Abstract function which wraps up the standard orcaice::createConsumerInterface<>() 
    //! and orcaice::connectToInterfaceWithTag<>() functions.
    //! Returns 1 if successful, otherwise 0
    virtual int connectToInterface()=0;
    //! Abstract function which wraps up the standard proxy->subscribe() function.
    //! Returns 1 if successful, otherwise 0
    virtual int subscribe()=0;
    
    //! Method for getting the sensor information data (such as LaserScanner2dData or OgMapData)
    //! and returns an abstract data object. All pathplanning drivers recieve the data 
    //! as an argument in this ISensorData
    //! form and must dynamic_cast() the data into the required format. 
    //!
    //! Allocates memory for ISensorData.
    //! Returns NULL if there's a problem.
    virtual ISensorData* getNext( int timeoutMs )=0;

    //! Returns the description of the sensor information as an abstract object.
    //! All drivers must dynamic_cast the data into the required format
    virtual ISensorDescription& description()=0;
    
    //! returns true if the proxy is empty
    virtual bool isProxyEmpty()=0;
    
    // this only works for range data at the moment.
    //TODO: need to make it more general
    virtual void setSimProxy( orcaice::PtrProxy<orca::RangeScanner2dDataPtr>*  obsProxy ){ std::cout << "WARNING (isensormodel.h): Classes inheriting from the base class ISensorModel should implement this function" << std::endl;};
    
private: 


};

enum SensorModelType
{
    rangeModel = 0,
    ogMapModel = 1
};

}

#endif
