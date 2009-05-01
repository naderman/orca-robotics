/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>

#include "jobs.h"
#include "homeevent.h"

using namespace std;
using namespace orcaqcm;

GetComponentsJob::GetComponentsJob( QCoreApplication* app, QObject* ocmModel, const orcaice::Context& context, const std::string& locatorString ) : 
    app_(app),
    ocmModel_(ocmModel),
    locatorString_(locatorString),
    context_(context)
{
}

void
GetComponentsJob::execute()
{
    //cout<<"default locator (refresh) :"<<context_.communicator()->getDefaultLocator()->ice_toString()<<endl;

    // this function talks to the Registry
    orcacm::RegistryHomeData regData = orcacm::getRegistryHomeData( context_, locatorString_ );

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
//         homePipe_->set( compData );
        app_->postEvent( ocmModel_, (QEvent*)new HomeEvent( compData ) );
    }

}
