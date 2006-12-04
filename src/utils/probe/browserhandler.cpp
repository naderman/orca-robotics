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
#include <orcaprobe/orcaprobe.h>
#include <IceGrid/Admin.h>

#include "browserhandler.h"

using namespace std;
using namespace probe;

BrowserHandler::BrowserHandler( orcaprobe::DisplayDriver & display,
                                std::vector<orcaprobe::Factory*> &factories,
                                const orcaice::Context & context )
    : factories_(factories),
      display_(display),
      ifaceProbe_(0),
      context_(context)
{
}

BrowserHandler::~BrowserHandler()
{
    delete ifaceProbe_;
}

void
BrowserHandler::chooseActivate()
{
    eventPipe_.push( ActivateEvent );
}

void 
BrowserHandler::chooseReload()
{
    eventPipe_.push( ReloadEvent );
}

void 
BrowserHandler::chooseUp()
{
    eventPipe_.push( UpEvent );
}

void 
BrowserHandler::chooseTop()
{
    eventPipe_.push( TopEvent );
}

void 
BrowserHandler::choosePick( int pick )
{
    pick_ = pick;
    eventPipe_.push( PickEvent );
}

void 
BrowserHandler::chooseFilter( const std::string & filter )
{
    filter_ = filter;
    eventPipe_.push( FilterEvent );
}

void
BrowserHandler::chooseDeactivate()
{
    eventPipe_.push( DeactivateEvent );
}

void 
BrowserHandler::run()
{
    BrowserEvent event;
    
    while ( isActive() )
    {
//         cout<<"BrowserHandler: waiting for an event..."<<endl;
        eventPipe_.getAndPopNext( event );

        switch ( event )
        {
        // approx in order of call frequency
        case PickEvent :
            //cout<<"pick event"<<endl;
            pick();
            break;
        case UpEvent :
            //cout<<"up event"<<endl;
            up();
            break;
        case TopEvent :
            //cout<<"up event"<<endl;
            top();
            break;
        case ReloadEvent :
            //cout<<"reload event"<<endl;
            reload();
            break;
        case FilterEvent :
            //cout<<"filter event"<<endl;
            filterRegistry();
            break;
        case ActivateEvent :
            //cout<<"load event"<<endl;
            activate();
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
            cout<<"unknown event "<<event<<". Ignoring..."<<endl;
            eventPipe_.push( FaultEvent );
        } // switch
    } // while
}

void 
BrowserHandler::loadRegistry()
{
    //cout<<"loading registry data for :"<<context_.communicator()->getDefaultLocator()->ice_toString()<<endl;
    
    //
    // remote call!
    //
//     registryData_ = orcacm::getRegistryData( context_, context_.communicator()->getDefaultLocator()->ice_toString() );
    registryData_ = orcacm::getRegistryHomeData( context_, context_.communicator()->getDefaultLocator()->ice_toString() );
    
    display_.showRegistryData( registryData_ );
}

void 
BrowserHandler::filterRegistry()
{
    cout<<"filtering registry data for :"<<context_.communicator()->getDefaultLocator()->ice_toString()<<endl;

    // simply call showRegistryData() again. the driver will filter it.
    display_.showRegistryData( registryData_ );
}

void 
BrowserHandler::loadComponent()
{
    //cout<<"loading component data for "<<orcaice::toString(registryData_.adapters[pick_].name)<<endl;
    lastComponentPick_ = pick_;
    
    //
    // remote call!
    //
//     componentData_ = orcacm::getComponentData( context_,
//                         orcaice::toString(registryData_.adapters[pick_].name) );
    componentData_ = orcacm::getComponentHomeData( context_, registryData_.homes[pick_].proxy );

    display_.showComponentData( componentData_ );
}

void 
BrowserHandler::loadInterface()
{
    // special case: the interface is actually created every time, so here we try to delete it to avoid mem leak.
    //cout<<"unloading interface"<<endl;
    if ( ifaceProbe_ ) {
        delete ifaceProbe_;
    }

    //cout<<"loading interface data for "<<componentData_.provides[pick_].name<<endl;
    lastInterfacePick_ = pick_;

    interfaceData_.name.platform = componentData_.name.platform;
    interfaceData_.name.component = componentData_.name.component;
    interfaceData_.name.iface = componentData_.provides[pick_].name;
    interfaceData_.id = componentData_.provides[pick_].id;   

    //
    // Load interface handler
    //
    // one of them must support it, otherwise it would not have an index
    for ( uint i=0; i < factories_.size(); ++i )
    {
        // if this interface is not supported, skip this factory
        if ( factories_[i]->isSupported( interfaceData_.id ) ) {
            ifaceProbe_ = factories_[i]->create( interfaceData_.id, interfaceData_.name, display_, context_ );
        }

    }

    if ( ifaceProbe_==0 ) {
        cout<<"Unsupported interface: '"<<interfaceData_.id<<"'. Sending fault event."<<endl;
        eventPipe_.push( FaultEvent );
        return;
    }
    
    // local call
    interfaceData_.operations = ifaceProbe_->operations();

    display_.showInterfaceData( interfaceData_ );
}

void 
BrowserHandler::loadOperation()
{
    //cout<<"loading operation data for "<<interfaceData_.operations[pick_].name<<endl;
    lastOperationPick_ = pick_;

    //
    // remote call!
    //
    if ( ifaceProbe_->loadOperation( pick_, operationData_ ) ) {
        eventPipe_.push( FaultEvent );
    }
    
    display_.showOperationData( operationData_ );
}

void 
BrowserHandler::quit()
{
    cout<<"Quitting..."<<endl;
    cout<<"Not implemented, use Ctrl-C."<<endl;
}
