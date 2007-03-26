/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef DRIVEBICYCLECONTROL_H
#define DRIVEBICYCLECONTROL_H

#include <orcaifaceimpl/proxiedconsumerI.h>
#include <orcaice/proxy.h>
#include <orca/drivebicycle.h>

#include <localnavutil/icontrol.h>
#include <localnavutil/drivebicyclecontroldata.h>

namespace localnav {

//!
//! @author Alex Brooks, Jonathan Sprinkle
//!

//! Specific implementation of @ref IControl for @ref DriveBicycle information
class DriveBicycleControl : public IControl
{

public: 

  DriveBicycleControl( const orcaice::Context&    context );
  virtual ~DriveBicycleControl();

  // returns 1 if successful, otherwise 0
  virtual int connectToInterface();
  // returns 1 if successful, otherwise 0
  virtual int subscribe();

  virtual void setCommand( const IControlData& cmd );

//   virtual IControlData& createCommand( );
  
  virtual bool isProxyEmpty();
  
  virtual orca::VehicleDescription description();
//   IControlDescription& description();
// TODO fix the stuff below here!!!
  virtual void setSimProxy( orcaice::Proxy<orca::DriveBicycleData>*  cmdProxy );
  
  virtual std::string toString( ) const;

private: 

  orcaifaceimpl::ProxiedConsumerI<orca::DriveBicycleConsumer,orca::DriveBicycleData>* cmdConsumer_;
  
  orcaice::Proxy<orca::DriveBicycleData>* cmdProxy_;
  
  orca::DriveBicyclePrx controlPrx_;

  orca::DriveBicycleConsumerPrx cmdConsumerPrx_;
  
  orca::VehicleDescription vehicleDescr_;
  
  DriveBicycleControlData driveBicycleControlData_;

  orcaice::Context context_;
};

}

#endif // #ifndef DRIVEBICYCLECONTROL_H
