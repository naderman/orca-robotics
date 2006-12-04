/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <vector>
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>

#include "interfaceprobe.h"

using namespace orcaprobe;

InterfaceProbe::InterfaceProbe( const orca::FQInterfaceName & name, DisplayDriver & display,
                            const orcaice::Context & context )
    : name_(name),
      displayDriver_(display),
      context_(context)
{
    // the generic proxy is created just once and then reused
    prx_ = context_.communicator()->stringToProxy( orcaice::toString( name_ ) );
};

std::vector<orcacm::OperationHeader> 
InterfaceProbe::operations()
{
    std::vector<orcacm::OperationHeader> headers;
    
    orcacm::OperationHeader op;

    for ( uint i=0; i<operations_.size(); ++i ) {
        op.name = operations_[i];
        headers.push_back( op );
    }   

    return headers;
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
        data.name = operations_[index];
    }
}
