/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ICONTROL_H
#define ICONTROL_H

#include <localnavutil/icontroldata.h>
#include <orcaice/ptrproxy.h>
#include <orca/vehicledescription.h>
#include <iostream>

namespace localnav {

//!
//! @author Alex Brooks, Ben Upcroft, and Jonathan Sprinkle
//!

//! This is an abstract class for controllers for various vehicles. Although navigation is a common problem, the
//! interface for sending vehicle commands should be abstracted to reflect the type of vehicle for which the
//! localnav driver was written. This interface allows particular drivers to determine that at config time,
//! and have the appropriate controller implementation be created at runtime.
//! 
//! This class should be differentiated from @ref IState classes, which are the interface for
//! querying the <em>state</em> of the controller at the current timestep. @ref IState is therefore
//! a readonly source of information for controller status, while @ref IControl is the writeable control input
//! for the vehicle.
//! 
//! Inspiration from this class comes from the @ref ISensorModel interface.
//!
//! All controller types should inherit from this class. Rather than implementing connections/subscriptions 
//! to interfaces directly in mainloop.cpp, all connections/subscriptions should be implemented in the child classes.
//!
class IControl
{

public: 

    virtual ~IControl(){};

    //! Abstract function which wraps up the standard orcaice::createConsumerInterface<>() 
    //! and orcaice::connectToInterfaceWithTag<>() functions.
    //! Returns 1 if successful, otherwise 0
    virtual int connectToInterface()=0;
    //! Abstract function which wraps up the standard proxy->subscribe() function.
    //! Returns 1 if successful, otherwise 0
    virtual int subscribe()=0;
    
    //! Method for setting the control command for the next timestep.
    //! 
    //! TODO Determine whether the cmd data needs to be allocated on the heap (JMS: I don't think so)
    //! Should throw an exception if there's a problem
    virtual void setCommand( const IControlData& cmd )=0;

    //! Returns the description of the controller as an abstract object.
    //! All drivers must dynamic_cast the data into the required format
//     virtual IControllerDescription& description()=0;
    virtual orca::VehicleDescription description()=0;
    
    //! Creates a 'blank' IControlData.
    //! Allocates memory which the caller is responsible to delete.
//     virtual IControlData& createCommand( )=0;
    
    //! returns true if the proxy is empty
    virtual bool isProxyEmpty()=0;
    
    virtual std::string toString( ) const=0;
    
    friend std::ostream& operator<<( std::ostream& out, const IControl& rhs ) 
    { 
      out << rhs.toString( );
      return out;
    }
private: 


};

enum ControlType
{
  velocityControl2d= 0,
  driveBicycle = 1
};

}

#endif //#ifndef ICONTROL_H
