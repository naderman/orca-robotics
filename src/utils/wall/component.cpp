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
#include <algorithm>

#include <orcaice/orcaice.h>
#include <orcawall/orcawall.h>
#include <hydrodll/dynamicload.h>
#include "component.h"

using namespace std;

namespace wall
{

static const char *DEFAULT_FACTORY_NAME="libOrcaWallFactory.so";

orcawall::Factory* loadFactory( hydrodll::DynamicallyLoadedLibrary &lib )
{
    orcawall::Factory *f = 
        hydrodll::dynamicallyLoadClass<orcawall::Factory,FactoryMakerFunc>
                                        (lib, "createFactory");
    return f;
}

Component::Component()
    : orcaice::Component( "Wall" )
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
    vector<string> libNames = hydroutil::toStringSeq( factoryLibNames, ' ' );
    
    // this will be a listing of unique supported interfaces
    std::vector<std::string> supportedInterfaces;
    std::vector<std::string> ifaces;
    
    for ( unsigned int i=0; i < libNames.size(); i++ )
    {
        stringstream ss;
        ss << "Loading factory library: " << libNames[i];
        context().tracer().info( ss.str() );
        
        try {
            hydrodll::DynamicallyLoadedLibrary *lib = new hydrodll::DynamicallyLoadedLibrary(libNames[i]);
            orcawall::Factory *f = loadFactory( *lib );
            libraries_.push_back(lib);
            factories_.push_back(f);
    
            ifaces = f->supportedTypes();
            for ( unsigned int j=0; j<ifaces.size(); ++j ) {
                supportedInterfaces.push_back( ifaces[j] );
            }                        
        }
        catch (hydrodll::DynamicLoadException &e)
        {
            cout << "ERROR(loggercomponent.cpp): " << e.what() << endl;
            throw;
        }
    }

    if ( factories_.empty() ) {
        std::string err = "No factories were loaded.";
        context().tracer().error( err );
        throw err;
    }

    // eliminate duplicates from the listing of supported interfaces
    std::sort( supportedInterfaces.begin(), supportedInterfaces.end() );
    std::unique( supportedInterfaces.begin(), supportedInterfaces.end() );

    return supportedInterfaces;
}

void
Component::createPlugin( const std::string& interfaceType, const std::string& tag )
{
    // debug
//     cout<<"adding slave type="<<interfaceType<<" sfx="<<interfaceTypeSuffix<<" fmt="<<format<<" prfx="<<filenamePrefix<<endl;

    for ( unsigned int i=0; i < factories_.size(); ++i )
    {
        // if this interface is not supported, skip this factory
        if ( !factories_[i]->isSupported( interfaceType ) ) {
            continue;
        }

//         context().tracer().debug( "creating logger type="+interfaceType+" sfx="+interfaceTypeSuffix+" fmt="+format+" prfx="+filenamePrefix, 5);
        orcawall::InterfaceSim* sim = factories_[i]->create( interfaceType, tag, context() );

        if ( sim ) { 
            sims_.push_back( (hydroiceutil::Thread*)sim );
            return;
        }
        else {
            std::string err = "Error when creating iface sim for supported interface type " + interfaceType;
            context().tracer().error( err );
            throw err;
        }
    }

    // none of the factories support this type
    std::string err = "Unsupported interface type " + interfaceType;
    context().tracer().error( err );
    throw err;
}

void 
Component::start()
{
    context().tracer().debug( "Starting component", 2 );
    //
    // enable network connections
    //
    activate();

    Ice::PropertiesPtr props = properties();
    std::string prefix = tag()+".Config.";

    string libNames = orcaice::getPropertyWithDefault( props, prefix+"FactoryLibNames", DEFAULT_FACTORY_NAME );
    // returns a listing of unique supported interfaces, for display drivers to know what's supported
    std::vector<std::string> supportedInterfaces = loadPluginLibraries( libNames );
    {
        stringstream ss;
        ss<<"The following "<<supportedInterfaces.size()<<" interfaces are supported:";
        for ( unsigned int j=0; j<supportedInterfaces.size(); ++j ) {
            ss << "\n\t" << supportedInterfaces[j];
        }
        context().tracer().info( ss.str() );
    }

    std::vector<std::string> providedTags; 
    providedTags = orcaice::getProvidedTags( context() );
    {
        stringstream ss; ss<<"Found "<<providedTags.size()<<" tags for simulated provided interfaces";
        context().tracer().info( ss.str() );
    }

    for ( unsigned int i=0; i<providedTags.size(); ++i ) {
        std::string type, suffix;
        orcawall::parseInterfaceTag( providedTags[i], type, suffix );
        context().tracer().debug("Processing interface with tag="+providedTags[i]+": type="+type+" sfx="+suffix,3);
        
        createPlugin( type, providedTags[i] );
    }
    {
        stringstream ss; ss<<"Created "<<sims_.size()<<" simulated interfaces";
        context().tracer().debug( ss.str(),3 );
    }

    //start the interface simulators
    for ( unsigned int i=0; i<sims_.size(); ++i ) {
        stringstream ss; ss<<"Starting interface simulator "<<i;
        context().tracer().debug( ss.str(), 3 );
        sims_[i]->start();
    }

    context().tracer().debug( "Component started", 2 );
    // the rest is handled by the application/service
}

void 
Component::stop()
{
    context().tracer().debug( "Stopping component", 2 );

    for ( unsigned int i=0; i<sims_.size(); ++i ) {
        stringstream ss; ss<<"Stopping interface simulator "<<i;
        context().tracer().debug( ss.str(), 3 );
        hydroiceutil::stopAndJoin( sims_[i] );
    }
    context().tracer().debug( "Component stopped", 2 );
}

} // namespace
