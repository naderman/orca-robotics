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
#include "drivebicyclecontrol.h"

using namespace std;

namespace localnav {

//!
//! @author Alex Brooks
//! 
//! Adapted by Jonathan Sprinkle for DriveBicycleControl
//!

DriveBicycleControl::DriveBicycleControl( const orcaice::Context&    context )
  : 
    cmdConsumer_(new orcaifaceimpl::ProxiedConsumerI<orca::DriveBicycleConsumer,orca::DriveBicycleData>),
    cmdProxy_(NULL),
    context_(context)
{
}

DriveBicycleControl::~DriveBicycleControl()
{
  if ( cmdConsumer_ ) delete cmdConsumer_;
}

std::string DriveBicycleControl::toString( ) const
{
  std::stringstream out;
  out << "FIXME there should be a better toString operation for DriveBicycleControl";
  return out.str( );
}



int 
DriveBicycleControl::connectToInterface()
{
  int result = 0;
     Ice::ObjectPtr cmdConsumerPtr = cmdConsumer_;

    try {
      cmdConsumerPrx_ = orcaice::createConsumerInterface<orca::DriveBicycleConsumerPrx>( context_, cmdConsumerPtr );
      cmdProxy_  = &(cmdConsumer_->proxy_);
      orcaice::connectToInterfaceWithTag<orca::DriveBicyclePrx>( context_, controlPrx_, "DriveBicycle" );
      context_.tracer()->debug("connected to a 'DriveBicycle' interface",5);
        
      result = 1;
    }
    catch( std::exception &e )
    {
      stringstream ss; ss << "Error while connecting to control interface: " << e.what();
      context_.tracer()->error( ss.str() );
    }
    catch( Ice::Exception &e )
    {
      stringstream ss; ss << "Error while connecting to control interface: " << e;
      context_.tracer()->error( ss.str() );
    }
    return result;
}

int 
DriveBicycleControl::subscribe()
{
/*  // FIXME: don't know what to do here
  assert( false );
  throw( "ERROR: need to determine what the subscribe method does for the controller" );
  return 0;*/
  
    try {
      controlPrx_->subscribe(  cmdConsumerPrx_ );
      return 1;
    }
    catch( std::exception &e )
    {
      stringstream ss; ss << "Error while subscribing to control interface: " << e.what();
      context_.tracer()->error( ss.str() );
    }
    catch( Ice::Exception &e )
    {
      stringstream ss; ss << "Error while subscribing to control interface: " << e;
      context_.tracer()->error( ss.str() );
    }
    return 0;
}

void
DriveBicycleControl::setCommand( const IControlData& cmd )
{
  const localnav::DriveBicycleControlData &cmdData = dynamic_cast<localnav::DriveBicycleControlData &>( const_cast<IControlData&>(cmd) );
  assert( &cmdData != NULL );
  if( &cmdData != NULL )
  {
    orca::DriveBicycleData command = cmdData.driveBicycleData( );
    controlPrx_->setCommand( command );
  }
  return;
}

/*IControlData&
DriveBicycleControl::createCommand( )
{
  IControlData *result = dynamic_cast<IControlData*>(new DriveBicycleControlData( ));
  if( result == NULL )
  {
    // throw an exception, bad things have happened!
    throw( "Unable to allocate memory, or bad dynamic_cast, during creation of DriveBicycleControl::createCommand( )" );
  }
  
  return *result;
}
*/
orca::VehicleDescription 
DriveBicycleControl::description()
{
  vehicleDescr_ = controlPrx_->getDescription( );
  return vehicleDescr_;
}

bool
DriveBicycleControl::isProxyEmpty()
{
  return cmdProxy_->isEmpty();
}

void
DriveBicycleControl::setSimProxy( orcaice::Proxy<orca::DriveBicycleData>*  cmdProxy )
{
  cmdProxy_ = cmdProxy;
}

} // namespace

