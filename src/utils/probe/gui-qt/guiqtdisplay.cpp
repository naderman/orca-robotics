/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <QApplication>

#include "guiqtdisplay.h"
#include "mainwin.h"
#include "guiqtevents.h"

using namespace std;
using namespace probe;

GuiQtDisplay::GuiQtDisplay( const std::vector<std::string> & supportedInterfaces  )
    : supportedInterfaces_(supportedInterfaces),
      browser_(0),
      model_(0),
      gui_(0)
{
//     cout<<"loading qt driver"<<endl;
}

GuiQtDisplay::~GuiQtDisplay()
{
}

void 
GuiQtDisplay::enable( orcaprobe::IBrowser* browser )
{
    browser_ = browser;
    assert( browser_ || "pointer to browser must be non-zero" );
    if ( !browser_ ) {
        cout<<"null browser pointer. exitting..."<<endl;
        exit(1);
    }
    
    // Set up QT stuff
//     cout<<"starting qt stuff"<<endl;
    char **v = 0;
    int c = 0;
    QApplication qapp(c,v);

    // create it only after the Qt app is started
    model_ = new orcaqcm::OcmModel;

    gui_ = new MainWindow( browser_, model_, supportedInterfaces_ );
    gui_->show();

    //
    // Send initial event to the browser after the GUI is running
    //
    browser_->chooseActivate();
    qapp.exec();
}

void 
GuiQtDisplay::showNetworkActivity( bool isActive )
{
    // make sure that GUI has already started
    if ( gui_ ) {
//         cout<<"posting net event"<<endl;
        qApp->postEvent( gui_, (QEvent*)new QorcaNetworkActivityChangedEvent( isActive ) );
    }
}

void 
GuiQtDisplay::setRegistryData( const orcacm::RegistryHierarchicalData1 & data )
{
    if ( gui_ ) {
    //     cout<<"posting reg event"<<endl;
        qApp->postEvent( gui_, (QEvent*)new QorcaRegistryDataChangedEvent( data ) );
    }
}

void 
GuiQtDisplay::setPlatformData( const orcacm::RegistryHierarchicalData2 & data )
{
    if ( gui_ )  {
        qApp->postEvent( gui_, (QEvent*)new QorcaPlatformDataChangedEvent( data ) );
    }
}

void 
GuiQtDisplay::setComponentData( const orcacm::ComponentData & data )
{
    if ( gui_ ) {
        qApp->postEvent( gui_, (QEvent*)new QorcaComponentDataChangedEvent( data ) );
    }
}

void 
GuiQtDisplay::setInterfaceData( const orcacm::InterfaceData & data )
{
    if ( gui_ ) {
        qApp->postEvent( gui_, (QEvent*)new QorcaInterfaceDataChangedEvent( data ) );
    }
}

void 
GuiQtDisplay::setOperationData( const orcacm::OperationData & data )
{
    if ( gui_ ) {
        qApp->postEvent( gui_, (QEvent*)new QorcaOperationDataChangedEvent( data ) );
    }
}

void 
GuiQtDisplay::setFocus( orcaprobe::IDisplay::FocusType focus )
{
    if ( gui_ ) {
        qApp->postEvent( gui_, (QEvent*)new QorcaFocusChangedEvent( focus ) );
    }
}
