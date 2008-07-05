/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include <orcalog/orcalog.h>
#include <hydrodll/dynamicload.h>

#include "mainthread.h"
#include "replayconductor.h"
#include "continuouscontroller.h"
#include "interactivecontroller.h"

using namespace std;
using namespace logplayer;

namespace {

std::string stringToLower( const std::string &s )
{
    std::string sLow = s;
    for ( unsigned int i=0; i < sLow.size(); i++ )
    {
        sLow[i] = tolower( sLow[i] );
    }
    return sLow;
}


static const char *DEFAULT_FACTORY_NAME="libOrcaLogFactory.so";

orcalog::ReplayerFactory* loadReplayerFactory( hydrodll::DynamicallyLoadedLibrary &lib )
{
    orcalog::ReplayerFactory *f = 
        hydrodll::dynamicallyLoadClass<orcalog::ReplayerFactory,ReplayerFactoryMakerFunc>
                                        (lib, "createReplayerFactory");
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
    // important: do not delete replayers because most of them derive from 
    // Ice smart pointers and self-destruct. Deleting them here will result
    // in seg fault.

    assert( libraries_.size() == replayerFactories_.size() );
    for ( unsigned int i=0; i < libraries_.size(); i++ ){
        delete replayerFactories_[i];
        delete libraries_[i];
    }
}

void
MainThread::walk()
{
    // config file parameters
    Ice::PropertiesPtr props = context_.properties();
    std::string prefix = context_.tag() + ".Config.";


    //
    // Load the factories
    //
    string libNames = orcaice::getPropertyWithDefault( props, prefix+"FactoryLibNames", DEFAULT_FACTORY_NAME );
    loadPluginLibraries( libNames );

    //
    // Load the master-file reader
    //
    string masterFilename = orcaice::getPropertyWithDefault( props, prefix+"LogFilename", "master.log" );
    try {
        masterFileReader_.reset( new orcalog::MasterFileReader( masterFilename.c_str(), context_ ) );
    }
    catch ( ... ) {
        context_.shutdown();
        throw;
    }

    bool requireAll = orcaice::getPropertyAsIntWithDefault( props, prefix+"RequireAll", 1 );

    //
    // Load individual replayers
    //

    // get info on all logs from the master file
    std::vector<std::string> filenames;
    std::vector<std::string> interfaceTypes;
    std::vector<std::string> formats;
    std::vector<bool> enableds;
    try {
        masterFileReader_->getLogs( filenames, interfaceTypes, formats, enableds );
    }
    catch ( ... ) {
        context_.shutdown();
        throw;
    }
    stringstream ss;
    ss << "found " << filenames.size() << " logs in the master file.";
    context_.tracer().info( ss.str() );

    // Instantiate a replayer for each log in the master file
    for ( unsigned int i=0; i<filenames.size(); ++i ) 
    {   
        stringstream ss;
        ss<<"Processing log: file="<<filenames[i]<<" type="+interfaceTypes[i]
          <<" fmt="<<formats[i]<<" on="<<(int)enableds[i];
        context_.tracer().debug( ss.str(), 3);

        //
        // Create replayer
        //
        try {
            createReplayer( interfaceTypes[i], formats[i], filenames[i], enableds[i], requireAll );
        }
        catch ( ... ) {
            context_.shutdown();
            throw;
        }
    }

    //
    // Initialise replayer
    //
    for ( size_t i=0; i < replayers_.size(); i++ )
    {
        replayers_[i]->init();
    }

    //
    // Load classes to orchestrate replaying
    //

    // read replay parameters
    IceUtil::Time beginTime;
    double        replayRate;
    bool          autoStart;
    readReplayParams( beginTime, replayRate, autoStart );

    ReplayConductor *conductor = new ReplayConductor( *masterFileReader_.get(),
                                                      replayers_,
                                                      beginTime,
                                                      replayRate,
                                                      context_ );
    replayConductor_ = conductor;

    // Instantiate highLevelController_
    string replayController = orcaice::getPropertyWithDefault( props, prefix+"ReplayController", "interactive" );
    if ( replayController == "interactive" )
    {
        highLevelController_ = new InteractiveController( *conductor, autoStart, context_ );
    }
    else if ( replayController == "continuous" )
    {
        highLevelController_ = new ContinuousController( *conductor, autoStart, context_ );
    }
    else
    {
        context_.shutdown();
        throw gbxutilacfr::Exception( ERROR_INFO, "Unknown ReplayController: "+replayController );
    }

    //
    // now we can safely activate
    // multi-try activation function
    //
    orcaice::activate( context_, this, subsysName() );


    // Start the thread to replay the logs
    replayConductor_->start();

    // Start the high-level control thread
    highLevelController_->start();
    
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
            orcalog::ReplayerFactory *f = loadReplayerFactory( *lib );
            libraries_.push_back(lib);
            replayerFactories_.push_back(f);
        }
        catch (hydrodll::DynamicLoadException &e)
        {
            context_.shutdown();
            throw;
        }
    }

    if ( replayerFactories_.empty() ) {
        context_.shutdown();
        throw gbxutilacfr::Exception( ERROR_INFO, "No replayer factories were loaded." );
    }
}

void
MainThread::createReplayer( const std::string& interfaceType, 
                   const std::string& format,
                   const std::string& filename,
                   bool enabled,
                   bool require )
{
    if ( !enabled ) 
    {
        // this replayer was disabled by the user in the config file
        replayers_.push_back( new orcalog::DummyReplayer );
        std::string s = "Interface replayer is disabled (type="+interfaceType+" file="+filename
                        +". Created dummy log player";
        context_.tracer().info( s );
        return;
    }

    orcalog::LogReaderInfo logReaderInfo( context_ );
    logReaderInfo.interfaceType = interfaceType;
    stringstream ssName;
    ssName << stringToLower(interfaceType) << interfaceTypeCounter_.nextAvailableId( interfaceType );
    logReaderInfo.interfaceName = ssName.str();
    logReaderInfo.format        = format;
    logReaderInfo.filename      = filename;

    // Search for a factory willing to instantiate this type of replayer
    for ( unsigned int i=0; i < replayerFactories_.size(); ++i )
    {
        // if this interface is not supported, skip this factory
        if ( !replayerFactories_[i]->isSupported( interfaceType ) ) {
            continue;
        }

        // this interface is supported, but it may throw if this particulaer format is not
        try
        {
            orcalog::Replayer* s = replayerFactories_[i]->create( logReaderInfo );
            if ( s ) { 
                replayers_.push_back( s );
                std::string s( "Created log player: " );
                s += " type=" + interfaceType;
                s += " file=" + filename;
                s += " fmt=" + format;
                context_.tracer().info( s );
                return;
            }
            else {
                context_.shutdown();
                throw gbxutilacfr::Exception( ERROR_INFO, "Error when creating replayer for supported interface type " + interfaceType );
            }
        }
        catch ( const orcalog::FormatNotSupportedException& e )
        {
            if ( require ) 
            {
                context_.shutdown();
                throw gbxutilacfr::Exception( ERROR_INFO, "Format "+format+" is not supported for interface type "+interfaceType );
            }
            else {
                replayers_.push_back( new orcalog::DummyReplayer() );
                std::string s = "Log format is not supported (type="+interfaceType+" fmt="+format+" file="+filename
                                +". Created dummy log player";
                context_.tracer().info( s );
                return;
            }
        }


    }

    // none of the factories support this type
    // all interfaces are required, no dummies
    if ( require ) {
        context_.shutdown();
        throw gbxutilacfr::Exception( ERROR_INFO, "Unsupported interface type " + interfaceType );
    }
    else {
        replayers_.push_back( new orcalog::DummyReplayer );
        std::string s = "Interface type is not supported (type="+interfaceType+" fmt="+format+" file="+filename
                        +". Created dummy log player";
        context_.tracer().info( s );
        return;
    }
}

void
MainThread::readReplayParams( IceUtil::Time &beginTime, double &replayRate, bool &autoStart )
{
    // config file parameters
    Ice::PropertiesPtr props = context_.properties();
    string prefix = context_.tag() + ".Config.";

    // BeginTime: Replay data starting from BeginTime (seconds) from the start of the log
    orca::Time tempTime;
    orcaobj::setInit( tempTime );
    tempTime = orcaobj::getPropertyAsTimeDurationWithDefault( props, prefix+"BeginTime", tempTime );
    beginTime = orcaice::toIceTime( tempTime );
    if ( beginTime<IceUtil::Time() ) {
        beginTime = IceUtil::Time();
        context_.tracer().warning( "Negative BeginTime was reset to 0.0" );
    }

    // ReplayRate: Adjusts the playback speed
    replayRate = orcaice::getPropertyAsDoubleWithDefault( props, prefix+"ReplayRate", 1.0 );
    stringstream ss;
    ss<<"Replay is triggered by the clock (real/replay="<<replayRate<<").";
    context_.tracer().info( ss.str() );

    autoStart = orcaice::getPropertyAsIntWithDefault( props, prefix+"AutoStart", 0 );
}
