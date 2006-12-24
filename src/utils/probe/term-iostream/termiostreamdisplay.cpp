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
#include <iomanip>    // for setw()
#include <orcaice/orcaice.h>

#include "termiostreamdisplay.h"
#include "../termdisplayevents.h"

using namespace std;

namespace probe
{

const std::string inputPrompt = "==> ";
const std::string upPrompt    = " u\tup";
const std::string topPrompt   = " U\ttop";
const std::string reloadPromp = " r\treload";
const std::string quitPrompt  = " q\tquit";
const std::string divider     = "---------------------------------------------";


TermIostreamDisplay::TermIostreamDisplay( const std::vector<std::string> & supportedInterfaces )
    : supportedInterfaces_(supportedInterfaces),
      browser_(0),
      events_(new orcaice::EventQueue)
{
}

TermIostreamDisplay::~TermIostreamDisplay()
{
}

void 
TermIostreamDisplay::enable( orcaprobe::BrowserDriver* browser )
{
    assert( browser || "pointer to browser must be non-zero" );
    if ( !browser ) {
        cout<<"null browser pointer. exitting..."<<endl;
        exit(1);
    }

    browser_ = browser;
    browser_->chooseActivate();

    orcaice::EventPtr event;
    int timeoutMs = 500;

    // this will not return
    for (;;)
    {
        if ( !events_->timedGet( event, timeoutMs ) ) {
            continue;
        }

        switch ( event->type() )
        {
        // approx in order of call frequency
        case FocusChanged : {
            //cout<<"focus changed event"<<endl;
            FocusChangedEventPtr e = FocusChangedEventPtr::dynamicCast( event );
            if ( !e ) {
                cout<<"failed to cast event to FocusChanged"<<endl;
                break;
            }
//             cout<<"changing focus to "<<e->focus_<<endl;
            switch ( e->focus_ )
            {
            case orcaprobe::DisplayDriver::RegistryFocus :
                printRegistryData( registryData_ );
                break;
            case orcaprobe::DisplayDriver::PlatformFocus :
                printPlatformData( platformData_ );
                break;
            case orcaprobe::DisplayDriver::ComponentFocus :
                printComponentData( componentData_ );
                break;
            case orcaprobe::DisplayDriver::InterfaceFocus :
                printInterfaceData( interfaceData_ );
                break;
            case orcaprobe::DisplayDriver::OperationFocus :
                printOperationData( operationData_ );
                break;
            default : {}
            }
            break;
        }
        case NetworkActivityChanged : {
            //cout<<"net activity event"<<endl;
            NetworkActivityChangedEventPtr e = NetworkActivityChangedEventPtr::dynamicCast( event );
            if ( !e ) break;
            printNetworkActivity( e->isActive_ );
            break;
        }
        case RegistryDataChanged : {
            //cout<<"registry event"<<endl;
            RegistryDataChangedEventPtr e = RegistryDataChangedEventPtr::dynamicCast( event );
            if ( !e ) break;
            registryData_ = e->data_;
            break;
        }
        case PlatformDataChanged : {
            //cout<<"platform event"<<endl;
            PlatformDataChangedEventPtr e = PlatformDataChangedEventPtr::dynamicCast( event );
            if ( !e ) break;
            platformData_ = e->data_;
            break;
        }
        case ComponentDataChanged : {
            //cout<<"component event"<<endl;
            ComponentDataChangedEventPtr e = ComponentDataChangedEventPtr::dynamicCast( event );
            if ( !e ) break;
            componentData_ = e->data_;
            break;
        }
        case InterfaceDataChanged : {
            //cout<<"interface event"<<endl;
            InterfaceDataChangedEventPtr e = InterfaceDataChangedEventPtr::dynamicCast( event );
            if ( !e ) break;
            interfaceData_ = e->data_;
            break;
        }
        case OperationDataChanged : {
            //cout<<"operation event"<<endl;
            OperationDataChangedEventPtr e = OperationDataChangedEventPtr::dynamicCast( event );
            if ( !e ) break;
            operationData_ = e->data_;
            break;
        }
        default : {
            cout<<"unknown display event "<<event->type()<<". Ignoring..."<<endl;
            break;
        }
        } // switch
    } // for
}

void 
TermIostreamDisplay::showNetworkActivity( bool isActive )
{
    orcaice::EventPtr e = new probe::NetworkActivityChangedEvent( isActive );
    events_->add( e );
}

void 
TermIostreamDisplay::setRegistryData( const orcacm::RegistryHierarchicalData1 & data )
{
    orcaice::EventPtr e = new probe::RegistryDataChangedEvent( data );
    events_->add( e );
}

void 
TermIostreamDisplay::setPlatformData( const orcacm::RegistryHierarchicalData2 & data )
{
    orcaice::EventPtr e = new probe::PlatformDataChangedEvent( data );
    events_->add( e );
}

void 
TermIostreamDisplay::setComponentData( const orcacm::ComponentData & data )
{
    orcaice::EventPtr e = new probe::ComponentDataChangedEvent( data );
    events_->add( e );
}

void 
TermIostreamDisplay::setInterfaceData( const orcacm::InterfaceData & data )
{
    orcaice::EventPtr e = new probe::InterfaceDataChangedEvent( data );
    events_->add( e );
}

void 
TermIostreamDisplay::setOperationData( const orcacm::OperationData & data )
{
    orcaice::EventPtr e = new probe::OperationDataChangedEvent( data );
    events_->add( e );
}

void 
TermIostreamDisplay::setFocus( orcaprobe::DisplayDriver::FocusType focus )
{
    orcaice::EventPtr e = new probe::FocusChangedEvent( focus );
    events_->add( e );
}

void 
TermIostreamDisplay::printNetworkActivity( bool isActive )
{
    if ( isActive ) {
        cout<<"\nNetwork access ...";
    }
    else {
        cout<<" finished"<<endl;
    }
}

void 
TermIostreamDisplay::printRegistryData( const orcacm::RegistryHierarchicalData1& data )
{
    cout<<endl<<divider<<endl;
    cout<<"Registry : "<<data.locatorString<<endl;
    cout<<divider<<endl;
    cout<<"Select a numbered platform from the list:"<<endl;
    
    // we must use the index of the option list so that the handler understands us
    for ( unsigned int i=0; i<data.platforms.size(); ++i ) 
    {
        // first filter on name
//         std::string adapt = data.homes[i].proxy->ice_getAdapterId();
//         if ( !adapt.compare( 0,filter_.size(), filter_ ) ) 
//         {
            // now filter on reachability
//             if ( data.homes[i].isReachable ) {
                cout<<setw(2)<<i<<"\t"<< data.platforms[i].name <<endl;
//             }
//             else {
//                 cout<<"\t"<< adapt <<endl;
//             }
// 
//         }
    }
    // other options
    cout<<divider<<endl;
    cout<<reloadPromp<<endl;
//     cout<<"f\tFilter"<<endl;
    cout<<quitPrompt<<endl;
    cout<<divider<<endl;
    
    // wait for input
    while (1)
    {
        cout << inputPrompt;
        // block until get a character + Enter

        string userInput;
        cin >> userInput;
        
        if( userInput == "r" )
        {
            filter_.clear();
            browser_->chooseReload();
            break;
        }
//         else if( userInput == "f" )
//         {
//             cout<<"Enter component name filter:"<<endl;
//             cout << inputPrompt;
// 
//             filter_.clear();
//             cin >> filter_;
// 
//             browser_->chooseFilter( filter_ );
//             break;
//         }
        else if( userInput == "q" )
        {
            browser_->chooseDeactivate();
            break;
        }
        else if ( userInput[0]>='0' && userInput[0]<='9' ) {
            int selection = atoi( userInput.c_str() );
            cout<<"you picked "<<selection<<endl;

            browser_->choosePick( selection );
            break;
        }
        else {
            cout<<"invalid selection: '"<<userInput<<"'. try again"<<endl;
        }
    } // while
}

void 
TermIostreamDisplay::printPlatformData( const orcacm::RegistryHierarchicalData2& data )
{
    cout<<endl<<divider<<endl;
    cout<<"Platform : "<<data.platform.name<<endl;
    cout<<divider<<endl;
    cout<<"Select a numbered component from the list:"<<endl;
    
    // we must use the index of the option so that the handler understands us
    for ( unsigned int i=0; i<data.homes.size(); ++i ) 
    {
        // first filter on name
        std::string adapt = data.homes[i].proxy->ice_getAdapterId();
//         if ( !adapt.compare( 0,filter_.size(), filter_ ) ) 
//         {
            // now filter on reachability
            if ( data.homes[i].isReachable ) {
                cout<<setw(2)<<i<<"\t"<< adapt <<endl;
            }
            else {
                cout<<"\t"<< adapt <<endl;
            }
//         }
    }
    // other options
    menu();
    
    // wait for input
    while (1)
    {
        cout << inputPrompt;
        // block until get a character + Enter

        string userInput;
        cin >> userInput;
        
        if( userInput == "u" || userInput == "U" )
        {
            browser_->chooseUp();
            break;
        }
        else if( userInput == "r" )
        {
            filter_.clear();
            browser_->chooseReload();
            break;
        }
        else if( userInput == "q" )
        {
            browser_->chooseDeactivate();
            break;
        }
        else if ( userInput[0]>='0' && userInput[0]<='9' ) {
            int selection = atoi( userInput.c_str() );
            //cout<<"you picked "<<selection<<endl;

            browser_->choosePick( selection );
            break;
        }
        else {
            cout<<"invalid selection: '"<<userInput<<"'. try again"<<endl;
        }
    } // while
}

void 
TermIostreamDisplay::printComponentData( const orcacm::ComponentData& data )
{
    cout<<endl<<divider<<endl;
    cout<<"Component : "<<orcaice::toString( data.name )<<endl;
    cout<<divider<<endl;
    cout<<"Select a numbered interface from the list:"<<endl;

    for ( unsigned int i=0; i<data.provides.size(); ++i ) 
    {
        // we must use the index of the option so that the handler understands us
        for ( unsigned int j=0; j<supportedInterfaces_.size(); ++j )
        {
            // is this interface in the list of supported interfaces?
            if ( supportedInterfaces_[j] == data.provides[i].id ) {
                // if it's supported, print out a number for feedback.
                cout<<setw(2)<<i;
                break;
            }
        } // supported interfaces

        cout<<setw(32)<<data.provides[i].name<<"\t"<<data.provides[i].id<<endl;
    } // provided interfaces

    menu();
    getUserInput();
}

void 
TermIostreamDisplay::printInterfaceData( const orcacm::InterfaceData& data )
{
    cout<<endl<<divider<<endl;
    cout<<"Interface : "<<orcaice::toString( data.name )<<" of type "<<data.id<<endl;
    cout<<divider<<endl;
    cout<<"Select an operation from the list:"<<endl;

    for ( unsigned int i=0; i<data.operations.size(); ++i ) {
        //if ( comps[i].isReachable ) {
            cout<<setw(2)<<i<<"\t"<<data.operations[i].name<<endl;
        //}
    }

    menu();
    getUserInput();

}

void 
TermIostreamDisplay::printOperationData( const orcacm::OperationData& data )
{
    cout<<endl<<divider<<endl;
    cout<<"Operation : "<<data.parentId<<"::"<<data.name<<endl;
    cout<<divider<<endl;
    for ( unsigned int i=0; i<data.results.size(); ++i ) {
        cout<<data.results[i].name<<":"<<endl;
        cout<<data.results[i].text<<endl;
    }
    cout<<divider<<endl;
    cout<<"Select an action from the list:"<<endl;
    
    menu();
    getUserInput();
}

void
TermIostreamDisplay::menu()
{
    cout<<divider<<endl;
    cout<<upPrompt<<endl;
    cout<<topPrompt<<endl;
    cout<<reloadPromp<<endl;
    cout<<quitPrompt<<endl;
    cout<<divider<<endl;
}

void 
TermIostreamDisplay::getUserInput()
{
    while (1)
    {
        cout << inputPrompt;

        // block until get a character + Enter
        string userInput;
        cin >> userInput;
        
        if( userInput == "u" )
        {
            browser_->chooseUp();;
            break;
        }
        else if( userInput == "U" )
        {
            browser_->chooseTop();
            break;
        }
        else if( userInput == "r" )
        {
            browser_->chooseReload();
            break;
        }
        else if( userInput == "q" )
        {
            browser_->chooseDeactivate();
            break;
        }
        // to do: check more than one character
        else if ( userInput[0]>='0' && userInput[0]<='9' ) {
            unsigned int selection = atoi( userInput.c_str() );
            //cout<<"you picked "<<selection<<endl;
            //check
            if ( selection>=0 && selection<componentData_.provides.size() ) {
                browser_->choosePick( selection );
                break;
            }
            else {
                cout<<"invalid numeric selection: '"<<selection<<"'. try again"<<endl;
            }
        }
        else {
            cout<<"invalid selection: '"<<userInput<<"'. try again"<<endl;
        }
    } // while
}

} // namespace
