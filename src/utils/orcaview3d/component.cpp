/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <QApplication>
 
#include <orcaice/orcaice.h>
#include <orcaqcm/networkhandler.h>
#include <orcaqgui/mainwin.h>
#include <orcaqgui/guielementmodel.h>
#include <orcaqgui3d/worldview.h>
#include <orcaqgui3d/platformcsfinder.h>
#include <orcadynamicload/dynamicload.h>
#include "component.h"
#include <orcaqgui/guielementfactory.h>
        
using namespace std;
using namespace orcaview3d;

static const char *DEFAULT_FACTORY_LIB_NAME="libOrcaQGui3dFactory.so";

orcaqgui::GuiElementFactory* loadFactory( orcadynamicload::DynamicallyLoadedLibrary &lib )
{
    orcaqgui::GuiElementFactory *f = 
        orcadynamicload::dynamicallyLoadClass<orcaqgui::GuiElementFactory,FactoryMakerFunc>
                                          (lib, "createFactory");
    return f;
}

Component::Component( string compName)
    : orcaice::Component( compName, orcaice::HomeInterface )
{
}

Component::~Component()
{
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
            orcaqgui::GuiElementFactory *f = loadFactory( *lib );
            libraries_.push_back(lib);
            factories_.push_back(f);
        }
        catch (orcadynamicload::DynamicLoadException &e)
        {
            cout << "ERROR(orcaview3d:component.cpp): " << e.what() << endl;
            throw;
        }
    }

    if ( factories_.empty() ) {
        std::string err = "No gui element factories were loaded.";
        context().tracer()->error( err );
        throw err;
    }
}

void
readScreenDumpParams( const orcaice::Context &context,
                      orcaqgui::ScreenDumpParams &screenDumpParams )
{
    Ice::PropertiesPtr prop = context.properties();
    std::string prefix = context.tag() + ".Config.";

    screenDumpParams.topPad = orcaice::getPropertyAsIntWithDefault( prop, prefix+"ScreenCapture.TopPad", 25 );
    screenDumpParams.sidePad = orcaice::getPropertyAsIntWithDefault( prop, prefix+"ScreenCapture.SidePad", 2 );
    screenDumpParams.bottomPad = orcaice::getPropertyAsIntWithDefault( prop, prefix+"ScreenCapture.BottomPad", 2 );
    Ice::StringSeq strIn; strIn.push_back("/tmp"); Ice::StringSeq strOut;
    strOut = orcaice::getPropertyAsStringSeqWithDefault( prop, prefix+"General.DumpPath", strIn );
    screenDumpParams.dumpPath = strOut[0];
    screenDumpParams.captureTimerInterval = orcaice::getPropertyAsIntWithDefault( prop, prefix+"ScreenCapture.CaptureTimerInterval", 1000 );
}

void 
Component::start()
{
    //
    // INITIAL CONFIGURATION
    //
    
    //
    // enable network connections
    //
    // Home interface only
    // this may throw, but may as well quit right then
    activate();
    
    // this runs in its own thread
    orcaqcm::NetworkHandler networkHandler( context() );
    networkHandler.start();
    
    // Set up QT stuff
    char **v = 0;
    int c = 0;
    QApplication qapp(c,v);

    orcaqgui::ScreenDumpParams screenDumpParams;
    readScreenDumpParams( context(), screenDumpParams );
    
    Ice::PropertiesPtr props = context().properties();
    std::string prefix = context().tag() + ".Config.";
    int displayRefreshTime = orcaice::getPropertyAsIntWithDefault( props, prefix+"General.DisplayRefreshTime", 200 );

    string libNames = orcaice::getPropertyWithDefault( props, prefix+"General.FactoryLibNames", DEFAULT_FACTORY_LIB_NAME );
    loadPluginLibraries( libNames );
   
    // main window for display
    orcaqgui::MainWindow gui( "OrcaView3d",
                             &networkHandler,
                             screenDumpParams,
                             displayRefreshTime );

    // Qt model for handling elements and their display in each of the widgets
    orcaqgui::GuiElementModel guiElemModel( factories_,
                                           context(),
                                           &gui );

    orcaqgui3d::PlatformCSFinder platformCSFinder;

    // widget for viewing the actual world
    orcaqgui3d::WorldView worldView3d( &platformCSFinder,
                                      &guiElemModel,
                                      gui.displayViewParent(),
                                      &gui );

    // tell the main window about the widgets, model, and factory
    gui.init( &guiElemModel,
              &worldView3d );

    gui.loadElementsFromConfigFile( context() );

    gui.show();

    // note: this does not return!
    qapp.exec();

    // normally ctrl-c handler does this, now we have to because UserHandler keeps the thread
    context().communicator()->shutdown();

    // the rest is handled by the application/service
}

void 
Component::stop()
{
    tracer()->debug("Stopping component",1);
}
