/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef VELOCITYCONTROL2DCONTROL_H
#define VELOCITYCONTROL2DCONTROL_H

#include <orcaifaceimpl/proxiedconsumerI.h>
#include <orcaice/proxy.h>
#include <orca/drivebicycle.h>

#include <localnavutil/icontrol.h>
#include <localnavutil/velocitycontrol2dcontroldata.h>
#include <orca/vehicledescription.h>

namespace localnav {

//!
//! @author Alex Brooks, Jonathan Sprinkle
//!

//! Specific implementation of @ref IControl for @ref VelocityControl2d information
class VelocityControl2dControl : public IControl
{

public: 

  VelocityControl2dControl( const orcaice::Context&    context ); //,
//                             orca::VehicleDescription vehicleDescr);
  virtual ~VelocityControl2dControl();

  // returns 1 if successful, otherwise 0
  virtual int connectToInterface();
  // returns 1 if successful, otherwise 0
  virtual int subscribe();

  virtual void setCommand( const IControlData& cmd );
  
//   virtual IControlData& createCommand( );

  virtual bool isProxyEmpty();
  
  virtual orca::VehicleDescription description();
  
  virtual std::string toString( ) const;

private: 

  orcaice::Proxy<orca::VelocityControl2dData>* cmdProxy_;
   
  orca::VelocityControl2dPrx controlPrx_;

  VelocityControl2dControlData controlData_;

  orca::VehicleDescription vehicleDescr_;
  
  orcaice::Context context_;
};

}

#endif // #ifndef VELOCITYCONTROL2DCONTROL_H
