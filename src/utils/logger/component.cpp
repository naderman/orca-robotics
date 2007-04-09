/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>
#include "component.h"

using namespace std;

namespace logger {

static const char *DEFAULT_FACTORY_LIB_NAME="libOrcaLogFactory.so";

orcalog::LogFactory* loadLogFactory( orcadynamicload::DynamicallyLoadedLibrary &lib )
{
    orcalog::LogFactory *f = 
        orcadynamicload::dynamicallyLoadClass<orcalog::LogFactory,LogFactoryMakerFunc>
                                              (lib, "createLogFactory");
    return f;
}

Component::Component( const std::string & compName ) : 
    orcaice::Component( compName ),
    master_(0)
{
}

Component::~Component()
{
    delete master_;
    // important: do not delete loggers because most of them derive from 
    // Ice smart pointers and self-destruct. Deleting them here will result
    // in seg fault.

    assert( libraries_.size() == logFactories_.size() );
    for ( unsigned int i=0; i < libraries_.size(); i++ ){
//         cout<<"deleting factory #"<<i<<endl;
        delete logFactories_[i];
//         cout<<"deleting library #"<<i<<endl;
        delete libraries_[i];
    }
}

void
Component::loadPluginLibraries( const std::string & factoryLibNames )
{
    // Parse space-separated list of lib names
    Ice::StringSeq libNames = orcaice::toStringSeq( factoryLibNames, ' ' );
    
    for ( unsigned int i=0; i < libNames.size(); i++ )
    {
        stringstream ss;
        ss << "Loading factory library: " << libNames[i];
        context().tracer()->info( ss.str() );
        
        try {
            orcadynamicload::DynamicallyLoadedLibrary *lib = new orcadynamicload::DynamicallyLoadedLibrary(libNames[i]);
            orcalog::LogFactory *f = loadLogFactory( *lib );
            libraries_.push_back(lib);
            logFactories_.push_back(f);
        }
        catch (orcadynamicload::DynamicLoadException &e)
        {
            cout << "ERROR(loggercomponent.cpp): " << e.what() << endl;
            throw;
        }
    }

    if ( logFactories_.empty() ) {
        std::string err = "No log factories were loaded.";
        context().tracer()->error( err );
        throw err;
    }
}

void
Component::createLogger( const std::string  &interfaceType, 
                   const std::string  &interfaceTypeSuffix,
                   const std::string  &format,
                   const std::string  &filenamePrefix )
{
    // debug
//     cout<<"adding slave type="<<interfaceType<<" sfx="<<interfaceTypeSuffix<<" fmt="<<format<<" prfx="<<filenamePrefix<<endl;

    for ( unsigned int i=0; i < logFactories_.size(); ++i )
    {
        // if this interface is not supported, skip this factory
        if ( !logFactories_[i]->isSupported( interfaceType ) ) {
            continue;
        }

//         context().tracer()->debug( "creating logger type="+interfaceType+" sfx="+interfaceTypeSuffix+" fmt="+format+" prfx="+filenamePrefix, 5);
        orcalog::Logger* logger = logFactories_[i]->create( 
                interfaceType, 
                interfaceTypeSuffix,
                format,
                master_,
                filenamePrefix,
                context() );

        if ( logger ) { 
            return;
        }
        else {
            std::string err = "Error when creating logger for supported interface type " + interfaceType;
            context().tracer()->error( err );
            throw err;
        }
    }

    // none of the factories support this type
    std::string err = "Unsupported interface type " + interfaceType;
    context().tracer()->error( err );
    throw err;
}

void 
Component::start()
{
    // config file parameters
    Ice::PropertiesPtr props = properties();
    std::string prefix = tag() + ".Config.";

    string libNames = orcaice::getPropertyWithDefault( props, prefix+"FactoryLibNames", DEFAULT_FACTORY_LIB_NAME );
    loadPluginLibraries( libNames );

//     string baseMasterFilename = orcaice::getPropertyWithDefault( props, prefix+"MasterFilename", "master.log" );
    string baseMasterFilename = "master.log";
    bool filenameTimestamp = (bool)orcaice::getPropertyAsIntWithDefault( props, prefix+"FilenameTimestamp", 1 );
    
    string filenamePrefix = "";
    if ( filenameTimestamp ) {
        // get current human-readable timestamp and add to filename
        // (in front, sorted so that files appear in timely order when ls'd)
        string dateTime =  IceUtil::Time::now().toDateTime();
        string month = dateTime.substr(0,2);
        string day = dateTime.substr(3,2);
        string year = dateTime.substr(6,2);
        string hour = dateTime.substr(9,2);
        string minutes = dateTime.substr(12,2);
        string seconds = dateTime.substr(15,2);
        filenamePrefix = year + month + day + "_" + hour + minutes + seconds + "_";
    }
    string masterFilename = filenamePrefix + baseMasterFilename;

    // create master file
    master_ = new orcalog::LogMaster( masterFilename.c_str(), context() );
    
    // activate component, this may throw and it will kill us
    activate();

    // Get a list of required tags
    //
    // e.g. Tag.Requires.Laser0.Proxy, Tag.Requires.Laser1.Proxy, etc.
    //
    // note: interface tags can NOT be arbitrary. this will only work if a Odometry2d
    //       interface uses a tag Odometry2dXX.
    //
    std::vector<std::string> requiredTags = orcaice::getRequiredTags( context() );
    stringstream ss;
    ss << "found " << requiredTags.size() << " required interfaces in the config file.";
    context().tracer()->info( ss.str() );

    // Instantiate a logger for each required interface
    for ( unsigned int i=0; i<requiredTags.size(); ++i ) 
    {
        context().tracer()->debug( "processing required interface tag="+requiredTags[i], 5);

        // parse interface tag into type and suffix
        std::string interfaceType;
        std::string interfaceTypeSuffix;
        orcalog::parseRequiredTag( requiredTags[i], interfaceType, interfaceTypeSuffix );

        // look up format
        std::string format = orcaice::getPropertyWithDefault( props, prefix+requiredTags[i]+".Format", "ice" );

        //
        // Create logger
        // this will throw on error, not catching it so it will kill us.
        //
        createLogger( interfaceType, interfaceTypeSuffix, format, filenamePrefix );
    }

    // doesn't make sense to continue if no loggers were created
    if ( master_->loggerCount() == 0 )
    {
        context().tracer()->warning("No loggers were created. Quitting.");
        context().communicator()->shutdown();
    }


    // initialize and subscribe all interfaces 
//     if ( autoStart ) {
        master_->start();    
//     }
}

void 
Component::stop()
{
//     context().tracer()->debug("Stopping logger component", 5 );
    // there are no threads to stop, we are done
}

}
