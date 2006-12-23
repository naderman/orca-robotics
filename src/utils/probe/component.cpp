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
#include <algorithm>

#include <orcaice/orcaice.h>
#include <orcaprobe/orcaprobe.h>
#include "component.h"

#include "browserhandler.h"

#include "iostream/iostreamdriver.h"
#ifdef HAVE_QT_DRIVER   
#include "qt/displayqtdriver.h"
#endif

using namespace std;

namespace probe
{

static const char *DEFAULT_FACTORY_NAME="libOrcaProbeFactory.so";

orcaprobe::Factory* loadFactory( orcadynamicload::DynamicallyLoadedLibrary &lib )
{
    orcaprobe::Factory *f = 
        orcadynamicload::dynamicallyLoadClass<orcaprobe::Factory,FactoryMakerFunc>
                                        (lib, "createFactory");
    return f;
}

Component::Component()
    : orcaice::Component( "Probe", orcaice::NoStandardInterfaces )
{
}

Component::~Component()
{
    assert( libraries_.size() == factories_.size() );
    for ( unsigned int i=0; i < libraries_.size(); i++ ){
//         cout<<"deleting factory #"<<i<<endl;
        delete factories_[i];
//         cout<<"deleting library #"<<i<<endl;
        delete libraries_[i];
    }
}

std::vector<std::string>
Component::loadPluginLibraries( const std::string & factoryLibNames )
{
    // Parse space-separated list of lib names
    Ice::StringSeq libNames = orcaice::toStringSeq( factoryLibNames, ' ' );
    
    // this will be a listing of unique supported interfaces
    std::vector<std::string> supportedInterfaces;
    std::vector<std::string> ifaces;
    
    for ( unsigned int i=0; i < libNames.size(); i++ )
    {
        stringstream ss;
        ss << "Loading factory library: " << libNames[i];
        context().tracer()->info( ss.str() );
        
        try {
            orcadynamicload::DynamicallyLoadedLibrary *lib = new orcadynamicload::DynamicallyLoadedLibrary(libNames[i]);
            orcaprobe::Factory *f = loadFactory( *lib );
            libraries_.push_back(lib);
            factories_.push_back(f);
    
            ifaces = f->supportedTypes();
            for ( unsigned int j=0; j<ifaces.size(); ++j ) {
                supportedInterfaces.push_back( ifaces[j] );
            }                        
        }
        catch (orcadynamicload::DynamicLoadException &e)
        {
            cout << "ERROR(loggercomponent.cpp): " << e.what() << endl;
            throw;
        }
    }

    if ( factories_.empty() ) {
        std::string err = "No log factories were loaded.";
        context().tracer()->error( err );
        throw err;
    }

    // eliminate duplicates from the listing of supported interfaces
    std::sort( supportedInterfaces.begin(), supportedInterfaces.end() );
    std::unique( supportedInterfaces.begin(), supportedInterfaces.end() );

    return supportedInterfaces;
}

void 
Component::start()
{
    //
    // enable network connections
    //
    activate();

    Ice::PropertiesPtr props = properties();
    std::string prefix = tag()+".Config.";

    string libNames = orcaice::getPropertyWithDefault( props, prefix+"FactoryLibNames", DEFAULT_FACTORY_NAME );
    // returns a listing of unique supported interfaces, for display drivers to know what's supported
    std::vector<std::string> supportedInterfaces = loadPluginLibraries( libNames );    

    // which driver to load?
    std::string driverName = orcaice::getPropertyWithDefault( props, prefix+"Driver", "iostream" );

    //
    // EVENT QUEUES
    //
    orcaice::EventQueuePtr netQueue_ = new orcaice::EventQueue();    
    orcaice::EventQueuePtr usrQueue_ = new orcaice::EventQueue();  

    BrowserHandler browserHandler( netQueue_, usrQueue_, factories_, context() );
    browserHandler.start();

    // generic interface to the display interface
    orcaprobe::DisplayDriver* displayDriver = 0;
    
    if ( driverName == "qt" ) 
    {
#ifdef HAVE_QT_DRIVER        
        tracer()->info( "Loading Qt driver");
        displayDriver = new DisplayQtDriver( usrQueue_, netQueue_, supportedInterfaces );
#else
        throw orcaice::Exception( ERROR_INFO, "Can't instantiate driver type 'qt' because it was not compiled." );
#endif
    }
    else if ( driverName == "iostream" ) 
    {
        tracer()->info( "Loading iostream driver");
        displayDriver = new IostreamDriver( usrQueue_, netQueue_, supportedInterfaces );
    }
    else {
        std::string errorStr = "Unknown driver type." + driverName + " Cannot talk to hardware.";
        tracer()->error( errorStr);
        throw orcaice::HardwareException( ERROR_INFO, errorStr );
    }

    // for Qt driver, this will not return
    displayDriver->enable();

    // normally ctrl-c handler does this, now we have to because MainBubble keeps the thread 
    context().communicator()->shutdown();
}

void 
Component::stop()
{
    // this component cannot quit nicely because display drivers are using the main thread
}

} // namespace
