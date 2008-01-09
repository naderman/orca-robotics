/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>
#include <hydrodll/dynamicload.h>

#include "mainthread.h"

using namespace std;
using namespace logger;

namespace {

static const char *DEFAULT_FACTORY_LIB_NAME="libOrcaLogFactory.so";

orcalog::AutoLoggerFactory* loadLogFactory( hydrodll::DynamicallyLoadedLibrary &lib )
{
    orcalog::AutoLoggerFactory *f = 
        hydrodll::dynamicallyLoadClass<orcalog::AutoLoggerFactory,AutoLoggerFactoryMakerFunc>
                                              (lib, "createAutoLoggerFactory");
    return f;
}

}

/////////////////////

MainThread::MainThread( const orcaice::Context& context ) :
    SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context)
{
    subStatus().setMaxHeartbeatInterval( 20.0 );
}

MainThread::~MainThread()
{
    // important: do not delete loggers because most of them derive from 
    // Ice smart pointers and self-destruct. Deleting them here will result
    // in seg fault.

    assert( libraries_.size() == logFactories_.size() );
    for ( unsigned int i=0; i < libraries_.size(); i++ )
    {
        delete logFactories_[i];
        delete libraries_[i];
    }
}

void
MainThread::walk()
{
    // config file parameters
    Ice::PropertiesPtr props = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

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
    try {
        masterFileWriter_.reset( new orcalog::MasterFileWriter( masterFilename.c_str(), context_ ) );
    }
    catch ( ... ) {
        context_.shutdown();
        throw;
    }
    

    // multi-try activation function
    orcaice::activate( context_, this, subsysName() );


    // Get a list of required tags
    //
    // e.g. Tag.Requires.Laser0.Proxy, Tag.Requires.Laser1.Proxy, etc.
    //
    // note: interface tags can NOT be arbitrary. this will only work if a Odometry2d
    //       interface uses a tag Odometry2dXX.
    //
    std::vector<std::string> requiredTags = orcaice::getRequiredTags( context_ );
    stringstream ss;
    ss << "found " << requiredTags.size() << " required interfaces in the config file.";
    context_.tracer().info( ss.str() );

    // Instantiate a logger for each required interface
    for ( unsigned int i=0; i<requiredTags.size(); ++i ) 
    {
        context_.tracer().debug( "processing required interface tag="+requiredTags[i], 5);

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
    if ( masterFileWriter_->loggerCount() == 0 )
    {
        context_.tracer().warning("No loggers were created. Quitting.");
        context_.communicator()->shutdown();
    }

    // initialize and subscribe all interfaces  
    for ( uint i=0; i < autoLoggers_.size(); i++ )
    {
        autoLoggers_[i]->initAndStartLogging();
    }
    
    // init subsystem is done and is about to terminate
    subStatus().ok( "Initialized." );
    subStatus().setMaxHeartbeatInterval( -1 );
}

void
MainThread::loadPluginLibraries( const std::string & factoryLibNames )
{
    // Parse space-separated list of lib names
    vector<string> libNames = hydroutil::toStringSeq( factoryLibNames, ' ' );
    
    for ( unsigned int i=0; i < libNames.size(); i++ )
    {
        stringstream ss;
        ss << "Loading factory library: " << libNames[i];
        context_.tracer().info( ss.str() );
        
        try {
            hydrodll::DynamicallyLoadedLibrary *lib = new hydrodll::DynamicallyLoadedLibrary(libNames[i]);
            orcalog::AutoLoggerFactory *f = loadLogFactory( *lib );
            libraries_.push_back(lib);
            logFactories_.push_back(f);
        }
        catch (hydrodll::DynamicLoadException &e)
        {
            context_.shutdown();
            throw;
        }
    }

    if ( logFactories_.empty() ) {
        context_.shutdown();
        throw hydroutil::Exception( ERROR_INFO, "No log factories were loaded." );
    }
}

void
MainThread::createLogger( const std::string  &interfaceType, 
                         const std::string  &interfaceTypeSuffix,
                         const std::string  &format,
                         const std::string  &filenamePrefix )
{
    orcalog::LogWriterInfo logWriterInfo( *masterFileWriter_, context_ );

    logWriterInfo.interfaceType = interfaceType;
    logWriterInfo.interfaceTag = interfaceType+interfaceTypeSuffix;
    std::string proxyString = orcaice::getRequiredInterfaceAsString( context_, logWriterInfo.interfaceTag );
    logWriterInfo.comment = orcaice::resolveLocalPlatform( context_, proxyString );
    logWriterInfo.format = format;
    logWriterInfo.filename = filenamePrefix + logWriterInfo.interfaceTag + ".log";

    for ( unsigned int i=0; i < logFactories_.size(); ++i )
    {
        // if this interface is not supported, skip this factory
        if ( !logFactories_[i]->isSupported( interfaceType ) ) {
            continue;
        }

        orcalog::AutoLogger* logger = logFactories_[i]->create( logWriterInfo );

        if ( logger ) { 
            autoLoggers_.push_back( logger );
            return;
        }
        else {
            context_.shutdown();
            throw hydroutil::Exception( ERROR_INFO, "Error when creating logger for supported interface type " + interfaceType );
        }
    }

    // none of the factories support this type
    context_.shutdown();
    throw hydroutil::Exception( ERROR_INFO, "Unsupported interface type " + interfaceType );
}