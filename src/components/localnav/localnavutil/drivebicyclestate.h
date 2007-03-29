/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef DRIVEBICYCLESTATE_H
#define DRIVEBICYCLESTATE_H

#include <orcaifaceimpl/consumertypes.h>
#include <orcaice/proxy.h>
#include <orca/drivebicycle.h>

#include <localnavutil/istate.h>
#include <localnavutil/drivebicyclestatedata.h>
#include <orca/vehicledescription.h>

namespace localnav {

//!
//! @author Alex Brooks, Jonathan Sprinkle
//!

//! Specific implementation of @ref IState for @ref DriveBicycle information
class DriveBicycleState : public IState
{

public: 

  DriveBicycleState( const orcaice::Context&    context );
  virtual ~DriveBicycleState();

  // returns 1 if successful, otherwise 0
  virtual int connectToInterface();
//   // returns 1 if successful, otherwise 0
  virtual int subscribe();

  //! IMPORTANT NOTE: Even though a timeout value is specified, we always
  //! return whatever is in the top of the buffer over the icestorm 
  //! connection. We generate a warning for this inside this method, 
  //! but after fetching the timestamp for the data (done to determine whether
  //! the timestamps are logical) no warning is generated, despite the fact
  //! that is the point at which we should word, simply because we do
  //! not have a context there. Since we have a context here, we present
  //! the warning for any potential fetchers of the timestamp of the data
  //! at the time the data is fetched.
  //! 
  //! See @ref DriveBicycleStateData::getTimestamp() for more info.
  virtual IStateData* getNext( const int timeoutMs );

  virtual bool isProxyEmpty();
  
  virtual orca::VehicleDescription description();
  
  std::string toString() const;

private: 

  
  orcaifaceimpl::proxiedDriveBicycleConsumerPtr cmdConsumer_;
  orcaice::Proxy<orca::DriveBicycleData>* cmdProxy_;
  orca::DriveBicyclePrx controlPrx_;

  DriveBicycleStateData stateData_;

  orca::VehicleDescription vehicleDescr_;
  
  orcaice::Context context_;
};

}

#endif // #ifndef DRIVEBICYCLESTATE_H
