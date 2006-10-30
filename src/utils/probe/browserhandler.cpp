/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>
#include <IceGrid/Admin.h>

#include "browserhandler.h"
#include "displaydriver.h"

// interface handlers
#include "homeprobe.h"
#include "powerprobe.h"

using namespace std;
using namespace probe;

BrowserHandler::BrowserHandler( orcaice::Buffer<BrowserEvent> & eventPipe,
                                DisplayDriver & display,
                                const orcaice::Context & context )
    : eventPipe_(eventPipe),
      displayDriver_(display),
      ifaceProbe_(0),
      context_(context)
{
}

BrowserHandler::~BrowserHandler()
{
}

void 
BrowserHandler::run()
{
    BrowserEvent event;
    
    while ( isActive() )
    {
        //cout<<"waiting for an event..."<<endl;
        eventPipe_.getAndPopNext( event );

        switch ( event )
        {
        case ActivateEvent :
            //cout<<"load event"<<endl;
            activate();
            break;
        case ReloadEvent :
            //cout<<"reload event"<<endl;
            reload();
            break;
        case UpEvent :
            //cout<<"up event"<<endl;
            up();
            break;
        case FaultEvent :
            //cout<<"fault event"<<endl;
            fault();
            break;
        case DeactivateEvent :
            //cout<<"stop event"<<endl;
            deactivate();
            break;
        default :
        {
            if ( event >= PickEvent ) {
                pick_ = event - PickEvent;
                pick();
            }
            else {
                cout<<"unknown event "<<event<<". Ignoring..."<<endl;
                eventPipe_.push( FaultEvent );
            }
            break;
        }
        } // switch
    } // while
}

void 
BrowserHandler::loadRegistry()
{
    //cout<<"loading registry data for :"<<context_.communicator()->getDefaultLocator()->ice_toString()<<endl;
    
    registryData_ = orcacm::getRegistryData( context_, context_.communicator()->getDefaultLocator()->ice_toString() );
    
    displayDriver_.showRegistryData( registryData_ );
}

void 
BrowserHandler::loadComponent()
{
    //cout<<"loading component data for "<<orcaice::toString(registryData_.adapters[pick_].name)<<endl;
    lastComponentPick_ = pick_;
    
    componentData_ = orcacm::getComponentData( context_,
                        orcaice::toString(registryData_.adapters[pick_].name) );

    /*
    // mark the interfaces as (un)supported
    for ( uint i=0; i<componentData_.provides.size(); ++i ) {
        std::string id = componentData_.provides[i].id;
        if ( id == "::orca::Home" || id == "::orca::Power" ) {
            componentData_.provides[i].isSupported = true;
        }
        else {
            componentData_.provides[i].isSupported = false;
        }
    }
    */
    displayDriver_.showComponentData( componentData_ );
}

void 
BrowserHandler::loadInterface()
{
    //cout<<"loading interface data for "<<componentData_.provides[pick_].name<<endl;
    lastInterfacePick_ = pick_;

    interfaceData_.name.platform = componentData_.name.platform;
    interfaceData_.name.component = componentData_.name.component;
    interfaceData_.name.iface = componentData_.provides[pick_].name;
    interfaceData_.id = componentData_.provides[pick_].id;   

    //
    // Load interface handler
    //
    if ( interfaceData_.id == "::orca::Home" ) {
        ifaceProbe_ = new HomeProbe( interfaceData_.name, displayDriver_, context_ );
    }
    else if ( interfaceData_.id == "::orca::Power" ) {
        ifaceProbe_ = new PowerProbe( interfaceData_.name, displayDriver_, context_ );
    }
    else {
        cout<<"unsupported interface. Sending fault event."<<endl;
        eventPipe_.push( FaultEvent );
        return;
    }
    interfaceData_.operations = ifaceProbe_->operations();
    
    displayDriver_.showInterfaceData( interfaceData_ );
}

void 
BrowserHandler::loadOperation()
{
    //cout<<"loading operation data for "<<interfaceData_.operations[pick_].name<<endl;
    lastOperationPick_ = pick_;

    if ( ifaceProbe_->loadOperation( pick_ ) ) {
        eventPipe_.push( FaultEvent );
    }
    
    operationData_ = ifaceProbe_->getOperationData( pick_ );
    
    displayDriver_.showOperationData( operationData_ );
}

void 
BrowserHandler::quit()
{
}
