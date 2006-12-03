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

orcaprobe::Factory* loadFactory( orcaprobe::DynamicallyLoadedLibrary &lib )
{
    orcaprobe::Factory *f = 
        orcaprobe::dynamicallyLoadClass<orcaprobe::Factory,FactoryMakerFunc>
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
    for ( uint i=0; i < libraries_.size(); i++ ){
//         cout<<"deleting factory #"<<i<<endl;
        delete factories_[i];
//         cout<<"deleting library #"<<i<<endl;
        delete libraries_[i];
    }
}

void
Component::loadPluginLibraries( const std::string & factoryLibNames )
{
    // Parse space-separated list of lib names
    Ice::StringSeq libNames = orcaice::toStringSeq( factoryLibNames, ' ' );
    
    for ( uint i=0; i < libNames.size(); i++ )
    {
        stringstream ss;
        ss << "Loading factory library: " << libNames[i];
        context().tracer()->info( ss.str() );
        
        try {
            orcaprobe::DynamicallyLoadedLibrary *lib = new orcaprobe::DynamicallyLoadedLibrary(libNames[i]);
            orcaprobe::Factory *f = loadFactory( *lib );
            libraries_.push_back(lib);
            factories_.push_back(f);
        }
        catch (orcaprobe::DynamicLoadException &e)
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
    loadPluginLibraries( libNames );

    // which driver to load?
    std::string driverName = orcaice::getPropertyWithDefault( props, prefix+"Driver", "iostream" );

    // generic interface to the user interface
    orcaprobe::BrowserDriver* browserDriver = 0;
    // generic interface to the display interface
    orcaprobe::DisplayDriver* displayDriver = 0;
    
    if ( driverName == "qt" ) 
    {
#ifdef HAVE_QT_DRIVER        
        tracer()->info( "Loading Qt driver");
        displayDriver = new DisplayQtDriver( &factories_ );
#else
        throw orcaice::Exception( ERROR_INFO, "Can't instantiate driver type 'qt' because it was not compiled." );
#endif
    }
    else if ( driverName == "iostream" ) 
    {
        tracer()->info( "Loading iostream driver");
        displayDriver = new IostreamDriver( &factories_ );
    }
    else {
        std::string errorStr = "Unknown driver type." + driverName + " Cannot talk to hardware.";
        tracer()->error( errorStr);
        throw orcaice::HardwareException( ERROR_INFO, errorStr );
    }

    BrowserHandler browserHandler( *displayDriver, factories_, context() );
    browserDriver = &browserHandler;
    browserHandler.start();

    // for Qt driver, this will not return
    displayDriver->enable( browserDriver );

    // normally ctrl-c handler does this, now we have to because MainBubble keeps the thread 
    context().communicator()->shutdown();

    // the rest is handled by the application/service
}

void 
Component::stop()
{
    // this component cannot quit nicely
}

} // namespace
