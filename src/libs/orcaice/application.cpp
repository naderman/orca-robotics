/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaice/application.h>
#include <orcaice/component.h>
#include <orcaice/orcaice.h>

#include "detail/privateutils.h"

using namespace std;

namespace orcaice {

namespace {

/*
    Order priority for different sources of configuration parameters, from lowest to highest:
        1. orca factory defaults
        2. global config file
        3. component config file
        4. command line arguments
*/
void setProperties( Ice::PropertiesPtr   &properties,
                    const Ice::StringSeq &commandLineArgs,
                    const std::string    &componentTag )
{
    // pre-parse Orca-specific command line arguments
    // (nothing here right now)
    // orcaice::parseOrcaCommandLineOptions( args );

    // Level 4. Highest priority, apply properties from the command line arguments
    // read in all command line options starting with '--", but not "-"
    // note that something like --bullshit will be parsed to --bullshit=1
    // Note that this is a standard Ice function.
    properties->parseCommandLineOptions( "", commandLineArgs );
    initTracerInfo( componentTag+": Loaded command line properties" );

    // Level 3. Now, apply properties from this component's config file (do not force!)
    detail::addPropertiesFromApplicationConfigFile( properties, commandLineArgs, componentTag );

    // Level 2. Now, apply properties from the global Orca config file
    detail::addPropertiesFromGlobalConfigFile( properties, componentTag );

    // Level 1. apply Orca factory defaults
    orcaice::detail::setFactoryProperties( properties, componentTag );
    initTracerInfo( componentTag+": Loaded factory default properties" );
}

}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

// argc/argv here are unnecessary.
Application::Application( orcaice::Component &component, bool installCtrlCHandler )
    : IceApplication(installCtrlCHandler),
      component_(component)
{
}

/*
    Order priority for different sources of configuration parameters, from lowest to highest:
        1. orca factory defaults
        2. global config file
        3. component config file
        4. command line arguments
*/
int
Application::main(int argc, char* argv[])
{
    // convert to string sequence for convenience
    Ice::StringSeq args = Ice::argsToStringSeq( argc, argv );

    // parse command line arguments for flags which require immediate action
    for ( unsigned int i=0; i<args.size(); ++i ) {
        if ( !args[i].compare( 0,2, "-h" ) ||
             !args[i].compare( 0,6, "--help" ) )
        {
            orcaice::initTracerInfo( component_.help( hydroutil::basename(args[0]) ) );
            // nothing to clean up yet
            exit(0);
        }
        else if ( !args[i].compare( 0,2, "-v" ) ||
                  !args[i].compare( 0,9, "--version" ) )
        {
            orcaice::detail::printAllVersions( component_ );
            // nothing to clean up yet
            exit(0);
        }
    }

    // print version information on the first line
    orcaice::detail::printAllVersions( component_ );

    Ice::InitializationData initData;
    // Note that we don't use the version which takes arguments so that the config file which may be
    // specified by mistake with --Ice.Config is not loaded.
    initData.properties = Ice::createProperties();

    // Set the component's properties based on the various sources from which properties can be read
    setProperties( initData.properties, args, component_.context().tag() );

    // now pass the startup options to Ice which will start the Communicator
    return IceApplication::main( argc, argv, initData );
}

int
Application::run( int argc, char* argv[] )
{
    // now communicator exists. we can further parse properties, make sure all the info is
    // there and set some properties (notably AdapterID)
    orca::FQComponentName fqCompName =
                    orcaice::detail::parseComponentProperties( communicator(), component_.context().tag() );

    // print all prop's now, after some stuff was added, e.g. Tag.AdapterId
    // note: is it possible that some of the prop's got stripped off by Ice::Application::main()? I don't think so.
    if ( communicator()->getProperties()->getPropertyAsInt( "Orca.PrintProperties" ) ) {
        orcaice::detail::printComponentProperties( communicator()->getProperties(), component_.context().tag() );
    }

    // create the one-and-only component adapter
    adapter_ = communicator()->createObjectAdapter(component_.context().tag());
    initTracerInfo( component_.context().tag()+": Created object adapter" );

    //
    // Give the component all the stuff it needs to initialize
    // Cannot change anything in Component::context_ after this step.
    //
    bool isApp = true;
    component_.init( fqCompName, isApp, adapter_ );
    initTracerInfo( component_.context().tag()+": Application initialized" );

    //
    // Start the component, catching all exceptions
    //
    stringstream exceptionSS;
    try
    {
        component_.start();
        // for the components which hug the thread, eg. Qt-based, this will be printed at shutdown
        // this is optional because after the comp is started we can't assume that dumping to cout 
        // will produce the desired result (e.g. if ncurses are used)
        if ( communicator()->getProperties()->getPropertyAsInt( "Orca.PrintComponentStarted" ) ) {
            initTracerInfo( component_.context().tag()+": Component started" );
        }
    }
    catch ( const Ice::Exception &e ) {
        exceptionSS << "Caught exception while starting component : " << e;
    }
    catch ( const std::exception &e ) {
        exceptionSS << "Caught exception while starting component: " << e.what();
    }
    catch ( const std::string &e ) {
        exceptionSS << "Caught exception while starting component: " << e;
    }
    catch ( const char *e ) {
        exceptionSS << "Caught exception while starting component: " << e;
    }
    catch ( ... ) {
        exceptionSS << "Caught exception while starting component.";
    }

    if ( !exceptionSS.str().empty() ) {
        initTracerError( component_.context().tag()+": "+exceptionSS.str() );
        initTracerInfo( component_.context().tag()+": Application quitting. Orca out." );
        return 1;
    }

    // component started without a problem. now will wait for Ctrl-C from user or comm
    communicator()->waitForShutdown();
    
    initTracerInfo( component_.context().tag()+": Communicator is destroyed. Stopping component" );
    component_.stop();
//     initTracerInfo( component_.context().tag()+": Finalising component" );
    component_.finalise();
//     initTracerInfo( component_.context().tag()+": Component stopped" );

    adapter_->waitForDeactivate();
//     initTracerInfo( component_.context().tag()+": Adapter deactivated" );

//     initTracerInfo( component_.context().tag()+": Application done." );
    initTracerInfo( component_.context().tag()+": Orca out." );

    return 0;
}

} // namespace
