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
      display_(display),
      context_(context)
{
    // the generic proxy is created just once and then reused
    prx_ = context_.communicator()->stringToProxy( orcaice::toString( name_ ) );
};

void 
InterfaceProbe::addOperation( const std::string & name, const std::string & signature )
{
    orcacm::OperationHeader op;
   
    op.name = name;
    op.signature = signature;
    operations_.push_back( op );
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
