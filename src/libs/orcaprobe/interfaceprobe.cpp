/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <vector>
#include <iostream>
#include <sstream>
#include <orcaice/orcaice.h>
#include <orcaice/icegridutils.h>
#include <orcaice/convertutils.h>
#include <orcacm/orcacm.h>

#include "interfaceprobe.h"

using namespace std;
using namespace orcaprobe;

InterfaceProbe::InterfaceProbe( const orca::FQInterfaceName& fqIName, const Ice::ObjectPrx& adminPrx,
                AbstractDisplay & display, const orcaice::Context & context )
    : name_(fqIName),
      display_(display),
      ctx_(context)
{
    // this is the least common denominator: all Orca objects derive from Ice::Object.
    // A derived class will OVERWRITE this with its own type.
    id_ = "::Ice::Object";

    // these are the basic operations which all Ice::Objects support.
    // A derived class will ADD its own operations to these.
    addOperation( "ice_ping", "void ice_ping()" );

    // the generic proxy is created just once and then reused
    // Standard interfaces are treated differently
    const orca::FQComponentName fqCName = orcaice::toComponent( name_ );
    if ( name_.iface == orcaice::toAdminFacet(fqCName,"::orca::Home") ||
         name_.iface == orcaice::toAdminFacet(fqCName,"::orca::Status") ||
         name_.iface == orcaice::toAdminFacet(fqCName,"::orca::Tracer") )
    {
        prx_ = adminPrx->ice_facet( name_.iface );
    }
    else
    {
        prx_ = ctx_.communicator()->stringToProxy( orcaice::toString( name_ ) );
    }
};

InterfaceProbe::~InterfaceProbe() 
{
    cout<<"InterfaceProbe::~InterfaceProbe()"<<endl;
}

string
InterfaceProbe::toString() const
{
    stringstream ss;
    ss << "Probe type="<<id_<<" instance="<<orcaice::toString(name_);
    return ss.str();
}

void 
InterfaceProbe::addOperation( const std::string& name, bool isSupported, const std::string& signature )
{
    orcacm::OperationHeader op;
   
    op.name = name;
    op.signature = signature;
    op.isSupported = isSupported;
    operations_.push_back( op );
}

void 
InterfaceProbe::addOperation( const std::string & name, const std::string & signature, bool isSupported )
{
    addOperation( name, isSupported, signature );
}

int 
InterfaceProbe::loadOperation( const int index, orcacm::OperationData & data )
{
    fillOperationData( index, data );
    data.results.clear();
    
    switch ( index )
    {
    // process standard operations
    case IcePingIndex :
        return loadIcePing( data );
    // process derived operations
    default :
        // base implementation will return error because we are given a non-base operation option
        return loadOperationEvent( index, data );
    }
    return 1;
}

int 
InterfaceProbe::loadOperationEvent( const int index, orcacm::OperationData & data )
{
    return 1;
}

int 
InterfaceProbe::loadIcePing( orcacm::OperationData & data )
{
    orcacm::ResultHeader res;
    
    try
    {
        prx_->ice_ping();
    }
    catch( const Ice::Exception & e )
    {
        stringstream ss;
        ss << e;
        res.name = "exception";
        res.text = ss.str();
        data.results.push_back( res );
        return 1;
    }

//     cout<<orcaice::toString(result)<<endl;
    res.name = "outcome";
    res.text = "ping successful";
    data.results.push_back( res );
    return 0;
}

void
InterfaceProbe::fillOperationData( const int index, orcacm::OperationData & data )
{
    data.parent = name_;
    data.parentId = id_;
    if ( index<0 || (unsigned int)index>operations_.size() ) {
        data.name = "unknown";
    }
    else {
        data.name = operations_[index].name;
    }
}
