/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/application.h>
#include <orcaice/component.h>
#include <orcaice/orcaice.h>

#include "privateutils.h"

using namespace std;

namespace orcaice {

// argc/argv here are unnecessary.
Application::Application( orcaice::Component &component, bool installCtrlCHandler )
    : IceApplication(installCtrlCHandler),
      component_(component)
{
}

Application::~Application()
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
    // convert to string sequence for convinience
    Ice::StringSeq args = Ice::argsToStringSeq( argc, argv );

    // parse command line arguments for flags which require immediate action
    for ( unsigned int i=0; i<args.size(); ++i ) {
        if ( !args[i].compare( 0,2, "-h" ) ||
             !args[i].compare( 0,6, "--help" ) )
        {
            orcaice::initTracerPrint( component_.help() );
            // nothing to clean up yet
            exit(0);
        }
        else if ( !args[i].compare( 0,2, "-v" ) ||
                  !args[i].compare( 0,9, "--version" ) )
        {
            // print out Ice and libOrcaIce versions.
            orcaice::printVersion();
            // nothing to clean up yet
            exit(0);
        }
    }

    // print version information on the first line
    orcaice::printVersion();

    Ice::InitializationData initData;
    // Note that we don't use the version which takes argumets so that the config file which may be
    // specified by mistake with --Ice.Config is not loaded.
    initData.properties = Ice::createProperties();

    // pre-parse Orca-specific command line arguments
    // (nothing here right now)
    // orcaice::parseOrcaCommandLineOptions( args );

    // Level 4. Highest priority, apply properties from the command line arguments
    // read in all command line options starting with '--", but not "-"
    // note that something like --bullshit will be parsed to --bullshit=1
    // Note that this is a standard Ice function.
    initData.properties->parseCommandLineOptions( "", args );
    initTracerPrint( component_.tag()+": Set command line properties" );
            // debug
//             initTracerPrint("after parseCommandLineOptions()");
//             orcaice::printComponentProperties( initData.properties, component_.tag() );

    // Level 3. Now, apply properties from this component's config file (do not force!)
    try
    {
        std::string filename = orcaice::getApplicationConfigFilename( args );

        if ( filename.empty() ) {
            initTracerPrint( component_.tag()+": "+warnMissingProperty("component properties file","Orca.Ice") );
        }
        else {
            initTracerPrint( component_.tag()+": Will try to load component properties from "+filename );

            orcaice::setComponentProperties( initData.properties, filename );
        }
    }
    catch ( const orcaice::Exception &e )
    {
        initTracerWarning( component_.tag()+": Failed to open component configuration file:"+e.what() );
            // debug
//         initTracerPrint( component_.tag()+": Application quitting. Orca out." );
//         exit(1);
    }
            // debug
//             initTracerPrint("after setComponentProperties()");
//             orcaice::printComponentProperties( initData.properties, component_.tag() );

    // Level 2. Now, apply properties from the global Orca config file
    try
    {
        std::string filename = orcaice::getGlobalConfigFilename( args );
        initTracerPrint( component_.tag()+": Will try to load global properties from "+filename );

        orcaice::setGlobalProperties( initData.properties, filename );
    }
    catch ( const orcaice::Exception &e )
    {
        initTracerWarning( component_.tag()+": Failed to open global configuration file: "+e.what() );
    }
            // debug
//             initTracerPrint("after setGlobalProperties()");
//             orcaice::printComponentProperties( initData.properties, component_.tag() );

    // Level 1. apply Orca factory defaults
    orcaice::setFactoryProperties( initData.properties, component_.tag() );
    initTracerPrint( component_.tag()+": Set factory properties." );
            // debug
//             initTracerPrint("after setFactoryProperties()");
//             orcaice::printComponentProperties( initData.properties, component_.tag() );

    // now pass the startup options to Ice which will start the Communicator
    return IceApplication::main( argc, argv, initData );
}

int
Application::run( int argc, char* argv[] )
{
    // now communicator exists. we can further parse properties, make sure all the info is
    // there and set some properties (notably AdapterID)
    orca::FQComponentName fqCompName =
                    orcaice::parseComponentProperties( communicator(), component_.tag() );

    // print all prop's now, after some stuff was added, e.g. Tag.AdapterId
    // note: is it possible that some of the prop's got stripped off by Ice::Application::main()? I don't think so.
    if ( communicator()->getProperties()->getPropertyAsInt( "Orca.PrintProperties" ) ) {
        orcaice::printComponentProperties( communicator()->getProperties(), component_.tag() );
    }

    // create the one-and-only component adapter
    adapter_ = communicator()->createObjectAdapter(component_.tag());
    initTracerPrint( component_.tag()+": Object adapter created" );

    //
    // Give the component all the stuff it needs to initialize
    // Cannot change inything in Component::context_ after this step.
    //
    bool isApp = true;
    component_.init( fqCompName, isApp, adapter_ );
    initTracerPrint( component_.tag()+": Application initialized" );

    //
    // Start the component, catching OrcaIce exceptions.
    // Other exception types will get caught by the Ice::Application.
    //
    try
    {
        component_.start();
        // for the components which hug the thread, eg. Qt-based, this will be printed at shutdown
        // this is optional because after the comp is started we can't assume that dumping to cout 
        // will produce the desired result (e.g. if ncurses are used)
        if ( communicator()->getProperties()->getPropertyAsInt( "Orca.PrintComponentStarted" ) ) {
            initTracerPrint( component_.tag()+": Component started" );
        }
    }
    catch ( const orcaice::Exception & e )
    {
        initTracerError( component_.tag()+": Caught OrcaIce exception: "+e.what() );

        // NOTE: in principle, we should try to stop the component first
        //       but, if the exception comes from the constructor, have to make sure we don't try to stop
        //      the threads which were not started yet. Tried in SegwayrRmp and didn't work.
//         initTracerError( component_.tag()+": Unexpected exception from component. Stopping component..." );
//         component_.stop();
//         initTracerPrint( component_.tag()+": Component stopped" );
        
        initTracerPrint( component_.tag()+": Application quitting. Orca out." );
        return 1;
    }

    // component started without a problem. now will wait for Ctrl-C from user or comm
    communicator()->waitForShutdown();
    
    initTracerPrint( component_.tag()+": Communicator is destroyed. Stopping component" );
    component_.stop();
    initTracerPrint( component_.tag()+": Component stopped" );

    adapter_->waitForDeactivate();
    initTracerPrint( component_.tag()+": Adapter deactivated" );

    initTracerPrint( component_.tag()+": Application quitting. Orca out." );

    return 0;
}

} // namespace
