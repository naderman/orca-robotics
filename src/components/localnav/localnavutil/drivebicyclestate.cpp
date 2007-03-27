/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <orcaice/orcaice.h>
#include "drivebicyclestate.h"
#include "drivebicyclestatedata.h"

using namespace std;

namespace localnav {

//!
//! @author Alex Brooks
//! 
//! Adapted by Jonathan Sprinkle for DriveBicycleState
//!

  DriveBicycleState::DriveBicycleState( const orcaice::Context&    context )
  : 
    cmdConsumer_(new orcaifaceimpl::ProxiedConsumerI<orca::DriveBicycleConsumer,orca::DriveBicycleData>),
    cmdProxy_(NULL),
  context_(context)
{
}

DriveBicycleState::~DriveBicycleState()
{
  if ( cmdConsumer_ ) delete cmdConsumer_;
}

std::string DriveBicycleState::toString( ) const
{
  std::stringstream out;
  out << "FIXME there should be a better toString operation for DriveBicycleState";
  return out.str( );
}



int 
DriveBicycleState::connectToInterface()
{
  int result = 0;
  Ice::ObjectPtr cmdConsumerPtr = cmdConsumer_;

  try {
    controlConsumerPrx_ = orcaice::createConsumerInterface<orca::DriveBicycleConsumerPrx>( context_, cmdConsumerPtr );
    cmdProxy_  = &(cmdConsumer_->proxy_);
    orcaice::connectToInterfaceWithTag<orca::DriveBicyclePrx>( context_, controlPrx_, "DriveBicycle" );
    context_.tracer()->debug("connected to a 'DriveBicycle' state interface",5);
    result=1;
  }
  catch( std::exception &e )
  {
    stringstream ss; ss << "Error while connecting to state interface: " << e.what();
    context_.tracer()->error( ss.str() );
  }
  catch( Ice::Exception &e )
  {
    stringstream ss; ss << "Error while connecting to state interface: " << e;
    context_.tracer()->error( ss.str() );
  }
  
  return result;
}

int 
DriveBicycleState::subscribe()
{
    int result=0;
    try {
      controlPrx_->subscribe(  controlConsumerPrx_ );
      result=1;
    }
    catch( std::exception &e )
    {
      stringstream ss; ss << "Error while subscribing to state interface: " << e.what();
      context_.tracer()->error( ss.str() );
    }
    catch( Ice::Exception &e )
    {
      stringstream ss; ss << "Error while subscribing to state interface: " << e;
      context_.tracer()->error( ss.str() );
    }
  
    return result;
}

IStateData* 
DriveBicycleState::getNext( const int timeoutMs )
{
  orca::DriveBicycleData stateData;
  
  int stateRet = cmdProxy_->getNext( stateData, timeoutMs );
  if ( stateRet != 0 )
  {
    stringstream ss;
    // FIXME HACK Is this wise to do, to return last known data?
    ss << "Timeout waiting for state data: no data for " << timeoutMs << "ms.  Returning last known data.";
    context_.tracer()->error( ss.str() );
  }
  else
  {
    // See DriveBicycleStateData::getTimestamp() for more comments.
//     stringstream ss;
//     orca::Time timestamp=orcaice::getNow();
//     ss << "Although a timeout of " << timeoutMs << "[ms] was specified, we return data independent of that. ";
//     ss << "Any calls to this->stateData_.getTimestamp() will return the time at which this message was generated: " << orcaice::toString(timestamp);
    stateData_.setData( stateData );
//     stateData_.setTimeStamp( timestamp );
//     context_.tracer()->warning( ss.str() );
  }
  
  return dynamic_cast<IStateData*>(&stateData_);
}

orca::VehicleDescription 
DriveBicycleState::description()
{
  vehicleDescr_ = controlPrx_->getDescription( );
  return vehicleDescr_;
}

bool
DriveBicycleState::isProxyEmpty()
{
  return ( cmdProxy_->isEmpty() );
}

} // namespace

