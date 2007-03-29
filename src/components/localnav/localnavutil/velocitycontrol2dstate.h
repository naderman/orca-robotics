/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef VELOCITYCONTROL2DSTATE_H
#define VELOCITYCONTROL2DSTATE_H

#include <orcaifaceimpl/consumertypes.h>
#include <orcaice/proxy.h>
#include <orca/drivebicycle.h>

#include <localnavutil/istate.h>
#include <localnavutil/velocitycontrol2dstatedata.h>
#include <orca/vehicledescription.h>
#include <orca/odometry2d.h>

namespace localnav {

//!
//! @author Alex Brooks, Jonathan Sprinkle
//!

//! Specific implementation of @ref IState for @ref VelocityControl2d information
class VelocityControl2dState : public IState
{

public: 

  VelocityControl2dState( const orcaice::Context&    context );

  virtual ~VelocityControl2dState() {}

  // returns 1 if successful, otherwise 0
  virtual int connectToInterface();
//   // returns 1 if successful, otherwise 0
  virtual int subscribe();

  virtual IStateData* getNext( int timeoutMs );

  virtual bool isProxyEmpty();
  
  // this only works for Odometry2dData at the moment.
  //TODO: need to make it more general
  virtual void setSimProxy( orcaice::Proxy<orca::Odometry2dData>*  odomProxy )
  {
    odomProxy_=odomProxy;
  }
  
  virtual orca::VehicleDescription description();

  std::string toString() const;

private: 

  orcaifaceimpl::proxiedOdometry2dConsumerPtr odomConsumer_;

  orcaice::Proxy<orca::Odometry2dData>           *odomProxy_;
  orca::Odometry2dPrx            odomPrx_;
  orca::Odometry2dData           odomData_;
    
  VelocityControl2dStateData state_;

  orca::VehicleDescription vehicleDescr_;
  
  orcaice::Context context_;
};

}

#endif // #ifndef VELOCITYCONTROL2DSTATE_H
