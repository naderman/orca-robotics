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
#include "velocitycontrol2dstate.h"
#include "velocitycontrol2dcontroldata.h"

using namespace std;

namespace localnav {

//!
//! @author Alex Brooks
//! 
//! Adapted by Jonathan Sprinkle for VelocityControl2dState
//!

  VelocityControl2dState::VelocityControl2dState( const orcaice::Context&    context )
  : 
    odomConsumer_(new orcaifaceimpl::proxiedOdometry2dConsumer(context)),
    odomProxy_(NULL),
    context_(context)
{
}

std::string VelocityControl2dState::toString( ) const
{
  std::stringstream out;
  out << "FIXME there should be a better toString operation for VelocityControl2dState";
  return out.str( );
}



int 
VelocityControl2dState::connectToInterface()
{
    int result = 0;
    try {
        orcaice::connectToInterfaceWithTag<orca::Odometry2dPrx>( context_, odomPrx_, "Odometry2d" );
        odomProxy_ =  &(odomConsumer_->proxy());
        result = 1;
    }
    catch( Ice::Exception &e )
    {
        stringstream ss; ss << "Error while connecting to VehicleState (i.e., odometry): " << e;
        context_.tracer()->error( ss.str() );
    }
    catch( std::exception &e )
    {
        stringstream ss; ss << "Error while connecting to VehicleState (i.e., odometry): " << e.what();
        context_.tracer()->error( ss.str() );
    }
    return result;
}

int 
VelocityControl2dState::subscribe()
{
    int result=0;
    try {
        odomPrx_->subscribe( odomConsumer_->consumerPrx() );
        result=1;
    }
    catch( Ice::Exception &e )
    {
        stringstream ss; ss << "Error while subscribing to VehicleState (i.e., odometry): " << e;
        context_.tracer()->error( ss.str() );
    }
    catch( std::exception &e )
    {
        stringstream ss; ss << "Error while subscribing to VehicleState (i.e., odometry): " << e.what();
        context_.tracer()->error( ss.str() );
    }
    return result;
}

IStateData* 
VelocityControl2dState::getNext( const int timeoutMs )
{
    odomProxy_->get( odomData_ );
    state_.setData( odomData_ );
    return dynamic_cast<IStateData*>(&state_);
}

orca::VehicleDescription 
VelocityControl2dState::description()
{
  // FIXME does anything need to be done to set this value?
  assert( false );
  throw orca::OrcaException( "I never thought you'd ask the state about the VehicleDescription!" );
  return vehicleDescr_;
}

bool
VelocityControl2dState::isProxyEmpty()
{
   return odomProxy_->isEmpty();
}

} // namespace

