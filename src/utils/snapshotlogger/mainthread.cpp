/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2007 Alexei Makarenko, Alex Brooks
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

namespace snapshotlogger {

namespace {

static const char *DEFAULT_FACTORY_LIB_NAME="libOrcaLogFactory.so";

orcalog::SnapshotLoggerFactory* loadLogFactory( hydrodll::DynamicallyLoadedLibrary &lib )
{
    orcalog::SnapshotLoggerFactory *f = 
        hydrodll::dynamicallyLoadClass<orcalog::SnapshotLoggerFactory,SnapshotLoggerFactoryMakerFunc>
                                              (lib, "createSnapshotLoggerFactory");
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
MainThread::initialise()
{
    // multi-try activation function
    orcaice::activate( context_, this, subsysName() );
    // check for stop signal after retuning from multi-try
    if ( isStopping() )
        return;

    initLoggers();
    initInterface();
}

void
MainThread::work()
{
    subStatus().setMaxHeartbeatInterval( 5.0 );

    while ( !isStopping() )
    {
        try {
            bool dummy;
            const int TIMEOUT_MS = 1000;
            int ret = requestStore_.getNext( dummy, TIMEOUT_MS );
            if ( ret == 0 )
            {
                takeSnapshot();
            
                // Clear any requests that might have arrived while we were taking the snapshot
                if ( !requestStore_.isEmpty() )
                    requestStore_.get( dummy );
            }
            subStatus().ok();
        }
        catch ( ... ) 
        {
            orcaice::catchMainLoopExceptions( subStatus() );
        }
    }
}

///////////////////

void
MainThread::initLoggers()
{
    // config file parameters
    Ice::PropertiesPtr props = context_.properties();
    const std::string prefix = context_.tag() + ".Config.";

    const double timeWindowSec = orcaice::getPropertyAsDoubleWithDefault( props,
                                                                          prefix+"TimeWindowSec",
                                                                          30.0 );
    useFilenameTimestamps_ = (bool)orcaice::getPropertyAsIntWithDefault( props, prefix+"FilenameTimestamp", 1 );

    string libNames = orcaice::getPropertyWithDefault( props,
                                                       prefix+"FactoryLibNames",
                                                       DEFAULT_FACTORY_LIB_NAME );
    loadPluginLibraries( libNames );

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
        
        orcalog::LogWriterInfo logWriterInfo( context_ );
        logWriterInfo.interfaceType = interfaceType;
        logWriterInfo.interfaceTag  = interfaceType+interfaceTypeSuffix;
        std::string proxyString     = orcaice::getRequiredInterfaceAsString( context_, 
                                                                             logWriterInfo.interfaceTag );
        logWriterInfo.comment       = orcaice::resolveLocalPlatform( context_, proxyString );
        logWriterInfo.format        = format;
        logWriterInfo.filename      = logWriterInfo.interfaceTag + ".log";

        //
        // Create logger
        // this will throw on error, not catching it so it will kill us.
        //
        orcalog::SnapshotLogger *logger = createLogger( interfaceType );

        try {
            logger->init( format, timeWindowSec );
        }
        catch ( std::exception &e )
        {
            // fatal error!
            context_.shutdown();
            throw;
        }
        logWriterInfos_.push_back( logWriterInfo );
        snapshotLoggers_.push_back( logger );
    }

    // initialize and subscribe all interfaces  
    for ( size_t i=0; i < snapshotLoggers_.size(); i++ )
        snapshotLoggers_[i]->subscribe( context_, logWriterInfos_[i].interfaceTag );
}

void
MainThread::initInterface()
{
    while ( !isStopping() )
    {
        try {
            buttonInterface_ = new orcaifaceimpl::ButtonImpl( "Button", context_ );
            buttonInterface_->setNotifyHandler( this );
            buttonInterface_->initInterface();
            context_.tracer().debug( "Activated button interface" );
            break;
        }
        catch ( ... ) {
            orcaice::catchExceptionsWithStatusAndSleep( "initialising Button interface", subStatus() );
        }
    }
}

std::string
MainThread::filenamePrefix()
{
    string filenamePrefix = "";
    if ( useFilenameTimestamps_ ) {
        filenamePrefix = orcalog::humanReadableTimeStamp() + "_";
    }    
    return filenamePrefix;
}

orcalog::MasterFileWriter *
MainThread::createMasterFileWriter( const std::string &filenamePrefix )
{
    string baseMasterFilename = "master.log";
    string masterFilename = filenamePrefix + baseMasterFilename;

    try {
        return new orcalog::MasterFileWriter( masterFilename.c_str(), context_ );
    }
    catch ( std::exception &e )
    {
        stringstream ss;
        ss << "Couldn't create masterFileWriter: " << e.what();
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
}

namespace {

    bool
    atLeastOneNonEmpty( const std::vector<orcalog::SnapshotLogger*> &loggers )
    {
        for ( size_t i=0; i < loggers.size(); i++ )
        {
            if ( loggers[i]->snapshotBufferSize() > 0 )
                return true;
        }
        return false;
    }

    orcalog::SnapshotLogger*
    loggerWithOldestObject( std::vector<orcalog::SnapshotLogger*> &loggers )
    {
        orca::Time tOldest;
        orcalog::SnapshotLogger *loggerWithOldest = NULL;

        // Find the age of the oldest item in the first non-empty buffer
        size_t i=0;
        for ( ; i < loggers.size(); i++ )
        {
            if ( loggers[i]->snapshotBufferSize() > 0 )
            {
                tOldest = loggers[i]->oldestArrivalTime();
                loggerWithOldest = loggers[i];
                break;
            }
        }
    
        // Compare it with the oldest items in the other non-empty buffers
        i = i+1;
        for ( ; i < loggers.size(); i++ )
        {
            if ( loggers[i]->snapshotBufferSize() > 0 )
            {
                double timeDiff = orcaice::timeDiffAsDouble( loggers[i]->oldestArrivalTime(),
                                                             tOldest );
                if ( timeDiff < 0 )
                {
                    tOldest = loggers[i]->oldestArrivalTime();
                    loggerWithOldest = loggers[i];
                }
            }
        }

        assert( loggerWithOldest != NULL );
        return loggerWithOldest;
    }

    void
    orchestrateSaving( std::vector<orcalog::SnapshotLogger*> &loggers )
    {
        while ( atLeastOneNonEmpty( loggers ) )
        {
            loggerWithOldestObject(loggers)->writeOldestObjToLogAndDiscard();
        }
    }

}

void
MainThread::takeSnapshot()
{
    cout<<"TRACE(mainthread.cpp): "<<__func__<< endl;

    if ( snapshotLoggers_.size() == 0 ) return;

    std::string fnamePrefix = filenamePrefix();

    // Create Master File
    std::auto_ptr<orcalog::MasterFileWriter> masterFileWriter( createMasterFileWriter( fnamePrefix ) );

    stringstream exceptionSS;
    try {

        //
        // Prepare for snapshot
        //
        for ( size_t i=0; i < snapshotLoggers_.size(); i++ )
        {
            // Prepend the timestamp to the filename
            orcalog::LogWriterInfo logWriterInfo = logWriterInfos_[i];
            logWriterInfo.filename = fnamePrefix + logWriterInfos_[i].filename;

            snapshotLoggers_[i]->prepareForSnapshot( logWriterInfo,
                                                     *masterFileWriter );
        }

        //
        // orchestrate saving the snapshot
        //
        orchestrateSaving( snapshotLoggers_ );
    }
    catch ( std::exception &e )
    {
        // This will get thrown after cleanup
        exceptionSS << e.what();
    }

    //
    // Finalise snapshot
    //
    for ( size_t i=0; i < snapshotLoggers_.size(); i++ )
        snapshotLoggers_[i]->finaliseSnapshot();

    if ( !exceptionSS.str().empty() )
    {
        throw gbxutilacfr::Exception( ERROR_INFO, exceptionSS.str() );
    }
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
            orcalog::SnapshotLoggerFactory *f = loadLogFactory( *lib );
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
        throw gbxutilacfr::Exception( ERROR_INFO, "No log factories were loaded." );
    }
}

orcalog::SnapshotLogger *
MainThread::createLogger( const std::string &interfaceType )
{
    for ( unsigned int i=0; i < logFactories_.size(); ++i )
    {
        // if this interface is not supported, skip this factory
        if ( !logFactories_[i]->isSupported( interfaceType ) ) {
            continue;
        }

        orcalog::SnapshotLogger* logger = logFactories_[i]->create( interfaceType );

        if ( logger ) { 
            return logger;
        }
        else {
            context_.shutdown();
            stringstream ss;
            ss << "Error when creating logger for supported interface type " << interfaceType << ": factory returned NULL pointer.";
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }
    }

    // none of the factories support this type
    context_.shutdown();
    throw gbxutilacfr::Exception( ERROR_INFO, "Unsupported interface type " + interfaceType );
}

}
