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
#include "velocitycontrol2dcontrol.h"

using namespace std;

namespace localnav {

//!
//! @author Alex Brooks
//! 
//! Adapted by Jonathan Sprinkle for VelocityControl2dControl
//!

VelocityControl2dControl::VelocityControl2dControl( const orcaice::Context&    context )//, 
//                                                     orca::VehicleDescription vehicleDescr)
  : 
//     cmdConsumer_(new orcaifaceimpl::ProxiedConsumerI<orca::VelocityControl2dConsumer,orca::VelocityControl2dData>),
    cmdProxy_(NULL),
//     vehicleDescr_(vehicleDescr ),
    context_(context)
{
}

VelocityControl2dControl::~VelocityControl2dControl()
{
//     if ( cmdConsumer_ ) delete cmdConsumer_;
}

std::string VelocityControl2dControl::toString( ) const
{
  std::stringstream out;
  out << "FIXME there should be a better toString operation for VelocityControl2dControl";
  return out.str( );
}



int 
VelocityControl2dControl::connectToInterface()
{
  int result = 0;

    try {
      orcaice::connectToInterfaceWithTag<orca::VelocityControl2dPrx>( context_, controlPrx_, "VelocityControl2d" );
      context_.tracer()->debug("connected to a 'VelocityControl2d' interface",5);
        
      result = 1;
    }
    catch( Ice::Exception &e )
    {
      stringstream ss; ss << "Error while connecting to control interface: " << e;
      context_.tracer()->error( ss.str() );
    }
    catch( std::exception &e )
    {
      stringstream ss; ss << "Error while connecting to control interface: " << e.what();
      context_.tracer()->error( ss.str() );
    }
    return result;

    //////////////// based on this, taken from mainloop.cpp
/*
    //TODO:  replace with generic base class IControl() similarly to subscribeForObservations()
    while ( isActive() )
    {
      try 
      {
      // connect to the control
        orcaice::connectToInterfaceWithTag<VelocityControl2dPrx>( context_, velControl2dPrx_, "VelocityControl2d" );
        context_.tracer()->debug("connected to a 'VelocityControl2d' interface",5);
        break;
      }
      catch ( std::exception &e )
      {
        stringstream ss; ss << "Error when connecting to VelocityControl2d interface: " << e.what();
        context_.tracer()->error( ss.str() );
      }
      catch ( Ice::Exception &e )
      {
        stringstream ss; ss << "Error when connecting to VelocityControl2d interface: " << e;
        context_.tracer()->error( ss.str() );
      }
      sleep(2);
    }
    while ( isActive() )
    {
      try 
      {
            // Get the vehicle description
        vehicleDescr_ = velControl2dPrx_->getDescription();
        break;
      }
      catch ( std::exception &e )
      {
        stringstream ss; ss << "Error when connecting to VelocityControl2d interface: " << e.what();
        context_.tracer()->error( ss.str() );
      }
      catch ( Ice::Exception &e )
      {
        stringstream ss; ss << "Error when connecting to VelocityControl2d interface: " << e;
        context_.tracer()->error( ss.str() );
      }
      sleep(2);
    }

  */



}

int 
VelocityControl2dControl::subscribe()
{
  // FIXME: don't know what to do here
  assert( false );
  throw( "ERROR: need to determine what the subscribe method does for the controller" );
  return 0;
  /*
    try {
      controlPrx_->subscribe(  controlConsumerPrx_ );
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
    return 0;*/
}


// int 
// VelocityControl2dControl::subscribe()
// {
//     try {
//       controlPrx_->subscribe(  controlConsumerPrx_ );
//       return 1;
//     }
//     catch( std::exception &e )
//     {
//       stringstream ss; ss << "Error while subscribing to control interface: " << e.what();
//       context_.tracer()->error( ss.str() );
//     }
//     catch( Ice::Exception &e )
//     {
//       stringstream ss; ss << "Error while subscribing to control interface: " << e;
//       context_.tracer()->error( ss.str() );
//     }
//     return 0;
// }
// 

void
VelocityControl2dControl::setCommand( const IControlData& cmd )
{
  const localnav::VelocityControl2dControlData &cmdData = dynamic_cast<localnav::VelocityControl2dControlData &>( const_cast<IControlData&>(cmd) );
  assert( &cmdData != NULL );
  if( &cmdData != NULL )
  {
    orca::VelocityControl2dData command = cmdData.velocityControl2dData( );
    controlPrx_->setCommand( command );
  }
  return;
/*    orca::OgMapData ogMapData;

    int sensorRet = obsProxy_->getNext( ogMapData, timeoutMs );
    if ( sensorRet != 0 )
    {
        stringstream ss;
        ss << "Timeout waiting for ogmap data: no data for " << timeoutMs << "ms.  Stopping.";
        context_.tracer()->error( ss.str() );
        return 0;
    }
    else
    {
        ogMapSensorData_.setData( ogMapData );
        return &ogMapSensorData_;
    }*/
}

// IControlData&
// VelocityControl2dControl::createCommand( )
// {
//   IControlData *result = dynamic_cast<IControlData*>(new VelocityControl2dControlData( ));
//   if( result == NULL )
//   {
//     // throw an exception, bad things have happened!
//     throw( "Unable to allocate memory, or bad dynamic_cast, during creation of VelocityControl2dControl::createCommand( )" );
//   }
//   
//   return *result;
// }

orca::VehicleDescription 
VelocityControl2dControl::description()
{
  vehicleDescr_ = controlPrx_->getDescription( );
  return vehicleDescr_;
//  return vehicleDescr_;
  /*  return controlDescription_;*/
}

bool
VelocityControl2dControl::isProxyEmpty()
{
  return cmdProxy_->isEmpty();
}

    
// void
// VelocityControl2dControl::setSimProxy( orcaice::Proxy<orca::VelocityControl2dData>*  cmdProxy )
// {
//   cmdProxy_ = cmdProxy;
// }
// 

} // namespace

