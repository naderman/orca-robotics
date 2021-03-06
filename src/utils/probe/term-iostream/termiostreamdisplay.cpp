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
#include <iomanip>    // for setw()
#include <orcaice/orcaice.h>
#include <orcaice/icegridutils.h>

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
const std::string divider0    = "=======================================================";
const std::string divider1    = "-------------------------------------------------------";

void topMenu( const std::string& header, const std::string& itemname, const std::string& options )
{
    cout<<"\n\n\n\n\n";
    cout<<endl<<divider0<<endl;
    cout<<header<<"\n";
    cout<<divider1<<"\n";
    cout<<"Select "<<itemname<<" from the list:"<<"\n";
    // options come with their own new line
    cout<<options;
    cout<<divider1<<"\n";
//     cout<<upPrompt<<"\n";
//     cout<<topPrompt<<"\n";
    cout<<reloadPromp<<"\n";
    cout<<quitPrompt<<"\n";
    cout<<divider0<<"\n";
};

void stdMenu( const std::string& header, const std::string& itemname, const std::string& options )
{
    cout<<"\n\n\n\n\n";
    cout<<endl<<divider0<<"\n";
    cout<<header<<"\n";
    cout<<divider1<<"\n";
    cout<<"Select "<<itemname<<" from the list:"<<"\n";
    // options come with their own new line
    cout<<options;
    cout<<divider1<<"\n";
    cout<<upPrompt<<"\n";
    cout<<topPrompt<<"\n";
    cout<<reloadPromp<<"\n";
    cout<<quitPrompt<<"\n";
    cout<<divider0<<"\n";
};

TermIostreamDisplay::TermIostreamDisplay( const std::vector<std::string> & supportedInterfaces ) : 
    supportedInterfaces_(supportedInterfaces),
    browser_(0),
    events_(new hydroiceutil::EventQueue)
{
    cout<<"The following interfaces are supported:";
    for ( unsigned int j=0; j<supportedInterfaces_.size(); ++j ) {
        cout << "\n\t" << supportedInterfaces_[j];
    }
    cout<<endl;
}

TermIostreamDisplay::~TermIostreamDisplay()
{
}

void 
TermIostreamDisplay::enable( orcaprobe::IBrowser* browser )
{
    assert( browser || "pointer to browser must be non-zero" );
    if ( !browser ) {
        cout<<"null browser pointer. exitting..."<<endl;
        exit(1);
    }

    browser_ = browser;
    browser_->chooseActivate();

    hydroiceutil::EventPtr event;
    int timeoutMs = 500;

    //
    // Main loop (will not return)
    //
    for (;;)
    {
        if ( !events_->timedGet( event, timeoutMs ) ) {
            continue;
        }
        cout<<"TermIostreamDisplay::enable(): event type="<<event->type()<<endl;

        switch ( event->type() )
        {
        // approx in order of call frequency
        case FocusChanged : {
            cout<<"focus changed event"<<endl;
            FocusChangedEventPtr e = FocusChangedEventPtr::dynamicCast( event );
            if ( !e ) {
                cout<<"failed to cast event to FocusChanged"<<endl;
                break;
            }
            cout<<"TermIostreamDisplay::enable(): changing focus to "<<e->focus_<<endl;
            switch ( e->focus_ )
            {
            case orcaprobe::AbstractDisplay::RegistryFocus :
                printRegistryData( registryData_ );
                break;
            case orcaprobe::AbstractDisplay::PlatformFocus :
                printPlatformData( platformData_ );
                break;
            case orcaprobe::AbstractDisplay::ComponentFocus :
                printComponentData( componentData_ );
                break;
            case orcaprobe::AbstractDisplay::InterfaceFocus :
                printInterfaceData( interfaceData_ );
                break;
            case orcaprobe::AbstractDisplay::OperationFocus :
            case orcaprobe::AbstractDisplay::ResultFocus :
                printOperationData( operationData_ );
                break;
            }
            break;
        }
        case NetworkActivityChanged : {
            //cout<<"net activity event"<<endl;
            NetworkActivityChangedEventPtr e = NetworkActivityChangedEventPtr::dynamicCast( event );
            assert( e );
            printNetworkActivity( e->isActive_ );
            break;
        }
        case RegistryDataChanged : {
            //cout<<"registry event"<<endl;
            RegistryDataChangedEventPtr e = RegistryDataChangedEventPtr::dynamicCast( event );
            assert( e );
            registryData_ = e->data_;
            break;
        }
        case PlatformDataChanged : {
            //cout<<"platform event"<<endl;
            PlatformDataChangedEventPtr e = PlatformDataChangedEventPtr::dynamicCast( event );
            assert( e );
            platformData_ = e->data_;
            break;
        }
        case ComponentDataChanged : {
            //cout<<"component event"<<endl;
            ComponentDataChangedEventPtr e = ComponentDataChangedEventPtr::dynamicCast( event );
            assert( e );
            componentData_ = e->data_;
            break;
        }
        case InterfaceDataChanged : {
            //cout<<"interface event"<<endl;
            InterfaceDataChangedEventPtr e = InterfaceDataChangedEventPtr::dynamicCast( event );
            assert( e );
            interfaceData_ = e->data_;
            break;
        }
        case OperationDataChanged : {
            //cout<<"operation event"<<endl;
            OperationDataChangedEventPtr e = OperationDataChangedEventPtr::dynamicCast( event );
            assert( e );
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
    events_->add( new probe::NetworkActivityChangedEvent( isActive ) );
}

void 
TermIostreamDisplay::setRegistryData( const orcacm::RegistryHierarchicalData1 & data )
{
    events_->add( new probe::RegistryDataChangedEvent( data ) );
}

void 
TermIostreamDisplay::setPlatformData( const orcacm::RegistryHierarchicalData2 & data )
{
    events_->add( new probe::PlatformDataChangedEvent( data ) );
}

void 
TermIostreamDisplay::setComponentData( const orcacm::ComponentData & data )
{
    events_->add( new probe::ComponentDataChangedEvent( data ) );
}

void 
TermIostreamDisplay::setInterfaceData( const orcacm::InterfaceData & data )
{
    events_->add( new probe::InterfaceDataChangedEvent( data ) );
}

void 
TermIostreamDisplay::setOperationData( const orcacm::OperationData & data )
{
    events_->add( new probe::OperationDataChangedEvent( data ) );
}

void 
TermIostreamDisplay::setFocus( orcaprobe::AbstractDisplay::FocusType focus )
{
cout<<"TermIostreamDisplay::setFocus: type="<<focus<<endl;
    events_->add( new probe::FocusChangedEvent( focus ) );
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
    stringstream ssHeader;
    ssHeader << "Registry : "<<data.locatorString;
    
    stringstream ssOptions;
    // we must use the index of the option list so that the handler understands us
    for ( unsigned int i=0; i<data.platforms.size(); ++i ) 
    {
        // first filter on name
//         std::string adapt = data.homes[i].proxy->ice_getAdapterId();
//         if ( !adapt.compare( 0,filter_.size(), filter_ ) ) 
//         {
            // now filter on reachability
//             if ( data.homes[i].isReachable ) {
                ssOptions << setw(2)<<i<<"\t"<< data.platforms[i].name <<endl;
//             }
//             else {
//                 cout<<"\t"<< adapt <<endl;
//             }
// 
//         }
    }

    topMenu( ssHeader.str(), "a platform", ssOptions.str() );
    
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
            // check
            if ( selection>=0 && selection<(int)data.platforms.size() ) {
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

void 
TermIostreamDisplay::printPlatformData( const orcacm::RegistryHierarchicalData2& data )
{
    stringstream ssHeader;
    ssHeader << "Platform : "<<data.platform.name;
    
    orca::FQComponentName compName;
    stringstream ssOptions;
    // we must use the index of the option so that the handler understands us
    for ( unsigned int i=0; i<data.homes.size(); ++i ) 
    {
        // first filter on name
        compName = orcaice::toComponentName( data.homes[i].proxy );

//         if ( !adapt.compare( 0,filter_.size(), filter_ ) ) 
//         {
            // print number only for the reachable
            if ( data.homes[i].isReachable ) {
                ssOptions << setw(2)<<i<<"\t";
            }
            else {
                ssOptions << "\t";
            }
            ssOptions << orcaice::toString(compName) << endl;
//         }
    }

    if ( data.homes.empty() ) {
        ssOptions << "[ No components were found ]" << endl;
    }

    stdMenu( ssHeader.str(), "a component", ssOptions.str() );
    
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
            unsigned int selection = atoi( userInput.c_str() );
            //cout<<"you picked "<<selection<<endl;
            //check
            if ( selection>=0 && selection<data.homes.size() ) {
                browser_->choosePick( selection );
                break;
            }
            cout<<"invalid numeric selection: '"<<selection<<"'. try again"<<endl;
        }
        else {
            cout<<"invalid selection: '"<<userInput<<"'. try again"<<endl;
        }
    } // while
}

void 
TermIostreamDisplay::printComponentData( const orcacm::ComponentData& data )
{
    stringstream ssHeader;
    ssHeader << "Component : "<<orcaice::toString( data.name );

    stringstream ssOptions;
    for ( unsigned int i=0; i<data.provides.size(); ++i ) 
    {
        // put a number for all interfaces because ice_ping() is always supported.
        // we must use the index of the option so that the handler understands us
        ssOptions << setw(2)<<i;
        
        bool isSupported = false;
        for ( unsigned int j=0; j<supportedInterfaces_.size(); ++j )
        {
            // is this interface in the list of supported interfaces?
            if ( supportedInterfaces_[j] == data.provides[i].id ) {
                isSupported = true;
                break;
            }
        } // supported interfaces

        if ( isSupported ) {
            // if it's supported, print out a secret sign
            ssOptions << " *  ";
        }
        else {
            ssOptions << "    ";
        }
        ssOptions.setf( ios::left, ios::adjustfield );
        ssOptions << setw(32)<<data.provides[i].name<<"\t"<<data.provides[i].id<<endl;
        ssOptions.setf( ios::right, ios::adjustfield );
    } // provided interfaces

    if ( data.provides.empty() ) {
        ssOptions << "[ No interfaces were found ]" << endl;
    }

    stdMenu( ssHeader.str(), "an interface", ssOptions.str() );
    getUserInput( data.provides.size() );
}

void 
TermIostreamDisplay::printInterfaceData( const orcacm::InterfaceData& data )
{
    stringstream ssHeader;
    ssHeader << "Interface : "<<orcaice::toString( data.name )<<" of type "<<data.id;

    stringstream ssOptions;
    for ( unsigned int i=0; i<data.operations.size(); ++i ) {
        //if ( comps[i].isReachable ) {
            // print number only for the supported 
            if ( data.operations[i].isSupported ) {
                ssOptions << setw(2)<<i<<"\t";
            }
            else {
                ssOptions << "\t";
            }
            ssOptions<<data.operations[i].name<<endl;
        //}
    }

    if ( data.operations.empty() ) {
        ssOptions << "[ No operations were found ]" << endl;
    }

    stdMenu( ssHeader.str(), "an operation", ssOptions.str() );
    getUserInput( data.operations.size() );
}

void 
TermIostreamDisplay::printOperationData( const orcacm::OperationData& data )
{
    stringstream ssHeader;
    ssHeader << "Operation : "<<data.parentId<<"::"<<data.name<<endl;

    stringstream ssOptions;
    for ( unsigned int i=0; i<data.results.size(); ++i ) {
        ssOptions << data.results[i].name<<":"<<endl;
        ssOptions << data.results[i].text<<endl;
    }
    
    stdMenu( ssHeader.str(), "an action", ssOptions.str() );
    getUserInput( data.results.size() );
}

void 
TermIostreamDisplay::getUserInput( unsigned int optionCount )
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
            if ( selection>=0 && selection<optionCount ) {
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
