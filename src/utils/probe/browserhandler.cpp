/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko
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

BrowserHandler::BrowserHandler( const orcaice::EventQueuePtr & myQueue, 
                                const orcaice::EventQueuePtr & otherQueue, 
                                std::vector<orcaprobe::Factory*> &factories,
                                const orcaice::Context & context ) :
    myQueue_(myQueue),
    otherQueue_(otherQueue),
    factories_(factories),
    ifaceProbe_(0),
    context_(context)
{
    eventPipe_.configure( 10 );
}

BrowserHandler::~BrowserHandler()
{
    delete ifaceProbe_;
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
BrowserHandler::run()
{
    orcaice::EventPtr eventPtr;
    int timeoutMs = 500;
    
    while ( isActive() )
    {
        try {
            // block with timeout on my events (change in verbosity)
            ret = myQueue_->timedGet( eventPtr, timeoutMs );
        }

        switch ( eventPtr->type() )
        {
        // approx in order of call frequency
        case PickEventType :
//             cout<<"pick event"<<endl;
            PickEventPtr pickEventPtr = PickEvent::checkedCast( eventPtr );
            pick_ = pickEventPtr->pick_;
            pick();
            break;
        case UpEventType :
            //cout<<"up event"<<endl;
            up();
            break;
        case TopEventType :
            //cout<<"up event"<<endl;
            top();
            break;
        case ReloadEventType :
//             cout<<"reload event"<<endl;
            reload();
            break;
        case FilterEventType :
            //cout<<"filter event"<<endl;
            filterRegistry();
            break;
        case ActivateEventType :
            //cout<<"load event"<<endl;
            activate();
            break;
        case FaultEventType :
            //cout<<"fault event"<<endl;
            fault();
            break;
        case DeactivateEventType :
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
    display_.showNetworkActivity( true );
    bool tryToPing = false;
    registryHomeData_ = orcacm::getRegistryHomeData( context_, 
            context_.communicator()->getDefaultLocator()->ice_toString(),
            tryToPing );
    display_.showNetworkActivity( false );

    registryData_ = orcacm::home2hierarch1( registryHomeData_ );

    display_.setRegistryData( registryData_ );
}

void 
BrowserHandler::showRegistry()
{
    display_.showRegistry();
}

void 
BrowserHandler::filterRegistry()
{
//     cout<<"filtering registry data for :"<<context_.communicator()->getDefaultLocator()->ice_toString()<<endl;

    // simply call showRegistryData() again. the driver will filter it.
    display_.setRegistryData( registryData_ );
}

void 
BrowserHandler::loadPlatform()
{
//     cout<<"loading platform data for "<<registryData_.platforms[pick_].name<<endl;
    lastPlatformPick_ = pick_;
    
    platformData_ = orcacm::home2hierarch2( registryHomeData_, registryData_.platforms[pick_] );

    display_.setPlatformData( platformData_ );
}

void 
BrowserHandler::showPlatform()
{
    display_.showPlatform();
}

void 
BrowserHandler::loadComponent()
{
    cout<<"loading component data for pick="<<pick_<<endl; //orcaice::toString(platformData_.homes[pick_].name)<<endl;
    lastComponentPick_ = pick_;
    
    //
    // remote call!
    //
    display_.showNetworkActivity( true );
    componentData_ = orcacm::getComponentHomeData( context_, platformData_.homes[pick_].proxy );
    display_.showNetworkActivity( false );

    // todo: this is a bit ugly
    componentData_.locatorString = platformData_.locatorString;

    cout<<"DEBUG: got a list of "<<componentData_.provides.size()<<" provided interfaces for "<<orcaice::toString(componentData_.name)<<endl;
    display_.setComponentData( componentData_ );
}

void 
BrowserHandler::showComponent()
{
    display_.showComponent();
}

void 
BrowserHandler::loadInterface()
{
    // special case: the interface is actually created every time, so here we try to delete it to avoid mem leak.
    //cout<<"unloading interface"<<endl;
    if ( ifaceProbe_ ) {
        delete ifaceProbe_;
    }

//     cout<<"loading interface data for "<<componentData_.provides[pick_].name<<endl;
    lastInterfacePick_ = pick_;

    interfaceData_.locatorString = componentData_.locatorString;
    interfaceData_.name.platform = componentData_.name.platform;
    interfaceData_.name.component = componentData_.name.component;
    interfaceData_.name.iface = componentData_.provides[pick_].name;
    interfaceData_.id = componentData_.provides[pick_].id;   

    //
    // Load interface handler
    //
    // one of them must support it, otherwise it would not have an index
    for ( unsigned int i=0; i < factories_.size(); ++i )
    {
        // if this interface is not supported, skip this factory
        if ( factories_[i]->isSupported( interfaceData_.id ) ) {
            ifaceProbe_ = factories_[i]->create( interfaceData_.id, interfaceData_.name, display_, context_ );
        }

    }

    if ( ifaceProbe_==0 ) {
        // load generic interface
        ifaceProbe_ = new orcaprobe::InterfaceProbe( interfaceData_.name, display_, context_ );

//         cout<<"Unsupported interface: '"<<interfaceData_.id<<"'. Sending fault event."<<endl;
//         // alexm: here we are not synch'd with Qt driver!
//         eventPipe_.push( FaultEvent );
        return;
    }
    
    // local call
    interfaceData_.operations = ifaceProbe_->operations();

    cout<<"DEBUG: BrowserHandler is calling setInterfaceData()"<<endl;
    display_.setInterfaceData( interfaceData_ );
}

void 
BrowserHandler::showInterface()
{
    display_.showInterface();
}

void 
BrowserHandler::loadOperation()
{
    //cout<<"loading operation data for "<<interfaceData_.operations[pick_].name<<endl;
    lastOperationPick_ = pick_;

    //
    // remote call!
    //
    display_.showNetworkActivity( true );
    if ( ifaceProbe_->loadOperation( pick_, operationData_ ) ) {
        eventPipe_.push( FaultEvent );
    }
    display_.showNetworkActivity( false );

    // todo: this is a bit ugly
    operationData_.locatorString = interfaceData_.locatorString;

    display_.setOperationData( operationData_ );
}

void 
BrowserHandler::showOperation()
{
    display_.showOperation();
}

void 
BrowserHandler::quit()
{
    cout<<"Quitting..."<<endl;
    cout<<"Not implemented, use Ctrl-C."<<endl;
}
