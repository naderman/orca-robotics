/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>
#include <orcaprobe/orcaprobe.h>
#include <orcaifaceutil/pathfollower2d.h>

#include "pathfollower2dprobe.h"

using namespace std;
using namespace orcaprobefactory;

PathFollower2dProbe::PathFollower2dProbe( const orca::FQInterfaceName& name, 
                                          const Ice::ObjectPrx&        adminPrx, 
                                          orcaprobe::AbstractDisplay&  display,
                                          const orcaice::Context&      context )
    : InterfaceProbe(name,adminPrx,display,context)
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
    orca::PathFollower2dPrx derivedPrx = orca::PathFollower2dPrx::checkedCast(prx_);
    orcaprobe::reportResult( data, "data", ifaceutil::toString( derivedPrx->getData() ) );
    return 0;
}

int 
PathFollower2dProbe::loadSubscribe( orcacm::OperationData & data )
{
    Ice::ObjectPtr consumer = this;
    orca::PathFollower2dConsumerPrx callbackPrx =
            orcaice::createConsumerInterface<orca::PathFollower2dConsumerPrx>( ctx_, consumer );

    orca::PathFollower2dPrx derivedPrx = orca::PathFollower2dPrx::checkedCast(prx_);
    derivedPrx->subscribe( callbackPrx );
    orcaprobe::reportSubscribed( data );
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
    res.text = ifaceutil::toString(pfData);
    data.results.push_back( res );

    display_.setOperationData( data );

    std::cout << "PathFollower2dProbe: setData(): " << ifaceutil::toString(pfData) << std::endl;
};

void 
PathFollower2dProbe::setState( const orca::PathFollower2dState &state, const Ice::Current&)
{
    orcacm::OperationData data;
    // this is the result for operation "subscribe" which has user index=2;
    fillOperationData( orcaprobe::UserIndex+2, data );

    orcacm::ResultHeader res;
    res.name = "state";
    stringstream ss; ss << ifaceutil::toString(state);
    res.text = ss.str();
    data.results.push_back( res );

    display_.setOperationData( data );

    std::cout << "PathFollower2dProbe: "<<__func__<<"(): " << ifaceutil::toString(state) << std::endl;
}
