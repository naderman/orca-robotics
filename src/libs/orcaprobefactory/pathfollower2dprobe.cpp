/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>
#include <orcaprobe/orcaprobe.h>
#include <orcaifacestring/pathfollower2d.h>

#include "pathfollower2dprobe.h"

using namespace std;
using namespace orcaprobefactory;

PathFollower2dProbe::PathFollower2dProbe( const orca::FQInterfaceName & name,
                                          orcaprobe::AbstractDisplay & display,
                                          const orcaice::Context & context )
    : InterfaceProbe(name,display,context)
{
    id_ = "::orca::PathFollower2d";
    
    addOperation( "getData" );
    addOperation( "subscribe" );
    addOperation( "unsubscribe" );
}
    
int 
PathFollower2dProbe::loadOperationEvent( const int index, orcacm::OperationData & data )
{
    switch ( index )
    {
    case orcaprobe::UserIndex :
        return loadGetData( data );
    case orcaprobe::UserIndex+1 :
        return loadSubscribe( data );
    case orcaprobe::UserIndex+2 :
        return loadUnsubscribe( data );
    }
    return 1;
}

int 
PathFollower2dProbe::loadGetData( orcacm::OperationData & data )
{
    orca::PathFollower2dData result;
    try
    {
        orca::PathFollower2dPrx derivedPrx = orca::PathFollower2dPrx::checkedCast(prx_);
        result = derivedPrx->getData();
        orcaprobe::reportResult( data, "data", ifacestring::toString(result) );
    }
    catch( const orca::DataNotExistException & e )
    {
        orcaprobe::reportException( data, "data is not ready on the remote interface" );
    }
    catch( const orca::HardwareFailedException & e )
    {
        orcaprobe::reportException( data, "remote hardware failure" );
    }
    catch( const Ice::Exception & e )
    {
        stringstream ss;
        ss<<e<<endl;
        orcaprobe::reportException( data, ss.str() );
    }
    return 0;
}

int 
PathFollower2dProbe::loadSubscribe( orcacm::OperationData & data )
{
    Ice::ObjectPtr consumer = this;
    orca::PathFollower2dConsumerPrx callbackPrx =
            orcaice::createConsumerInterface<orca::PathFollower2dConsumerPrx>( ctx_, consumer );
    try
    {
        orca::PathFollower2dPrx derivedPrx = orca::PathFollower2dPrx::checkedCast(prx_);
        derivedPrx->subscribe( callbackPrx );
        orcaprobe::reportSubscribed( data );
    }
    catch( const Ice::Exception & e )
    {
        stringstream ss;
        ss<<e<<endl;
        orcaprobe::reportException( data, ss.str() );
    }
    return 0;
}

int 
PathFollower2dProbe::loadUnsubscribe( orcacm::OperationData & data )
{
    orcaprobe::reportNotImplemented( data );
    return 0;
}

void 
PathFollower2dProbe::setData(const orca::PathFollower2dData &pfData, const Ice::Current&)
{
    orcacm::OperationData data;
    // this is the result for operation "subscribe" which has user index=2;
    fillOperationData( orcaprobe::UserIndex+2, data );

    orcacm::ResultHeader res;
    res.name = "data";
    res.text = ifacestring::toString(pfData);
    data.results.push_back( res );

    display_.setOperationData( data );

    std::cout << "PathFollower2dProbe: setData(): " << ifacestring::toString(pfData) << std::endl;
};

void 
PathFollower2dProbe::setWaypointIndex( int index, const Ice::Current&)
{
    orcacm::OperationData data;
    // this is the result for operation "subscribe" which has user index=2;
    fillOperationData( orcaprobe::UserIndex+2, data );

    orcacm::ResultHeader res;
    res.name = "waypointIndex";
    stringstream ss; ss << index;
    res.text = ss.str();
    data.results.push_back( res );

    display_.setOperationData( data );

    std::cout << "PathFollower2dProbe: setWaypointIndex(): " << index << std::endl;
}
void 
PathFollower2dProbe::setActivationTime(const orca::Time &absoluteTime, double relativeTime, const Ice::Current&)
{
    orcacm::OperationData data;
    // this is the result for operation "subscribe" which has user index=2;
    fillOperationData( orcaprobe::UserIndex+2, data );

    orcacm::ResultHeader res;
    res.name = "activationTime";
    stringstream ss; ss << "relative: " << relativeTime << ", absolute: " << ifacestring::toString(absoluteTime);
    res.text = ss.str();
    data.results.push_back( res );

    display_.setOperationData( data );

    std::cout << "PathFollower2dProbe: setActivationTime(): rel: " << relativeTime << "s, abs: " << ifacestring::toString( absoluteTime ) << std::endl;
}
void 
PathFollower2dProbe::setEnabledState(bool enabledState, const Ice::Current&)
{
    orcacm::OperationData data;
    // this is the result for operation "subscribe" which has user index=2;
    fillOperationData( orcaprobe::UserIndex+2, data );

    orcacm::ResultHeader res;
    res.name = "enabledState";
    stringstream ss; ss << enabledState;
    res.text = ss.str();
    data.results.push_back( res );

    display_.setOperationData( data );

    std::cout << "PathFollower2dProbe: setEnabledState(): " << enabledState << std::endl;
}
