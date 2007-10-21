/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>
#include <hydrodll/dynamicload.h>

#include "component.h"
#include "mainloop.h"

using namespace std;
using namespace logplayer;

namespace logplayer {

static const char *DEFAULT_FACTORY_NAME="libOrcaLogFactory.so";

orcalog::ReplayFactory* loadReplayFactory( hydrodll::DynamicallyLoadedLibrary &lib )
{
    orcalog::ReplayFactory *f = 
        hydrodll::dynamicallyLoadClass<orcalog::ReplayFactory,ReplayFactoryMakerFunc>
                                        (lib, "createReplayFactory");
    return f;
}

Component::Component( const std::string & compName )
    : orcaice::Component( compName ),
      mainLoop_(0),
      master_(0)
{
}

Component::~Component()
{
    delete master_;
    // important: do not delete replayers because most of them derive from 
    // Ice smart pointers and self-destruct. Deleting them here will result
    // in seg fault.

    assert( libraries_.size() == replayFactories_.size() );
    for ( unsigned int i=0; i < libraries_.size(); i++ ){
        delete replayFactories_[i];
        delete libraries_[i];
    }
}

void
Component::loadPluginLibraries( const std::string & factoryLibNames )
{
    // Parse space-separated list of lib names
    vector<string> libNames = hydroutil::toStringSeq( factoryLibNames, ' ' );
    
    for ( unsigned int i=0; i < libNames.size(); i++ )
    {
        stringstream ss;
        ss << "Loading factory library: " << libNames[i];
        context().tracer()->info( ss.str() );
        
        try {
            hydrodll::DynamicallyLoadedLibrary *lib = new hydrodll::DynamicallyLoadedLibrary(libNames[i]);
            orcalog::ReplayFactory *f = loadReplayFactory( *lib );
            libraries_.push_back(lib);
            replayFactories_.push_back(f);
        }
        catch (hydrodll::DynamicLoadException &e)
        {
            cout << "ERROR(loggercomponent.cpp): " << e.what() << endl;
            throw;
        }
    }

    if ( replayFactories_.empty() ) {
        std::string err = "No replayer factories were loaded.";
        context().tracer()->error( err );
        throw err;
    }
}

void
Component::createReplayer( const std::string& interfaceType, 
                   const std::string& format,
                   const std::string& filename,
                   bool enabled,
                   bool require )
{
    // this replayer was disabled by the user in the config file
    if ( !enabled ) {
        replayers_.push_back( new orcalog::DummyReplayer(context()) );
        std::string s = "Interface replay is disabled (type="+interfaceType+" file="+filename
                        +". Created dummy log player";
        context().tracer()->info( s );
        return;
    }

    // debug
//     cout<<"adding slave type="<<interfaceType<<" sfx="<<interfaceTypeSuffix<<" fmt="<<format<<" prfx="<<filenamePrefix<<endl;
    for ( unsigned int i=0; i < replayFactories_.size(); ++i )
    {
        // if this interface is not supported, skip this factory
        if ( !replayFactories_[i]->isSupported( interfaceType ) ) {
            continue;
        }

        // this interface is supported, but it may throw if this particulaer format is not
        try
        {
            orcalog::Replayer* s = replayFactories_[i]->create( 
                    interfaceType, 
                    format,
                    filename,
                    context() );
            if ( s ) { 
                replayers_.push_back( s );
                std::string s( "Created log player: " );
                s += " type=" + interfaceType;
                s += " file=" + filename;
                s += " fmt=" + format;
                context().tracer()->info( s );
                return;
            }
            else {
                std::string err = "Error when creating replayer for supported interface type " + interfaceType;
                context().tracer()->error( err );
                throw err;
            }
        }
        catch ( const orcalog::FormatNotSupportedException& e )
        {
            // all interfaces are required, no dummies
            if ( require ) {
                std::string err = "Format "+format+" is not supported for interface type"+interfaceType;
                context().tracer()->error( err );
                throw err;
            }
            else {
                replayers_.push_back( new orcalog::DummyReplayer(context()) );
                std::string s = "Log format is not supported (type="+interfaceType+" fmt="+format+" file="+filename
                                +". Created dummy log player";
                context().tracer()->info( s );
                return;
            }
        }


    }

    // none of the factories support this type
    // all interfaces are required, no dummies
    if ( require ) {
        std::string err = "Unsupported interface type " + interfaceType;
        context().tracer()->error( err );
        throw err;
    }
    else {
        replayers_.push_back( new orcalog::DummyReplayer(context()) );
        std::string s = "Interface type is not supported (type="+interfaceType+" fmt="+format+" file="+filename
                        +". Created dummy log player";
        context().tracer()->info( s );
        return;
    }
}

void
Component::start()
{
    // config file parameters
    Ice::PropertiesPtr props = properties();
    std::string prefix = tag() + ".Config.";

    string libNames = orcaice::getPropertyWithDefault( props, prefix+"FactoryLibNames", DEFAULT_FACTORY_NAME );
    loadPluginLibraries( libNames );

    string masterFilename = orcaice::getPropertyWithDefault( props, prefix+"LogFilename", "master.log" );
    master_ = new orcalog::ReplayMaster( masterFilename.c_str(), context() );

    bool requireAll = orcaice::getPropertyAsIntWithDefault( props, prefix+"RequireAll", 1 );

    // get info on all logs from the master file
    std::vector<std::string> filenames;
    std::vector<std::string> interfaceTypes;
    std::vector<std::string> formats;
    std::vector<bool> enableds;
    // this may throw and it will kill us
    master_->getLogs( filenames, interfaceTypes, formats, enableds );
    stringstream ss;
    ss << "found " << filenames.size() << " logs in the master file.";
    context().tracer()->info( ss.str() );

    // Instantiate a replayer for each log in the master file
    for ( unsigned int i=0; i<filenames.size(); ++i ) 
    {   
        stringstream ss;
        ss<<"Processing log: file="<<filenames[i]<<" type="+interfaceTypes[i]
          <<" fmt="<<formats[i]<<" on="<<(int)enableds[i];
        context().tracer()->debug( ss.str(), 3);

        //
        // Create replayer
        // this will throw on error, not catching it so it will kill us.
        //
        createReplayer( interfaceTypes[i], formats[i], filenames[i], enableds[i], requireAll );
    }

    // this will launch its own thread and replay the logs
    mainLoop_ = new MainLoop( master_, replayers_, replayFactories_, context() );

    // now we can safely activate
    activate(); 

    // replay the data now
    mainLoop_->start();
    
    // the rest is handled by the application/service
}

void
Component::stop()
{
    context().tracer()->debug("Stopping component", 2 );
    hydroutil::stopAndJoin( mainLoop_ );
}

} // namespace
