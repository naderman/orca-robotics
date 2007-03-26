/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ISTATE_H
#define ISTATE_H

#include <orcaice/ptrproxy.h>

namespace localnav {

//!
//! @author Alex Brooks, Ben Upcroft, and Jonathan Sprinkle
//!

// forward declarations
class IStateData;
  
//! This is an abstract class for controls for various vehicles. Although navigation is a common problem, the
//! interface for sending vehicle commands should be abstracted to reflect the type of vehicle for which the
//! localnav driver was written. This interface allows particular drivers to determine that at config time,
//! and have the appropriate control implementation be created at runtime.
//! 
//! This class should be differentiated from @ref IState classes, which are the interface for
//! querying the <em>state</em> of the control at the current timestep. @ref IState is therefore
//! a readonly source of information for control status, while @ref IControl is the writeable control input
//! for the vehicle.
//! 
//! Inspiration from this class comes from the @ref ISensorModel interface.
//!
//! All control types should inherit from this class. Rather than implementing connections/subscriptions 
//! to interfaces directly in mainloop.cpp, all connections/subscriptions should be implemented in the child classes.
//!
class IState
{

public: 

  virtual ~IState(){};

  //! Abstract function which wraps up the standard orcaice::createConsumerInterface<>() 
  //! and orcaice::connectToInterfaceWithTag<>() functions.
  //! Returns 1 if successful, otherwise 0
  virtual int connectToInterface()=0;
  //! Abstract function which wraps up the standard proxy->subscribe() function.
  //! Returns 1 if successful, otherwise 0
  virtual int subscribe()=0;
  
  //! Method for getting the control command at the current timestep for 
  //! classes such as @ref DriveBicycle, which need to know our steering angle and other
  //! information that is not available from odometry. The method 
  //! returns an abstract data object. All pathplanning drivers recieve the data 
  //! as an argument in this IStateData
  //! form and must dynamic_cast() the data into the required format. 
  //!
  //! 
  virtual IStateData* getNext( const int timeoutMs )=0;

  //! Returns the description of the control as an abstract object.
  //! All drivers must dynamic_cast the data into the required format
  virtual orca::VehicleDescription description()=0;
  
  //! returns true if the proxy is empty
  virtual bool isProxyEmpty()=0;
    
  // this only works for Odometry2dData at the moment.
  //TODO: need to make it more general
  virtual void setSimProxy( orcaice::Proxy<orca::Odometry2dData>*  odomProxy ){ std::cout << "WARNING (istate.h): Classes inheriting from the base class IState should implement this function" << std::endl;};
  
  
private: 


};

}

#endif //#ifndef ISTATE
