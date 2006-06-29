/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <iostream>
#include <fstream>
#include <sstream>

#include <QApplication>
#include <QPainter>
#include <QPixmap>
#include <QTextStream>
#include <Q3PopupMenu>

#include <qdom.h>
#include <q3mainwindow.h>
#include <qstatusbar.h>
#include <qmenubar.h>
#include <qfile.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qmessagebox.h>
#include <q3filedialog.h>
#include <qdatetime.h>
#include <q3hbox.h>
#include <q3vbox.h>
#include <qcombobox.h>
#include <qlineedit.h>

#include <orcaice/orcaice.h>
#include <orcadef/defparser.h>
#include <orcadef/componentdefinition.h>

#include <qorcauml/qorcacanvas.h>
#include <qorcauml/qorcacomponent.h>
#include <qorcauml/qorcainterface.h>
#include <qorcauml/qorcalink.h>
#include <qorcauml/qorcaicons.h>

#include "mainwin.h"
#include "qorcalibview.h"
#include "qorcaprojview.h"
#include "gorcafile.h"
#include "loadsave.h"

using namespace std;

MainWindow::MainWindow( QWidget* parent, const char* name, Qt::WFlags f )
        : Q3MainWindow(parent,name,f), libDom_(0), projDom_(0)
{
    setWindowTitle("Orca2: G-Orca");
    setWindowIcon ( QPixmap(orca2_2x3_yellow_130_xpm) );

    //QApplication::setStartDragDistance(10);

    Q3HBox* hbox = new Q3HBox( this );
    setCentralWidget( hbox );

    Q3VBox* vbox = new Q3VBox( hbox );

    // library of components: Canvas, Canvas View
    // will resize to size after loading components
    libCanvas_ = new QorcaCanvas();
    libCanvas_->resize(400,1000);
    libView_ = new QorcaLibView( libCanvas_, vbox );
    libView_->setMaximumWidth( 300 );
    QMatrix m = libView_->worldMatrix();
    m.scale( 0.5, 0.5 );
    libView_->setWorldMatrix( m );
    
    // component filters: by name
    nameFilter_ = new QLineEdit( vbox );
    nameFilter_->setText( "*" );
    QObject::connect( nameFilter_,SIGNAL(textChanged(const QString&)), this,SLOT(filterLibrary()) );

    // component filters: by interface name
    Q3HBox* hboxlet = new Q3HBox( vbox );
    interfaceFilter_ = new QComboBox( hboxlet );
    providedRequiredFilter_ = new QComboBox( hboxlet );

    // interface names
    QStringList interfaceNames;
    interfaceNames<<"*"<<"Platform2d"<<"Position2d"<<"Laser";
    interfaceFilter_->insertStringList( interfaceNames );
    QObject::connect( interfaceFilter_,SIGNAL(activated(int)), this,SLOT(filterLibrary()) );

    // roles
    QStringList roles;
    roles<<"*"<<"Provided"<<"Required";
    providedRequiredFilter_->insertStringList( roles );
    QObject::connect( providedRequiredFilter_,SIGNAL(activated(int)), this,SLOT(filterLibrary()) );

    // current project: Canvas, Canvas View
    projCanvas_ = new QorcaCanvas();
    projCanvas_->resize(2000,1000);
    projView_ = new QorcaProjView( projCanvas_, hbox );
    QObject::connect( projView_,SIGNAL(copyLibToProj(const QPoint&)), this,SLOT(copyLibToProj(const QPoint&)) );

    // set spacing and relative stretch factors
    hbox->setSpacing( 5 );
    hbox->setStretchFactor( libView_, 1 );
    hbox->setStretchFactor( projView_, 3 );

    initInterface();
    if ( !initLibrary() ) {
        qApp->quit();
    }
    //newProject();
}

MainWindow::~MainWindow() {}

void MainWindow::initInterface()
{
    // icon location
    //QString iconPath( "/usr/share/icons/qred/" );
    QString iconPath( "/usr/share/icons/crystalsvg/16x16/actions/" );
    QPixmap newProjIcon, openProjIcon, saveProjIcon, closeProjIcon, exitIcon;
    newProjIcon.load( iconPath+"window_new.png" );
    openProjIcon.load( iconPath+"project_open.png" );
    saveProjIcon.load( iconPath+"filesave.png" );
    closeProjIcon.load( iconPath+"fileclose.png");
    exitIcon.load( iconPath+"exit.png");
    QPixmap insertCompIcon, deleteCompIcon;
    insertCompIcon.load( iconPath+"editcopy.png" );
    deleteCompIcon.load( iconPath+"editcut.png" );
    QPixmap reloadIcon, zoominIcon, zoomoutIcon;
    reloadIcon.load( iconPath+"reload.png" );
    zoominIcon.load( iconPath+"viewmag+.png" );
    zoomoutIcon.load( iconPath+"viewmag-.png" );
    QPixmap validateIcon, configureIcon, infoIcon;
    validateIcon.load( iconPath+"ok.png" );
    configureIcon.load( iconPath+"configure.png" );
    infoIcon.load( iconPath+"info.png" );

    QMenuBar* menu = menuBar();

    Q3PopupMenu* fileMenu = new Q3PopupMenu( menu );
    fileMenu->insertItem(exitIcon, "&Quit", qApp, SLOT(quit()), Qt::CTRL|Qt::Key_Q);
    menu->insertItem("&File", fileMenu);

    Q3PopupMenu* viewMenu = new Q3PopupMenu( menu );
    viewMenu->insertItem("Toggle &Library", this, SLOT(toggleLibrary()), Qt::CTRL|Qt::Key_1);
    viewMenu->insertSeparator();
    viewMenu->insertItem(reloadIcon, "&Reload", this, SLOT(reloadView()), Qt::Key_F5);
    viewMenu->insertSeparator();
    viewMenu->insertItem(zoominIcon, "&Zoom in", this, SLOT(zoomIn()), Qt::CTRL|Qt::Key_Equal);
    viewMenu->insertItem(zoomoutIcon, "Zoom &out", this, SLOT(zoomOut()), Qt::CTRL|Qt::Key_Minus);
    viewMenu->insertItem("Zoom &Reset", this, SLOT(zoomReset()), Qt::CTRL|Qt::Key_0);
    viewMenu->insertSeparator();
    viewMenu->insertItem("&Enlarge Project Canvas", this, SLOT(enlarge()));
    viewMenu->insertItem("Shr&ink Project Canvas", this, SLOT(shrink()));
    menu->insertItem("&View", viewMenu);

    Q3PopupMenu* libMenu = new Q3PopupMenu( menu );
    // reload segfaults for some reason
    //libMenu->insertItem(reloadIcon, "&Reload ", this, SLOT(initLibrary()));
    libMenu->insertItem("Show &All ", this, SLOT(showAllLibrary()), Qt::CTRL|Qt::Key_M);
    libMenu->insertSeparator();
    libMenu->insertItem("&Print ", this, SLOT(printLibrary()));
    menu->insertItem("&Library", libMenu);

    Q3PopupMenu* projectMenu = new Q3PopupMenu( menu );
    projectMenu->insertItem(newProjIcon, "&New Project", this, SLOT(newProject()), Qt::CTRL|Qt::Key_N);
    projectMenu->insertItem(openProjIcon, "&Open Project", this, SLOT(openProject()), Qt::CTRL|Qt::Key_O);
    projectMenu->insertItem(saveProjIcon, "&Save Project", this, SLOT(saveProject()), Qt::CTRL|Qt::Key_S);
    projectMenu->insertSeparator();
    projectMenu->insertItem(saveProjIcon, "&Export Config Files", this, SLOT(exportProject()), Qt::CTRL|Qt::Key_E);
    projectMenu->insertSeparator();
    projectMenu->insertItem(closeProjIcon, "C&lose Project", this, SLOT(closeProject()),Qt::CTRL|Qt::Key_W);
    projectMenu->insertSeparator();
    projectMenu->insertItem(infoIcon, "Project &Info", this, SLOT(showProjectInfo()),Qt::CTRL|Qt::Key_I);
    menu->insertItem("&Project", projectMenu);

    Q3PopupMenu* compMenu = new Q3PopupMenu( menu );
    compMenu->insertItem("Select &All", this, SLOT(selectAll()),Qt::CTRL|Qt::Key_A);
    compMenu->insertSeparator();
    compMenu->insertItem(insertCompIcon, "&Insert Selected", this, SLOT(copyLibToProj()),Qt::CTRL|Qt::Key_V);
    compMenu->insertItem(deleteCompIcon, "&Delete Selected", projView_,SLOT(deleteSelected()),Qt::CTRL|Qt::Key_D);
    compMenu->insertSeparator();
    compMenu->insertItem(validateIcon, "&Validate Selected", this, SLOT(validateSelected()), Qt::Key_F9);
    menu->insertItem("&Component", compMenu);

    Q3PopupMenu* toolsMenu = new Q3PopupMenu( menu );
    toolsMenu->insertItem("&Link Ports", projView_, SLOT(toggleLinkTool()));
    menu->insertItem("&Tools", toolsMenu);

    Q3PopupMenu* helpMenu = new Q3PopupMenu( menu );
    helpMenu->insertItem("About &Gorca", this, SLOT(aboutApp()));
    helpMenu->insertItem("About &Orca", this, SLOT(aboutOrca()));
    menu->insertItem("&Help", helpMenu);

    statusBar();

    static int r=24;
    srand(++r);
}

bool MainWindow::initLibrary()
{
    // clear display (it will also clear canvas)
    libView_->clear();
    cout<<"view cleared :"<<libCanvas_->allItems().size()<<endl;
    // clear component storage
    libraryModel_.clear();

    // find component definition files
    QDir d;
    //! @todo fix this
    string installPrefix = INSTALL_PREFIX;
    string installDir    = installPrefix + "/orca/def";
    d.setPath( installDir.c_str() );
    d.setNameFilter( "*.def" );
    d.setFilter( QDir::Files );

    QStringList filelist = d.entryList();
    cout<<"MainWindow::initLibrary: found "<<filelist.size()<<" component definition files"<<endl;
    
    QString filename;

    for ( QStringList::Iterator it=filelist.begin(); it!=filelist.end(); ++it )
    {
        filename = d.path() + "/" + QString( *it );
        cout<<"MainWindow::initLibrary: loading "<<filename.toStdString()<<endl;

        orcadef::ComponentDef def;
        try {
            orcadef::parseDefFile( filename.toStdString(), def );
            cout<<orcadef::toString(def)<<endl;
        }
        catch ( std::string &e )
        {
            cout<<": Error parsing '" << filename.toStdString() << "': " << e << endl;
            exit(1);
        }

        libraryModel_.push_back( new ComponentModel( def ) );
    }

    // resize the library canvas appropriately
    libCanvas_->resize( libCanvas_->width(), (libraryModel_.size()+1)*200 );

    // create components in the canvas
    for ( unsigned int i=0; i<libraryModel_.size(); ++i ) 
    {
        QorcaComponent* q = new QorcaComponent( libCanvas_, *(libraryModel_[i]) );
        q->generateDefaultCfg();
        q->move( 100, 100 + 200*i);
        q->setActive( false );
        q->show();
        // spread the ports equally around the perimeter
        q->placePortsAuto();
    }

    // refresh canvas
    libCanvas_->update();
    // scroll canvas view to the top
    libView_->setContentsPos( 0, 0 );

    statusBar()->message("Loaded "+QString::number(libraryModel_.size())
                         +" components from "+QString::number(filelist.size())
                         +" component definition files. "
                         +QString::number(filelist.size()-libraryModel_.size())
                         +" files failed validation.", 15000);

    cout<<"lib loaded. model:"<<libraryModel_.size()<<" view:"<<libCanvas_->allComponents().size()<<
            " ("<<libCanvas_->allItems().size()<<")"<<endl;
    return true;
}

void MainWindow::showAllLibrary() {
    interfaceFilter_->setCurrentItem( 0 );
    providedRequiredFilter_->setCurrentItem( 0 );

    filterLibrary();
}

void MainWindow::filterLibrary() {
    // find all components
    Q3CanvasItemList l = libCanvas_->allComponents();
    QorcaComponent* q;

    // component counter for display
    int i = 0;
    // query each component for the right interface
    for ( Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it ) 
    {
        q = (QorcaComponent*)(*it);       

        if ( q->hasInterface( nameFilter_->text(),
                              interfaceFilter_->currentText(),
                              providedRequiredFilter_->currentText() ) )
       {
            q->move( 100, 100 + 200*i++);
            q->reallyShow();
        } 
        else 
        {
            q->reallyHide();
        }
    }
    // scroll up
    libView_->ensureVisible( 0, 0 );

    libCanvas_->update();

    statusBar()->message("Showing "+QString::number(i)+" components.", 3000);
}

void MainWindow::newProject() {
    // clear proj file name
    projFileName_ = QString();
    // clear project component counter
    QorcaComponent::resetCounter();

    projModel_.clear();


    // if doesn't exist, create new, otherwise simply reset
/*
    if ( projDom_==0 ) {
        projDom_ = new QDomDocument();
    } else {
        *projDom_ = QDomDocument();
    }
    QDomElement root = projDom_->createElement( "project" );
    projDom_->appendChild( root );
    root.setAttribute( "version", "0.12" );
    root.setAttribute( "xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance" );
    root.setAttribute( "xmlns", "http://orca-robotics.sf.net/component" );
    root.setAttribute( "xmlns:o", "http://orca-robotics.sf.net/definition" );
    root.setAttribute( "xmlns:go", "http://orca-robotics.sf.net/gorca-project" );
    root.setAttribute( "xsi:schemaLocation", "http://orca-robotics.sf.net/component /usr/local/share/orca/schemas/generic.xsd http://orca-robotics.sf.net/definition /usr/local/share/orca/schemas/definition.xsd http://orca-robotics.sf.net/gproject /usr/local/share/orca/schemas/gproject.xsd" );

    // header
    QDomElement header = projDom_->createElement( "header" );
    root.appendChild( header );
    // set created/modified date and time
    QString rightnow = QDateTime::currentDateTime().toString();
    header.setAttribute( "dateCreated", rightnow );
    header.setAttribute( "dateModified", rightnow );
    header.setAttribute( "creator", "orca-robotics.sf.net" );
    header.setAttribute( "title", "First G-Orca Project" );

    QDomElement components = projDom_->createElement( "components" );
    root.appendChild( components );

    QDomElement links = projDom_->createElement( "links" );
    root.appendChild( links );
*/
    // say this only when closing a project
    //    statusBar()->message("Created a new project.", 3000);
}

void MainWindow::copyLibToProj( const QPoint& dropPoint )
{
    cout<<"MainWindow::copyLibToProj"<<endl;
    // find all components
    Q3CanvasItemList l = libCanvas_->allComponents();

    QorcaComponent* q;
    int counter = 0;
    // copy every selected module
    for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) 
    {
        q = (QorcaComponent*)(*it);

        if ( q->isSelected() ) 
        {
            // Add an entry in the project model
            projModel_.push_back( new ComponentModel( q->def(), q->cfg() ) );

            // Create an associated GUI element
            int i=projModel_.size()-1;
            QorcaComponent* newComp = new QorcaComponent(  projCanvas_, 
                                                          *projModel_[i] );
            newComp->setCenter( dropPoint.x(), dropPoint.y() );
            newComp->setActive( true );
            newComp->show();
            // spread the ports equally around the perimeter
            newComp->placePortsAuto();
            counter++;
        }
    }
    // refresh canvas
    projCanvas_->update();

    statusBar()->message("Copied "+QString::number(counter)
                         +" components to the project.", 3000);

    cout<<"TRACE(mainwin.cpp): after copyLibToProj: " << endl << toString( projModel_ ) << endl;
    
}

void MainWindow::printLibrary() {
    // debug
    cout<<libDom_->toString().toStdString()<<endl;
}

void MainWindow::selectAll() {
    // find all components
    Q3CanvasItemList l = projCanvas_->allComponents();
    QorcaComponent* q;
    for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
        q = (QorcaComponent*)(*it);
        q->setSelected( true );
    }
    projCanvas_->update();
}

void MainWindow::validateSelected() {
    QMessageBox::information( 0,"Critical Error","Not implemented" );
    return;
}

void MainWindow::saveProject() 
{

    cout<<"TRACE(mainwin.cpp): SAVING PROJECT!!" << endl;

    // find all components
    Q3CanvasItemList l = projCanvas_->allComponents();

#ifdef ALEX_HOW_DOES_THIS_WORK
    if ( projFileName_.isEmpty() ) {
        projFileName_ = Q3FileDialog::getSaveFileName(
            ".",
            "Projects (*.xml *.gop)",
            this,
            "open file dialog"
            "Choose a project file" );
        // test for cancel
        if ( projFileName_.isNull() ) {
            return;
        }
    }
    // write
//     QFile pfile( projFileName_ );
//     if ( pfile.open( QIODevice::WriteOnly ) ) {
//         QTextStream stream( &pfile );
//         stream << QString::fromStdString(s) <<endl;
//         pfile.close();
//     }
#endif

    // Hard-code it for now.
    std::string fileName = "saveFile.gop";

    gorca_loadsave::toFile( projModel_, fileName );

//     // tell all components to dump their data to file
//     for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) 
//     {
//         qc = (QorcaComponent*)(*it);

//         cout<<"TRACE(mainwin.cpp): Saving qc:" << endl;
//         string saveStr = gorca_loadsave::toString( *qc );
//         cout << saveStr;

//         gorca_loadsave::toFile( *qc, fileName );

//         // Test: load/save and compare
// //         cout<<"TRACE(mainwin.cpp): TESTING LOAD/SAVE!!" << endl;
// //         gorca_loadsave::fromFile( *qc, fileName );
// //         gorca_loadsave::toFile( *qc, "compare.out" );
//     }


/*
    // find all links
    l = projCanvas_->allLinks();
    //cout<<"MainWindow::saveProject: project has "<<l.size()<<" links"<<endl;

    QorcaLink* qlink;
    QDomNodeList nlist = projDom_->elementsByTagName( "links" );
    QDomNode links = nlist.item( 0 );
    // is there a better way to delete all elements?
    nlist = links.childNodes();    
    cout<<"MainWindow::saveProject: the old xml file has "<<nlist.length()<<" links"<<endl;

    while ( links.childNodes().length() ) {
        QDomNode n = links.removeChild( links.childNodes().item(0) );
        if ( !n.isNull() ) {
            //cout<<"removed node successfuly"<<endl;
        } else {
            cout<<"warning: failed to remove node"<<endl;
        }
    }
    // count again
    nlist = links.childNodes();    
    cout<<"MainWindow::saveProject: the old xml file now has "<<nlist.length()<<" links"<<endl;
    
    // tell all links to dump there data to xml
    for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
        qlink = (QorcaLink*)(*it);
        QDomElement xlink = projDom_->createElement( "link" );
        links.appendChild( xlink );
        qlink->setXml( xlink );
        qlink->writeXml();
    }
    // confirm new xml
    QDomNodeList nlistNew = projDom_->elementsByTagName( "link" );
    QDomNode linksNew = nlistNew.item( 0 );
    nlistNew = links.childNodes();    
    cout<<"MainWindow::saveProject: the new xml file has "<<nlistNew.length()<<" links"<<endl;
    
    // reset modified date and time
    QString rightnow = QDateTime::currentDateTime().toString();
    nlist = projDom_->elementsByTagName( "header" );
    nlist.item(0).toElement().setAttribute( "dateModified", rightnow );
*/

    statusBar()->message("Saved project to file \""
                         +projFileName_+"\".", 3000);
}

void MainWindow::closeProject() 
{
    cout<<"TRACE(mainwin.cpp): closeProject()" << endl;

    // clear display (it will also clear canvas)
    projView_->clear();
    // clear document
    newProject();

    statusBar()->message("Created a new project.", 3000);

    cout<<"TRACE(mainwin.cpp): Finished closeProject()" << endl;

}

void MainWindow::openProject() 
{
    // Clear the current project
    closeProject();

    cout<<"TRACE(mainwin.cpp): OPEN PROJECT!!!" << endl;

    std::string fileName = "saveFile.gop";

    gorca_loadsave::fromFile( projModel_, fileName );

    //
    // Instantiate GUI components based on the model we just loaded
    //
    std::vector<QorcaComponent*> allComps;
    for ( unsigned int i=0; i < projModel_.size(); i++ )
    {
        QorcaComponent *newComp = new QorcaComponent( projCanvas_, 
                                                      *(projModel_[i]) );
        newComp->show();
        allComps.push_back(newComp);
    }

    //
    // Set up the required links of each component
    //
    cout<<"TRACE(mainwin.cpp): Setting up links..." << endl;
    for ( unsigned int i=0; i < projModel_.size(); i++ )
    {
        cout<<"TRACE(mainwin.cpp): i: " << i << " of " << projModel_.size()  << endl;
        cout<<"  - "<<allComps[i]->description()<<endl;
        // Loop over our required interfaces: reqI
        for ( unsigned int reqI=0; reqI < projModel_[i]->cfg.required.size(); reqI++ )
        {
            cout<<"    reqI: " << reqI << " of " << projModel_[i]->cfg.required.size() << endl;

            // NOTE: only works for indirect proxies
            orca::FQInterfaceName fqReq = orcaice::toInterfaceName( projModel_[i]->cfg.required[reqI].proxy );

            // Loop over all the other components: j
            for ( unsigned int j=0; j < projModel_.size(); j++ )
            {
                // Check the platform and component
                if ( fqReq.platform  == projModel_[j]->cfg.fqname.platform &&
                     fqReq.component == projModel_[j]->cfg.fqname.component )
                {
                    // Loop over its provided interfaces: provI
                    for ( unsigned int provI=0; provI < projModel_[j]->cfg.provided.size(); provI++ )
                    {
                        if ( fqReq.iface == projModel_[j]->cfg.provided[provI].name )
                        {
                            projView_->linkInterfaces( allComps[i]->required_.at(reqI),
                                                       allComps[j]->provided_.at(provI) );
                        }
                    }
                }
            }
        }
    }

    // Load all components
//    QorcaComponent q;

/*
    // close current project (creates a new one automatically)
    closeProject();

    projFileName_ = Q3FileDialog::getOpenFileName(
                        ".",
                        "Projects (*.xml *.gop)",
                        this,
                        "open file dialog"
                        "Choose a project file" );
    // test for cancel
    if ( projFileName_.isNull() ) {
        return;
    }
    // read the XML file
    QFile projFile( projFileName_ );
    if ( !projFile.open( QIODevice::ReadOnly ) ) {
        QMessageBox::critical( 0,"Critical Error","Cannot open file "+projFileName_ );
        return;
    }
    // if the file is valid add it to the repository
    // namespace process is OFF, otherwise have to use special NS functions
    if ( !projDom_->setContent( &projFile, false ) ) {
        projFile.close();
        return;
    }

    // find <components> node in the project DOM (local namespace)
    QDomNodeList nodelist = projDom_->elementsByTagName( "o:componentDefinition" );
    //cout<<"MainWindow::openProject: found "<<nodelist.length()<<" components in the project file"<<endl;

    for ( unsigned int i=0; i<nodelist.length(); i++ ) {
        // stick into canvas
        QorcaComponent* c = new QorcaComponent( projCanvas_, nodelist.item(i).toElement() );
        c->setActive( true );
        c->show();
    }

    // parse links
    QDomNodeList linklist = projDom_->elementsByTagName( "link" );
        cout<<"MainWindow::openProject: found "<<linklist.length()<<" links in the project file"<<endl;

    for ( unsigned int i=0; i<linklist.length(); i++ ) {
        // read id's of connected components
        int from = linklist.item(i).toElement().attribute( "from" ).toInt();
        int to = linklist.item(i).toElement().attribute( "to" ).toInt();

        QorcaInterface* qfrom = lookupPort( from );
        QorcaInterface* qto = lookupPort( to );

        if ( qfrom && qto ) {
            QorcaLink* q = new QorcaLink( projCanvas_, qfrom, qto );
            q->show();
            // tell the ports that they are linked
            // only the client will do something (we don't which one it
            // is but we don't care).
            qfrom->linkEvent();
            qto->linkEvent();
        } else {
            cout<<"MainWindow::openProject: couldn't find ports "<<from<<"->"<<to<<endl;
        }
    }

    // refresh canvas
    projCanvas_->update();

    statusBar()->message("Loaded project with "+QString::number(nodelist.length())
                         +" components and "+QString::number(linklist.length())
                         +" links.", 9000);
*/
}

/*!
    This function does 3 things:
    1) tells every component to save its XML config file
    2) writes an XML system file containing information on all auto-start components
    3) writes a text script file which can be used to start up the system as well (this may be discontinued soon)
*/
void MainWindow::exportProject() {
    //cout<<"MainWindow::exportProjec: debug: writing project"<<endl;
    setupProjectDir();

    int nconfig = exportConfigFiles();
    int nauto   = exportSystemFile();
    exportHostFiles();
    exportScriptFile();

    statusBar()->message("Saved config files for "+QString::number(nconfig)
                         +" components, including "+QString::number(nauto)+" with auto start.", 9000);
}

void MainWindow::setupProjectDir() {
    // clean up host directories
    QDir projDir( "." );
    projDir.setFilter( QDir::Dirs | QDir::NoSymLinks );
    QStringList dirs = projDir.entryList();
    for ( QStringList::Iterator it = dirs.begin(); it != dirs.end(); ++it ) {
        if ( *it != "." && *it != ".." ) {
            QDir hostDir( *it );
            hostDir.setFilter( QDir::Files );
            QStringList files = hostDir.entryList();
            for ( QStringList::Iterator it2 = files.begin(); it2 != files.end(); ++it2 ) {
                if ( !hostDir.remove( *it2 ) ) {
                    cout<<"coudn't remove "<<it2->toStdString()<<" file"<<endl;
                }
            }
            if ( !projDir.rmdir( *it ) ) {
                cout<<"coudn't remove "<<it->toStdString()<<" dir"<<endl;
            }
        }
    }

    // make a list of hosts
    Q3CanvasItemList l = projCanvas_->allComponents();
    QorcaComponent* qc;
    QStringList hosts;
    for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
        qc = (QorcaComponent*)(*it);
        if ( hosts.contains( qc->hostName() ) == 0 ) {
            hosts << qc->hostName();
        }
    }

    // create host directories
    for ( QStringList::Iterator it = hosts.begin(); it != hosts.end(); ++it ) {
        if ( !projDir.exists( *it ) ) {
            if ( projDir.mkdir( *it, false ) ) {
                //cout<<"created "<<*it<<" dir"<<endl;
            } else {
                cout<<"coudn't create "<<it->toStdString()<<" dir"<<endl;
            }
        }
    }
}

int MainWindow::exportConfigFiles() {
    // find all components
    Q3CanvasItemList l = projCanvas_->allComponents();

#if 0
    QorcaComponent* qc;
    // tell all components to dump there data to xml
    for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
        //cout<<"MainWindow::exportConfigFiles: debug: writing component"<<endl;
        qc = (QorcaComponent*)(*it);
        qc->exportConfig();
    }
#endif
    return l.size();
}

int MainWindow::exportSystemFile() {
    QDomDocument sysDom;

    QDomElement root = sysDom.createElement( "system" );
    sysDom.appendChild( root );
    root.setAttribute( "version", "0.12" );
    root.setAttribute( "info", "Autogenerated by Gorca." );
    root.setAttribute( "xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance" );
    root.setAttribute( "xmlns","http://orca-robotics.sf.net/system");
    root.setAttribute( "xsi:schemaLocation", "http://orca-robotics.sf.net/system /usr/local/share/orca/schemas/system.xsd" );

    // find all components
    Q3CanvasItemList l = projCanvas_->allComponents();
    int nauto = 0;

    QorcaComponent* qc;
    // tell all components to dump there data to xml
    for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
        qc = (QorcaComponent*)(*it);

        QDomElement deploy = sysDom.createElement( "componentDeployment" );
        root.appendChild( deploy );

        deploy.setAttribute( "hostName", qc->hostName() );
        deploy.setAttribute( "executeFileName", qc->exeFileName() );
        deploy.setAttribute( "configFileName", qc->configFileName() );
        /*
        if ( qc->isAutoStart() ) {
            deploy.setAttribute( "autoStart", "true" );
            nauto++;
        } else {
            deploy.setAttribute( "autoStart", "false" );
        }
        */
        // we don't need platform name here
        //deploy.setAttribute( "platformName", qc->platformName() );
    }

    // write the system file
    QFile pfile( "system.xml" );
    if ( pfile.open( QIODevice::WriteOnly ) ) {
        QTextStream stream( &pfile );
        stream << "<?xml version=\"1.0\"?>" << endl;
        stream << "<!-- Orca system deployment file  -->" << endl;
        stream << "<!-- Automatically generated by Gorca  -->" << endl;
        stream << sysDom.toString()<<endl;
        pfile.close();
    }

    return nauto;
}


int MainWindow::exportHostFiles() {
    // make a list of hosts
    Q3CanvasItemList l = projCanvas_->allComponents();
    QorcaComponent* qc;
    QStringList hosts;
    for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
        qc = (QorcaComponent*)(*it);
        if ( hosts.contains( qc->hostName() ) == 0 ) {
            hosts << qc->hostName();
        }
    }

    // for each host, create a system file
    for ( QStringList::Iterator it = hosts.begin(); it != hosts.end(); ++it ) {
        QString thisHost = *it;
        QDir hostDir( *it );

        QDomDocument hostDom;

        QDomElement root = hostDom.createElement( "system" );
        hostDom.appendChild( root );
        root.setAttribute( "version", "0.12" );
        root.setAttribute( "info", "Autogenerated by Gorca for host " + thisHost + "." );
        root.setAttribute( "xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance" );
        root.setAttribute( "xmlns","http://orca-robotics.sf.net/system");
        root.setAttribute( "xsi:schemaLocation", "http://orca-robotics.sf.net/system /usr/local/share/orca/schemas/system.xsd" );

        // find all components
        Q3CanvasItemList l = projCanvas_->allComponents();

        QorcaComponent* qc = 0;
        // tell all components to dump there data to xml
        for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
            qc = (QorcaComponent*)(*it);

            // check host name
            if ( qc->hostName() != thisHost ) {
                continue;
            }

            QDomElement deploy = hostDom.createElement( "componentDeployment" );
            root.appendChild( deploy );

            deploy.setAttribute( "hostName", qc->hostName() );
            deploy.setAttribute( "executeFileName", qc->exeFileName() );
            deploy.setAttribute( "configFileName", qc->configFileName() );
            /*
            if ( qc->isAutoStart() ) {
                deploy.setAttribute( "autoStart", "true" );
                nauto++;
            } else {
                deploy.setAttribute( "autoStart", "false" );
            }
            */
            // we don't need platform name here
            //deploy.setAttribute( "platformName", qc->platformName() );
        }

        // write the system file
        QFile pfile( hostDir.filePath( "host.xml" ) );
        if ( pfile.open( QIODevice::WriteOnly ) ) {
            QTextStream stream( &pfile );
            stream << "<?xml version=\"1.0\"?>" << endl;
            stream << "<!-- Orca system deployment file  -->" << endl;
            stream << "<!-- Automatically generated by Gorca  -->" << endl;
            stream << hostDom.toString()<<endl;
            pfile.close();
        }
    }

    return hosts.size();
}

int MainWindow::exportScriptFile() {
    // generate a script which will distribute and start up all components in the project
    QString script;
    script = "#!/bin/sh\n\n";

    // find all components
    Q3CanvasItemList l = projCanvas_->allComponents();

    QorcaComponent* qc;
    int counter = 0;
    // tell all components to dump there data to xml
    for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
        qc = (QorcaComponent*)(*it);

        // write to script, if autostart
        /*
        if ( qc->isAutoStart() ) {
            script.append( "start_orca_component.sh "
                           + qc->hostName() + " "
                           + qc->exeFileName() + " "
                           + qc->platformName() + " "
                           + qc->configFileName() + "\n\n" );
            counter++;
        }
        */
    }

    // write the script which will distribute and start up all components in the project
    QFile scriptfile( "start.sh" );
    if ( scriptfile.open( QIODevice::WriteOnly ) ) {
        QTextStream stream( &scriptfile );
        stream << script <<endl;
        scriptfile.close();
    }

    return counter;
}

void MainWindow::showProjectInfo() {
    int ncomps = projCanvas_->allComponents().size();
    int nlinks= projCanvas_->allLinks().size();
    int nplatfs = projCanvas_->platformCount();
    int nhosts = projCanvas_->hostCount();

    //    QMessageBox::information( this, "G-Orca",
    //                              "Components\t\t"+QString::number(ncomps)+"\n"
    //                              "Links\t\t\t"+QString::number(nlinks) );
    statusBar()->message(QString::number(ncomps)+" components, "
                         + QString::number(nlinks)+" links, "
                         + QString::number(nplatfs)+" platforms, "
                         + QString::number(nhosts)+" hosts.", 9000);
}

void MainWindow::reloadView() {
    libCanvas_->update();
    projCanvas_->update();
}

void MainWindow::toggleLibrary() {
    if ( libView_->isVisible() ) {
        libView_->hide();
        interfaceFilter_->hide();
        providedRequiredFilter_->hide();
    } else {
        libView_->show();
        interfaceFilter_->show();
        providedRequiredFilter_->show();
    }
}

QorcaInterface* MainWindow::lookupPort( const int pid ) {
    Q3CanvasItemList al = projCanvas_->allPorts();
    cout<<"MainWindow::lookupPort: searching "<<al.size()<<" ports"<<endl;
    QorcaInterface* qp;

    for (Q3CanvasItemList::Iterator it=al.begin(); it!=al.end(); ++it) {
        qp = (QorcaInterface*)(*it);
        if ( qp->id() == pid) {
            cout<<"got it!"<<endl;
            return qp;
        } else {
            cout<<qp->id()<<" ? "<< pid<<endl;
        }
    }
    return 0;
}

void MainWindow::enlarge() {
    projCanvas_->resize(projCanvas_->width()*4/3, projCanvas_->height()*4/3);
}

void MainWindow::shrink() {
    projCanvas_->resize(projCanvas_->width()*3/4, projCanvas_->height()*3/4);
}

void MainWindow::zoomIn() {
    QMatrix m = projView_->worldMatrix();
    m.scale( 1.25, 1.25 );
    projView_->setWorldMatrix( m );
}

void
MainWindow::zoomOut() {
    QMatrix m = projView_->worldMatrix();
    m.scale( 0.75, 0.75 );
    projView_->setWorldMatrix( m );
}

void
MainWindow::zoomReset() {
    projView_->setWorldMatrix( QMatrix() );
}

void
MainWindow::quit() {
    qApp->exit();
}

void
MainWindow::aboutOrca()
{
    QPixmap orcaIcon( orca2_2x3_yellow_130_xpm );

    QMessageBox mb( this );
    mb.setWindowTitle("About Orca");
    mb.setWindowIcon ( orcaIcon );
    mb.setText("Orca is an open-source framework for\n"
               "developing component-based robotic systems. It\n"
               "provides the means for defining and developing the\n"
               "building-blocks which can be pieced together to\n"
               "form arbitrarily complex robotic systems, from single\n"
               "vehicles to distributed sensor networks.\n\n"
               "http://orca-robotics.sf.net\n" );
    mb.setIconPixmap( orcaIcon );
    //mb.setMaximumWidth( 400 );
    mb.exec();
}

void
MainWindow::aboutApp()
{
    QPixmap orcaIcon( orca2_2x3_yellow_130_xpm );

    QMessageBox mb( this );
    mb.setWindowTitle("About G-Orca");
    mb.setWindowIcon ( orcaIcon );
    mb.setText("Graphical system composition tool.\n\n"
                "http://orca-robotics.sf.net\n" );
    mb.setIconPixmap( orcaIcon );
    //mb.setMaximumWidth( 400 );
    mb.exec();
}
