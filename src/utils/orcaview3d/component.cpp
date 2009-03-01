/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <QApplication>
 
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include <hydrodll/dynamicload.h>
#include <hydroiceutil/jobqueue.h>
#include <orcaqgui/mainwin.h>
#include <orcaqgui/selectableelementwidget.h>
#include <orcaqgui/configfileelements.h>
#include <orcaqguielementmodelview/guielementmodel.h>
#include <hydroqgui/platformcsfinder.h>
#include <orcaqgui/iguielementfactory.h>
#include "component.h"
#include "worldview.h"
        
using namespace std;

namespace orcaview3d {

namespace {

    static const char *DEFAULT_FACTORY_LIB_NAME="libOrcaQGui3dFactory.so";

    hydroqgui::IGuiElementFactory* loadFactory( hydrodll::DynamicallyLoadedLibrary &lib )
    {
        hydroqgui::IGuiElementFactory *f = 
            hydrodll::dynamicallyLoadClass<hydroqgui::IGuiElementFactory,FactoryMakerFunc>
            (lib, "createFactory");
        return f;
    }

}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

Component::Component()
    : orcaice::Component( "OrcaView3d", orcaice::NoStandardInterfaces )
{
}

Component::~Component()
{
    assert( libraries_.size() == factories_.size() );
    for ( unsigned int i=0; i < libraries_.size(); i++ ){
        delete factories_[i];
        delete libraries_[i];
    }
}       

std::vector<std::string>
Component::loadPluginLibraries( const std::string& factoryLibNames )
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
            hydroqgui::IGuiElementFactory *f = loadFactory( *lib );
            libraries_.push_back(lib);
            factories_.push_back(f);

            ifaces = f->supportedElementTypesAsStdString();
            for ( unsigned int j=0; j<ifaces.size(); ++j ) {
                supportedInterfaces.push_back( ifaces[j] );
            }    
        }
        catch (hydrodll::DynamicLoadException &e)
        {
            cout << "ERROR(orcaview3d:component.cpp): " << e.what() << endl;
            throw;
        }
    }

    for ( size_t i=0; i < factories_.size(); i++ )
    {
        cout<<"TRACE(orcaview3d:component.cpp): Setting context for "<<i<<"'th factory" << endl;

        orcaqgui::IGuiElementFactory *orcaFactory = 
                dynamic_cast<orcaqgui::IGuiElementFactory*>(factories_[i]);
        if ( orcaFactory != NULL )
        {
            cout<<"TRACE(orcaview3d:component.cpp): setContext()" << endl;
            orcaFactory->setContext( context() );
        }
        else
        {
            stringstream ss;
            ss << "Factory is not an orca factory!  (couldn't cast to orcaqgui::IGuiElementFactory*)";
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }
    }

    if ( factories_.empty() ) {
        std::string err = "No gui element factories were loaded.";
        context().tracer().error( err );
        throw err;
    }

    // eliminate duplicates from the listing of supported interfaces
    std::sort( supportedInterfaces.begin(), supportedInterfaces.end() );
    std::unique( supportedInterfaces.begin(), supportedInterfaces.end() );

    return supportedInterfaces;
}

void
readScreenDumpParams( const orcaice::Context                 &context,
                      orcaqgui::MainWindow::ScreenDumpParams &screenDumpParams )
{
    Ice::PropertiesPtr prop = context.properties();
    std::string prefix = context.tag() + ".Config.";

    screenDumpParams.topPad = orcaice::getPropertyAsIntWithDefault( prop, prefix+"ScreenCapture.TopPad", 25 );
    screenDumpParams.sidePad = orcaice::getPropertyAsIntWithDefault( prop, prefix+"ScreenCapture.SidePad", 2 );
    screenDumpParams.bottomPad = orcaice::getPropertyAsIntWithDefault( prop, prefix+"ScreenCapture.BottomPad", 2 );
    Ice::StringSeq strIn; strIn.push_back("/tmp"); Ice::StringSeq strOut;
    strOut = orcaobj::getPropertyAsStringSeqWithDefault( prop, prefix+"General.DumpPath", strIn );
    screenDumpParams.dumpPath = strOut[0];
    screenDumpParams.captureTimerInterval = orcaice::getPropertyAsIntWithDefault( prop, prefix+"ScreenCapture.CaptureTimerInterval", 1000 );
}

void 
Component::start()
{
    Ice::PropertiesPtr props = context().properties();
    std::string prefix = context().tag() + ".Config.";
    
    //
    // enable network connections
    //
    // Home interface only
    // this may throw, but may as well quit right then
    activate();
    
    //
    // Start job queue
    //
    hydroiceutil::JobQueue::Config jconfig;
    jconfig.threadPoolSize = orcaice::getPropertyAsIntWithDefault( props, prefix+"JobQueueThreadPoolSize", 1 );
    jconfig.queueSizeWarn = orcaice::getPropertyAsIntWithDefault( props, prefix+"JobQueueSizeWarning", -1 );
    jconfig.traceAddEvents = false;
    jconfig.traceDoneEvents = false;
    
    hydroiceutil::JobQueue jobQueue( context().tracer(), jconfig ) ;
    
    // Set up QT stuff
    char **v = 0;
    int c = 0;
    QApplication qapp(c,v);

    orcaqgui::MainWindow::ScreenDumpParams screenDumpParams;
    readScreenDumpParams( context(), screenDumpParams );
    
    int displayRefreshTime = orcaice::getPropertyAsIntWithDefault( props, prefix+"General.DisplayRefreshTime", 200 );

    string libNames = orcaice::getPropertyWithDefault( props, prefix+"General.FactoryLibNames", DEFAULT_FACTORY_LIB_NAME );
    // returns a listing of unique supported interfaces, for display drivers to know what's supported
    std::vector<std::string> supportedInterfaces = loadPluginLibraries( libNames );

    // Manages platform(s) in focus
    hydroqgui::PlatformFocusManager platformFocusManager;

    // main window for display
    orcaqgui::MainWindow mainWin( "OrcaView3d",
                                  screenDumpParams,
                                  supportedInterfaces );

    // Color scheme
    hydroqgui::StringToRandomColorMap platformColorScheme;

    // Handles coordination of mouse events between GuiElements
    hydroqguielementutil::MouseEventManager mouseEventManager;

    // Manages the coordinate frome for display
    hydroqgui::CoordinateFrameManager coordinateFrameManager;

    // Manages use of shortcut keys between Gui Elements
    hydroqguielementutil::ShortcutKeyManager shortcutKeyManager( &mainWin );

    // Stores the set of Gui Elements
    hydroqgui::GuiElementSet guiElementSet;

    // Qt model for handling elements and their display in each of the widgets
    orcaqgemv::GuiElementModel guiElementModel( factories_,
                                                mainWin,
                                                mouseEventManager,
                                                shortcutKeyManager,
                                                coordinateFrameManager,
                                                platformFocusManager,
                                                guiElementSet,
                                                platformColorScheme );

    // Can work out the coordinate system of a platform
    orcaqgui3d::PlatformCSFinder platformCSFinder;

    // widget for viewing the actual world
    WorldView *worldView = new WorldView( platformCSFinder,
                                          mouseEventManager,
                                          guiElementSet,
                                          coordinateFrameManager,
                                          mainWin,
                                          platformFocusManager,
                                          displayRefreshTime );
    
    // Gui-Element-selecting widget
    orcaqgui::SelectableElementWidget *selectableElementWidget = 
            new orcaqgui::SelectableElementWidget(  platformFocusManager, 
                                                    jobQueue, 
                                                    context(), 
                                                   &guiElementModel, 
                                                    mainWin );
    
    // Central GUI widget
    QSplitter *centralWidget = new QSplitter( Qt::Horizontal );
    centralWidget->addWidget( selectableElementWidget );
    centralWidget->addWidget( worldView );
    mainWin.setCentralWidget( centralWidget );
    
    // Grid is a special element which is always loaded
    orcaqgui::loadGrid( guiElementModel );

    // Load all elements specified in the config file
    orcaqgui::loadElementsFromConfigFile( guiElementModel, context() );

    mainWin.show();

    // note: this does not return!
    qapp.exec();

    // normally ctrl-c handler does this, now we have to because UserThread keeps the thread
    context().communicator()->shutdown();

    // the rest is handled by the application/service
}

void Component::stop()
{
    context().tracer().debug("stopping component",5);
}

}
