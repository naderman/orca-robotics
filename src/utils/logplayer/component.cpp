/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>

#include "component.h"
#include "mainloop.h"

using namespace std;
using namespace logplayer;

namespace logplayer {

static const char *DEFAULT_FACTORY_NAME="libOrcaLogFactory.so";

orcalog::ReplayFactory* loadReplayFactory( orcadynamicload::DynamicallyLoadedLibrary &lib )
{
    orcalog::ReplayFactory *f = 
        orcadynamicload::dynamicallyLoadClass<orcalog::ReplayFactory,ReplayFactoryMakerFunc>
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
    Ice::StringSeq libNames = orcaice::toStringSeq( factoryLibNames, ' ' );
    
    for ( unsigned int i=0; i < libNames.size(); i++ )
    {
        stringstream ss;
        ss << "Loading factory library: " << libNames[i];
        context().tracer()->info( ss.str() );
        
        try {
            orcadynamicload::DynamicallyLoadedLibrary *lib = new orcadynamicload::DynamicallyLoadedLibrary(libNames[i]);
            orcalog::ReplayFactory *f = loadReplayFactory( *lib );
            libraries_.push_back(lib);
            replayFactories_.push_back(f);
        }
        catch (orcadynamicload::DynamicLoadException &e)
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
Component::createReplayer( const std::string  &interfaceType, 
                   const std::string  &format,
                   const std::string  &filename )
{
    // debug
//     cout<<"adding slave type="<<interfaceType<<" sfx="<<interfaceTypeSuffix<<" fmt="<<format<<" prfx="<<filenamePrefix<<endl;
    for ( unsigned int i=0; i < replayFactories_.size(); ++i )
    {
        // if this interface is not supported, skip this factory
        if ( !replayFactories_[i]->isSupported( interfaceType ) ) {
            continue;
        }

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

    string libNames = orcaice::getPropertyWithDefault( props, prefix+"FactoryLibNames", DEFAULT_FACTORY_NAME );
    loadPluginLibraries( libNames );

    string masterFilename = orcaice::getPropertyWithDefault( props, prefix+"LogFilename", "master.log");
    master_ = new orcalog::ReplayMaster( masterFilename.c_str(), context() );

    // get info on all logs from the master file
    std::vector<std::string> filenames;
    std::vector<std::string> interfaceTypes;
    std::vector<std::string> formats;
    // this may throw and it will kill us
    master_->getLogs( filenames, interfaceTypes, formats );
    stringstream ss;
    ss << "found " << filenames.size() << " logs in the master file.";
    context().tracer()->info( ss.str() );

    // Instantiate a replayer for each log in the master file
    for ( unsigned int i=0; i<filenames.size(); ++i ) 
    {
        context().tracer()->debug( "Processing log: file="+filenames[i]+" type="+interfaceTypes[i]+" fmt="+formats[i], 5);

        //
        // Create replayer
        // this will throw on error, not catching it so it will kill us.
        //
        createReplayer( interfaceTypes[i], formats[i], filenames[i] );
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
//     context().tracer()->debug("Stopping logplayer component", 5 );
    orcaice::Thread::stopAndJoin( mainLoop_ );
}

} // namespace
