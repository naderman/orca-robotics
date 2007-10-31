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
#include <assert.h>

#include <QtGui>

#include <orcaice/orcaice.h>
#include <orcaqt/orcaqt.h>
#include <orcaqcm/orcaqcm.h>

#include "mainwin.h"
#include "guiqtevents.h"
#include "probeview.h"
#include "probedelegate.h"

using namespace std;
using namespace probe;

MainWindow::MainWindow( orcaprobe::IBrowser *browser,
                        orcaqcm::OcmModel        *model,
                        const std::vector<std::string> & supportedInterfaces,
                        QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags),
      browser_(browser),
      model_(model),
      supportedInterfaces_(supportedInterfaces)
{
    assert(browser_ || "null pointer to browser");
    assert(model_ || "null pointer to model");

    setWindowTitle("Orca: Probe");
    setWindowIcon ( QPixmap(orcaqt::orca2_2x3_yellow_130_xpm) );
    QFont f;
    f.setPointSize( 7 );
    setFont( f );
    resize( qApp->desktop()->availableGeometry(0).width(), 400 );

    pathLabel_ = new QLabel;
    QLabel *movLabel = new QLabel;
    movie_ = new QMovie(this);
    movie_->setCacheMode(QMovie::CacheAll);
    movie_->setFileName("/opt/orca2/images/working.mng");
    movLabel->setMovie(movie_);
    
    QHBoxLayout *labelLayout = new QHBoxLayout;
    labelLayout->addWidget(pathLabel_, 0, Qt::AlignLeft );
    labelLayout->addWidget(movLabel, 0, Qt::AlignRight );

    QSplitter* split = new QSplitter();
    split->setOrientation(Qt::Horizontal);
    split->setMinimumWidth( 400 );

    ProbeView::config( browser_, pathLabel_, statusBar() );

    regView_    = new ProbeView( "Regisry", model_, split );
    platfView_  = new ProbeView( "Platform", model_, split );
    compView_   = new ProbeView( "Component", model_, split );
    ifaceView_  = new ProbeView( "Interface", model_, split );
    operView_   = new ProbeView( "Operation", model_, split );

    split->setStretchFactor( 0, 1 );
    split->setStretchFactor( 1, 1 );
    split->setStretchFactor( 2, 1 );
    split->setStretchFactor( 3, 1 );
    split->setStretchFactor( 4, 2 );

    regView_->setNeighbors( 0, platfView_ );
    platfView_->setNeighbors( regView_, compView_ );
    compView_->setNeighbors( platfView_, ifaceView_ );
    ifaceView_->setNeighbors( compView_, operView_ );
    operView_->setNeighbors( ifaceView_, 0 );

    QWidget *centralWidget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout( labelLayout, 0 );
    layout->addWidget( split, 1 );

    centralWidget->setLayout( layout );
    setCentralWidget( centralWidget );

    setupMenuBar();

    statusBar()->showMessage( "Initialized", 2000 );
}

void
MainWindow::setupMenuBar()
{
    QMenu *fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&Quit", this, SLOT(quit()), Qt::CTRL | Qt::Key_Q );

    QMenu *toolsMenu = menuBar()->addMenu("&Tools");
    toolsMenu->addAction("Reload &Current", this, SLOT(reload()), Qt::Key_F5 );
    toolsMenu->addAction("Reload &Registry", this, SLOT(top()), Qt::SHIFT | Qt::Key_F5 );
    
    QMenu* helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("&About Probe", this, SLOT(aboutApp()), Qt::Key_F1 );
    helpMenu->addAction("About &Orca", this, SLOT(aboutOrca()), Qt::SHIFT | Qt::Key_F1 );

}

void
MainWindow::reload()
{
    browser_->chooseReload();
}

void
MainWindow::top()
{
    browser_->chooseTop();
}

void 
MainWindow::keyPressEvent ( QKeyEvent * event )
{
    switch (event->key()) 
    {
    case Qt::Key_Enter : {
        if ( regView_->hasFocus() ) {
            browser_->choosePick( 0 );

            compView_->setFocus( Qt::OtherFocusReason );
        }
        else {
        }
        break; 
    }
    } //  switch

    QMainWindow::keyPressEvent(event);
}

void
MainWindow::customEvent( QEvent* e )
{
    switch ( e->type() )
    {
    case NetworkActivityChanged :
    {
        QorcaNetworkActivityChangedEvent* de = dynamic_cast<QorcaNetworkActivityChangedEvent*>(e);
        assert( de && "event should be a NetworkActivityChangedEvent" );

        if ( de->isActive_ ) {
//             cout<<"starting movie"<<endl;
            movie_->start();
        }
        else {
//             cout<<"stopping movie"<<endl;
//             movie_->stop();
            QTimer::singleShot( 500, movie_, SLOT(stop()) );
        }
        break;
    }
    case FocusChanged :
    {
//         cout<<"FocusChangedEvent: "<<endl;
        QorcaFocusChangedEvent* de = dynamic_cast<QorcaFocusChangedEvent*>(e);
        assert( de && "event should be a FocusChangedEvent" );
        cout<<"FocusChangedEvent: "<<(int)de->focus_<<endl;

        switch ( de->focus_ )
        {
        case orcaprobe::IDisplay::RegistryFocus :
            regView_->setFocus( Qt::OtherFocusReason );
            break;
        case orcaprobe::IDisplay::PlatformFocus :
            platfView_->setFocus( Qt::OtherFocusReason );
            break;
        case orcaprobe::IDisplay::ComponentFocus :
            compView_->setFocus( Qt::OtherFocusReason );
            break;
        case orcaprobe::IDisplay::InterfaceFocus :
            ifaceView_->setFocus( Qt::OtherFocusReason );
            break;
        case orcaprobe::IDisplay::OperationFocus :
            operView_->setFocus( Qt::OtherFocusReason );
            break;
        default :
            break;
        } // focus switch
        break;
    }
    case RegistryDataChanged :
    {
        cout<<"RegistryDataChangedEvent"<<endl;
        QorcaRegistryDataChangedEvent* de = dynamic_cast<QorcaRegistryDataChangedEvent*>(e);
        assert( de && "event should be a RegistryDataChangedEvent" );

        // clear everything, otherwise, have to check for nodes which are no
        // longer in the registry.
        model_->clear();

        model_->setRegistry( 
                QString::fromStdString( de->data_.locatorString ),
                QString::fromStdString( de->data_.address ),
                true );

        for ( unsigned int i=0; i<de->data_.platforms.size(); ++i ) {
//             cout<<"processing platform "<<de->data_.platforms[i].name<<endl;
            model_->setPlatform(
                QString::fromStdString( de->data_.locatorString ),
                QString::fromStdString( de->data_.platforms[i].name ) );
        }

        // set default view, it will be always followed by SetFocus event
        regView_->putIntoFocus();
        break;
    }
    case PlatformDataChanged :
    {
        cout<<"PlatformDataChangedEvent"<<endl;
        QorcaPlatformDataChangedEvent* de = dynamic_cast<QorcaPlatformDataChangedEvent*>(e);
        assert( de && "event should be a PlatformDataChangedEvent" );

        model_->setRegistry( 
                QString::fromStdString( de->data_.locatorString ),
                QString::fromStdString( de->data_.address ),
                true );

        std::string adapt;
        orca::FQComponentName compName;
        for ( unsigned int i=0; i<de->data_.homes.size(); ++i ) {
            // local call
            adapt = de->data_.homes[i].proxy->ice_getAdapterId();
            compName = orcaice::toComponentName( adapt );
            model_->setComponent(
                QString::fromStdString( de->data_.locatorString ),
                QString::fromStdString( compName.platform ),
                QString::fromStdString( compName.component ),
                QString::fromStdString( de->data_.homes[i].address ),
                de->data_.homes[i].isReachable,
                99 );
        }

//         cout<<model_->rowCount( regView_->currentIndex() )<<" children"<<endl;
        platfView_->putIntoFocus();
        break;
    }
    case ComponentDataChanged :
    {
        cout<<"QorcaComponentEvent"<<endl;
        QorcaComponentDataChangedEvent* de = dynamic_cast<QorcaComponentDataChangedEvent*>(e);
        assert( de && "event should be a ComponentDataChangedEvent" );

        model_->setRegistry( 
                QString::fromStdString( de->data_.locatorString ),
                QString::fromStdString( de->data_.address ),
                true );

        model_->setComponent(
            QString::fromStdString( de->data_.locatorString ),
            QString::fromStdString( de->data_.name.platform ),
            QString::fromStdString( de->data_.name.component ),
            QString::fromStdString( de->data_.address ),
            de->data_.isReachable,
            de->data_.timeUp );

        for ( unsigned int i=0; i<de->data_.provides.size(); ++i ) {
//             cout<<"processing interface "<<de->data_.provides[i].name<<endl;

            // check if the interface is supported     
            bool isSupported = false; 
            std::vector<std::string>::iterator res;      
            res = std::find( supportedInterfaces_.begin(), supportedInterfaces_.end(), 
                             de->data_.provides[i].id );
            if ( res != supportedInterfaces_.end() ) {
                isSupported = true;
            }

            model_->setInterface(
                QString::fromStdString( de->data_.locatorString ),
                QString::fromStdString( de->data_.name.platform ),
                QString::fromStdString( de->data_.name.component ),
                QString::fromStdString( de->data_.provides[i].name ),
                true,
                QString::fromStdString( de->data_.provides[i].id ),
                isSupported );
        }

//         cout<<model_->rowCount( platfView_->currentIndex() )<<" children"<<endl;

        compView_->putIntoFocus();
        break;
    }
    case InterfaceDataChanged :
    {
        cout<<"QorcaInterfaceEvent"<<endl;
        QorcaInterfaceDataChangedEvent* de = dynamic_cast<QorcaInterfaceDataChangedEvent*>(e);
        assert( de && "event should be a InterfaceDataChangedEvent" );

        for ( unsigned int i=0; i<de->data_.operations.size(); ++i ) {
//             cout<<"processing operation "<<de->data_.operations[i].name<<endl;
            model_->setOperation(
                QString::fromStdString( de->data_.locatorString ),
                QString::fromStdString( de->data_.name.platform ),
                QString::fromStdString( de->data_.name.component ),
                QString::fromStdString( de->data_.name.iface ),
                QString::fromStdString( de->data_.operations[i].name  ),
                QString::fromStdString( de->data_.operations[i].signature ) );
        }

//         cout<<model_->rowCount( compView_->currentIndex() )<<" children"<<endl;

        ifaceView_->putIntoFocus();
        break;
    }
    case OperationDataChanged :
    {
//         cout<<"QorcaOperationEvent"<<endl;
        QorcaOperationDataChangedEvent* de = dynamic_cast<QorcaOperationDataChangedEvent*>(e);
        assert( de && "event should be a OperationDataChangedEvent" );

        for ( unsigned int i=0; i<de->data_.results.size(); ++i ) {
            model_->setResult(
                QString::fromStdString( de->data_.locatorString ),
                QString::fromStdString( de->data_.parent.platform ),
                QString::fromStdString( de->data_.parent.component ),
                QString::fromStdString( de->data_.parent.iface ),
                QString::fromStdString( de->data_.name ),
                QString::fromStdString( de->data_.results[i].name ),
                QString::fromStdString( de->data_.results[i].text ) );

//             cout<<"MainWindow: "<<de->data_.results[i].name<<":"<<endl;
//             cout<<de->data_.results[i].text<<endl;
        }

//         cout<<model_->rowCount( ifaceView_->currentIndex() )<<" children"<<endl;

        operView_->putIntoFocus();
        break;
    }
    default :
        break;
    }
}

void
MainWindow::quit()
{
    close();
}

void
MainWindow::aboutOrca()
{
    orcaqt::aboutOrca( this );
}

void
MainWindow::aboutApp()
{
    QString text;
    text += "Lets you browse the remote interfaces of an\n";
    text += "Orca system based on the information from\n";
    text += "the IceGrid Registry.\n";
    text += "Supported interfaces:\n";
    for ( unsigned int i=0; i<supportedInterfaces_.size(); ++i ) {
        text += "    ";
        text += QString::fromStdString( supportedInterfaces_[i] );
        text += "\n";
    }
    orcaqt::aboutApp( this, "About Probe", text );
}
