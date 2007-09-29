/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>

#include "calls.h"

using namespace std;
using namespace orcaqcm;

GetComponentsCall::GetComponentsCall( const orcaice::Context & context, const std::string & locatorString,
                orcaiceutil::Notify<orcacm::ComponentData>* homePipe )
    : context_(context),
      locatorString_(locatorString),
      homePipe_(homePipe)
{
}

void
GetComponentsCall::execute()
{
    //cout<<"default locator (refresh) :"<<context_.communicator()->getDefaultLocator()->ice_toString()<<endl;

    // don't try to ping adapters here, we'll have to do it anyway below, when we get their full information
    bool pingAdapters = false;

    // this function talks to the Registry
//     orcacm::RegistryFlatData regData = orcacm::getRegistryData( context_, locatorString_, pingAdapters );
    orcacm::RegistryHomeData regData = orcacm::getRegistryHomeData( context_, locatorString_, pingAdapters );

    if ( !regData.isReachable ) {
        cerr<<"registry unreachable"<<endl;
        return;
    }

    cout<<"retrieved a list of "<<regData.homes.size()<<" adapters"<<endl;

    orcacm::ComponentData compData;
    
    for ( unsigned int i=0; i<regData.homes.size(); ++i ) {

        // this function talks to the Home interface
//         compData = orcacm::getComponentData( context_, regData.adapters[i].name );
        compData = orcacm::getComponentHomeData( context_, regData.homes[i].proxy );

        compData.locatorString = regData.locatorString;
        compData.adminAddress = regData.address;
        //
        // push results one by one to UserHandler
        //
        homePipe_->set( compData );
    }

}
