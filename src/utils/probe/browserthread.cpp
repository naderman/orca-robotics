/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>
#include <orcaprobe/orcaprobe.h>
#include <IceGrid/Admin.h>

#include "browserthread.h"
#include "browserevents.h"

using namespace std;
using namespace probe;

BrowserThread::BrowserThread( orcaprobe::IDisplay & display,
                                std::vector<orcaprobe::Factory*> &factories,
                                const orcaice::Context & context ) :
    SafeThread(context.tracer()),
    factories_(factories),
    display_(display),
    events_(new hydroiceutil::EventQueue),
    ifaceProbe_(0),
    context_(context)
{
}

BrowserThread::~BrowserThread()
{
    delete ifaceProbe_;
}

void
BrowserThread::chooseActivate()
{
//     cout<<"chooseActivate"<<endl;
    hydroiceutil::EventPtr e = new probe::ActivateEvent;
    events_->add( e );
}

void 
BrowserThread::chooseReload()
{
    hydroiceutil::EventPtr e = new probe::ReloadEvent;
    events_->add( e );
}

void 
BrowserThread::chooseUp()
{
    hydroiceutil::EventPtr e = new probe::UpEvent;
    events_->add( e );
}

void 
BrowserThread::chooseTop()
{
    hydroiceutil::EventPtr e = new probe::TopEvent;
    events_->add( e );
}

void 
BrowserThread::choosePick( int pick )
{
//     cout<<"choosePick() pick="<<pick<<endl;
    hydroiceutil::EventPtr e = new probe::PickEvent( pick );
    events_->add( e );
}

void 
BrowserThread::chooseFilter( const std::string & filter )
{
    hydroiceutil::EventPtr e = new probe::FilterEvent; // FilterEvent( filter_ );
    events_->add( e );
}

void
BrowserThread::chooseDeactivate()
{
    hydroiceutil::EventPtr e = new probe::DeactivateEvent;
    events_->add( e );
}

void 
BrowserThread::walk()
{
    hydroiceutil::EventPtr event;
    int timeoutMs = 500;
    
    while ( !isStopping() )
    {
        if ( !events_->timedGet( event, timeoutMs ) ) {
            continue;
        }

        switch ( event->type() )
        {
        // approx in order of call frequency
        case Pick : {
//             cout<<"pick event"<<endl;
            PickEventPtr e = PickEventPtr::dynamicCast( event );
            pick_ = e->pick_;
            pick();
            break;
        }
        case Up : {
            //cout<<"up event"<<endl;
            up();
            break;
        }
        case Top : {
            //cout<<"up event"<<endl;
            top();
            break;
        }
        case Reload : {
//             cout<<"reload event"<<endl;
            reload();
            break;
        }
        case Filter : {
            //cout<<"filter event"<<endl;
            filterRegistry();
            break;
        }
        case Activate : {
            cout<<"activate event"<<endl;
            activate();
            break;
        }
        case Fault : {
            //cout<<"fault event"<<endl;
            fault();
            break;
        }
        case Deactivate : {
            //cout<<"stop event"<<endl;
            deactivate();
            break;
        }
        default : {
            cout<<"unknown browser event "<<event->type()<<". Ignoring..."<<endl;
            hydroiceutil::EventPtr e = new probe::FaultEvent;
            events_->add( e );
        }
        } // switch
    } // while
}

void 
BrowserThread::loadRegistry()
{
    cout<<"loading registry data for :"<<context_.communicator()->getDefaultLocator()->ice_toString()<<endl;
    
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
BrowserThread::showRegistry()
{
    display_.setFocus( orcaprobe::IDisplay::RegistryFocus );
}

void 
BrowserThread::filterRegistry()
{
//     cout<<"filtering registry data for :"<<context_.communicator()->getDefaultLocator()->ice_toString()<<endl;

    // simply call showRegistryData() again. the driver will filter it.
    display_.setRegistryData( registryData_ );
}

void 
BrowserThread::loadPlatform()
{
//     cout<<"loading platform data for "<<registryData_.platforms[pick_].name<<endl;
    lastPlatformPick_ = pick_;
    
    platformData_ = orcacm::home2hierarch2( registryHomeData_, registryData_.platforms[pick_] );

    display_.setPlatformData( platformData_ );
}

void 
BrowserThread::showPlatform()
{
    display_.setFocus( orcaprobe::IDisplay::PlatformFocus );
}

void 
BrowserThread::loadComponent()
{
//     cout<<"loading component data for "<<endl; //<<orcaice::toString(platformData_.homes[pick_].name)<<endl;
    lastComponentPick_ = pick_;
    
    //
    // remote call!
    //
    display_.showNetworkActivity( true );
    componentData_ = orcacm::getComponentHomeData( context_, platformData_.homes[pick_].proxy );
    display_.showNetworkActivity( false );

    // todo: this is a bit ugly
    componentData_.locatorString = platformData_.locatorString;

//     cout<<"DEBUG: got a list of "<<componentData_.provides.size()<<" provided interfaces for "<<orcaice::toString(componentData_.name)<<endl;
    display_.setComponentData( componentData_ );
}

void 
BrowserThread::showComponent()
{
    display_.setFocus( orcaprobe::IDisplay::ComponentFocus );
}

void 
BrowserThread::loadInterface()
{
    // special case: the interface is actually created every time, so here we try to delete it to avoid mem leak.
    //cout<<"unloading interface"<<endl;
    if ( ifaceProbe_ ) {
        // most of our interfaces are now Ice smart pointers, can't delete them.
        // but some of them are not and it's a problem
//         delete ifaceProbe_;
        ifaceProbe_ = 0;
    }
    
//     cout<<"DEBUG: will load interface "<<pick_<<"/"<<componentData_.provides.size()<<endl;
//     cout<<"DEBUG: loading interface data for "<<componentData_.provides[pick_].name<<endl;
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

    // if specific probe was not found in the factories, load generic probe
    if ( ifaceProbe_==0 ) {
        ifaceProbe_ = new orcaprobe::InterfaceProbe( interfaceData_.name, display_, context_ );
    }
    
    // local call
    interfaceData_.operations = ifaceProbe_->operations();

    display_.setInterfaceData( interfaceData_ );
}

void 
BrowserThread::showInterface()
{
    display_.setFocus( orcaprobe::IDisplay::InterfaceFocus );
}

void 
BrowserThread::loadOperation()
{
    //cout<<"loading operation data for "<<interfaceData_.operations[pick_].name<<endl;
    lastOperationPick_ = pick_;

    //
    // remote call!
    //
    display_.showNetworkActivity( true );
    if ( ifaceProbe_->loadOperation( pick_, operationData_ ) ) {
        events_->add( new probe::FaultEvent );
    }
    display_.showNetworkActivity( false );

    // todo: this is a bit ugly
    operationData_.locatorString = interfaceData_.locatorString;

    display_.setOperationData( operationData_ );
}

void 
BrowserThread::showOperation()
{
    display_.setFocus( orcaprobe::IDisplay::OperationFocus );
}

void 
BrowserThread::quit()
{
    cout<<"Quitting..."<<endl;
    cout<<"Not implemented, use Ctrl-C."<<endl;
}
