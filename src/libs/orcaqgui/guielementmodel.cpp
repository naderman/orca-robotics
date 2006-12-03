/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaqgui/orcaguiuserevent.h>
#include <orcaice/orcaice.h>

#include "guielementmodel.h"
#include "guielementfactory.h"
#include "guielementview.h"
#include "ihumanmanager.h"

using namespace std;

namespace orcaqgui {
    
// ================= INTERFACENODE ===========================================
GuiElementModel::InterfaceNode::InterfaceNode( const QString &r, const QString &p, const QString &c, const QString &iface, const QString &i  )
    : registry(r),
      platform(p),
      component(c),
      interface(iface),
      id(i),
      element(0)
{
}

GuiElementModel::InterfaceNode::~InterfaceNode()
{
    delete element;
}

// to be used in list search, name is sufficient
bool
GuiElementModel::InterfaceNode::operator==( const InterfaceNode & other ) const
{
    return registry==other.registry && platform==other.platform
        && component==other.component && interface==other.interface
        && id==other.id;
}
// ==============================================================================

GuiElementModel::GuiElementModel( const std::vector<orcaqgui::GuiElementFactory*> &factories,
                                  const orcaice::Context & context, 
                                  IHumanManager *messageDisplayer,
                                  QObject *parent )
    : QAbstractTableModel(parent),
      factories_(factories),
      context_(context),
      humanManager_(messageDisplayer),
      colorCounter_(0),
      view_(0)
{
    double timeInSec = orcaice::timeAsDouble( orcaice::getNow() );
    seed_ = (uint)floor(timeInSec);
    
    colorVector_.push_back(Qt::black);  // this one gets assigned to "global", no display
    colorVector_.push_back(Qt::red);
    colorVector_.push_back(Qt::blue);
    colorVector_.push_back(Qt::cyan);
    colorVector_.push_back(Qt::magenta);
    colorVector_.push_back(Qt::darkYellow);    
    colorVector_.push_back(Qt::darkRed);
    colorVector_.push_back(Qt::darkBlue);
    colorVector_.push_back(Qt::darkGreen);
    colorVector_.push_back(Qt::darkCyan);
    colorVector_.push_back(Qt::darkMagenta);
    
    headers_ << "Name" << "Details";
    coordinateFramePlatform_ = "global";
    platformInFocus_ = "global";
}

GuiElementModel::~GuiElementModel()
{
}

int GuiElementModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return elements_.size();
}

int
GuiElementModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return headers_.size();
}

QVariant
GuiElementModel::data(const QModelIndex &idx, int role) const
{
//     cout<<"data for (r,c)=("<<idx.row()<<","<<idx.column()<<") ["<<rowCount(QModelIndex())<<"]"<<endl;
    if (!idx.isValid()) {
        return QVariant();
    }
    if (idx.row()>rowCount( idx.parent() ) && idx.row()<0
                && idx.column()>columnCount( idx.parent() ) && idx.column()<0) {
        //cout<<"data: didn't pass test "<<idx.row()<<","<<idx.column()<<endl;
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        switch ( idx.column() ) {
        case 0 :
        {
            QString fqIName = elements_[idx.row()]->interface+"@"
                        +elements_[idx.row()]->platform+"/"
                        +elements_[idx.row()]->component;
            return fqIName;
        }
        case 1 :
            return elements_[idx.row()]->id;
        }
    }  
    else if ( role == InterfaceIdRole ) {
        // for all columns
        return elements_[idx.row()]->id;
    }  
    else if ( role == ContextMenuRole ) {
        // for all columns
        if ( elements_[idx.row()]->element==0 ) {
            return QVariant();
        }
        return elements_[idx.row()]->element->contextMenu();
    }
    else if ( role == FocusRole ) {
        // for all columns
        // if it's in Global CS we can set the coordinate frame to it.
        return elements_[idx.row()]->element->isInGlobalCS();
    }   // role

    return QVariant();
}


QVariant
GuiElementModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal ) {
        return QVariant();
    }
    return headers_.at( section );
}

bool
GuiElementModel::removeRows( int row, int count, const QModelIndex & parent )
{
    //cout<<"removing row "<<row<<endl;
    
    assert( row>-1 && row<elements_.size() && "row number must exist" );
    // too lazy to implement multi-row removal
    assert( count==1 && "only removal of one row at a time is implemented" );
    assert( parent==QModelIndex() && "this is a table model, parent must be root" );

    // if it's the mode owner, reset the mode
    if ( elements_[row]->element == humanManager_->modeOwner() )
        humanManager_->modeOwner()->lostMode();

    beginRemoveRows( QModelIndex(), row, row );
    // the elements_ list contains pointers, we have to delete the object
    delete elements_[row];
    // now remove the entry in the list
    elements_.removeAt( row );
    endRemoveRows();

    return true;
}

GuiElement *
GuiElementModel::instantiateFromFactories( const QString &id, const QColor &platformColor, const QString &proxyStr )
{
    for ( uint i=0; i < factories_.size(); i++ )
    {
        // if this interface is not supported, skip this factory
        if ( !factories_[i]->isSupported( id ) )
            continue;

        return factories_[i]->create( context_, id, proxyStr, platformColor, humanManager_  );
    }
    return NULL;
}
                                      

void
GuiElementModel::createGuiElement( const QStringList & interfaceInfo )
{
    Q_ASSERT( interfaceInfo.size()==5 );

    // find our interface or make a new one
    InterfaceNode* node = new InterfaceNode( interfaceInfo[0], interfaceInfo[1], interfaceInfo[2],
                      interfaceInfo[3], interfaceInfo[4] );

    QString proxyStr = interfaceInfo[3]+"@"+interfaceInfo[1]+"/"+interfaceInfo[2];
    
    cout<<"TRACE(guielementmodel.cpp): creating element with of type "<<node->id.toStdString()<<" with proxy "<<proxyStr.toStdString()<<endl;
    
    // 
    // Set color for all elements on the platform
    //
    QColor platformColor;
    if ( isNewPlatform( node->platform ) )
    {
        // assign a new colour
        if ( colorCounter_>=colorVector_.size() ) {
            platformColor = generateRandomColor();
        } else {
            platformColor = colorVector_[colorCounter_];
            colorCounter_++;
        }
        // save for the future
        colorMap_[node->platform] = platformColor;
        //cout<<"TRACE(guielementmodel.cpp): emit newPlatform signal" << endl;
        emit ( newPlatform(node->platform) );
    }
    else
    {
        // lookup in our map
        platformColor = colorMap_[node->platform];
    }
    //node->element->setColor( color );

    //
    // Instantiate the GuiElement of the right kind
    //
    node->element = instantiateFromFactories( node->id, platformColor, proxyStr );
    if (node->element==NULL)
    {
        //cout << "TRACE(guielementmodel.cpp): Interface not supported." << endl;
        humanManager_->showBoxMsg(orcaqgui::Warning, "Interface is not supported by the GUI");
        delete node;
        return;   
    }
    
    
    //
    // We need to tell the new element whether it's in focus or not
    //
    if (node->platform == platformInFocus_)
    {
        node->element->setFocus( true );
    }
    else if ( platformInFocus_== "global")
    {
        node->element->setFocus( true );
    } 
    else 
    {
        node->element->setFocus( false );
    }
        
    int ii = elements_.indexOf( node );
    if ( ii==-1 ) {    
        ii = elements_.size();
        cout<<"TRACE(guielementmodel.cpp): creating interface node "<<ii<<endl;

        //
        // stick new node into the list
        //
        beginInsertRows( QModelIndex(), ii,ii );
        elements_.append( node );
        endInsertRows();
        
        // hide if not in focus
        if ( (node->platform != platformInFocus_) && (view_!=NULL) && (platformInFocus_ != "global") ) {
            view_->hideRow( ii );
        }
        
    }
    else {
        //cout<<"using interface node "<<ii<<" for component "<<c->name.toStdString()<<endl;
    }

    //cout<<"finished creating"<<endl;
}

bool
GuiElementModel::isNewPlatform( QString &platformName )
{
    for ( int i=0; i<elements_.size(); ++i )
    {
        if ( elements_[i]->platform == platformName ) return false;
    }
    return true;    
}

QColor
GuiElementModel::generateRandomColor()
{    
    seed_ = seed_ + 10;
    srand(seed_);
    
    QColor colour;
    int r = (int)floor( (double)rand()/((double)(RAND_MAX)+1.0)*256 );
    int g = (int)floor( (double)rand()/((double)(RAND_MAX)+1.0)*256 );
    int b = (int)floor( (double)rand()/((double)(RAND_MAX)+1.0)*256 );       
    colour.setRgb(r,g,b);
    //cout << "random color is: " << r << " " << g << " " << b << endl;
    return colour;    
}


void 
GuiElementModel::changePlatformFocus( const QString &platform )
{
    // keep a local copy
    platformInFocus_ = platform;
    
    //cout << "TRACE(guielementmodel.cpp): changePlatformFocus: " << platform.toStdString() << endl;
    for ( int i=0; i<elements_.size(); ++i )
    {
        // special case: show everybody with their colours
        if (platform=="global") 
        {
            elements_[i]->element->setFocus(true);
            continue;
        }
        
        if ( elements_[i]->platform == platform )
        {
            elements_[i]->element->setFocus(true);
        }
        else
        {
            elements_[i]->element->setFocus(false);  
        }
    }
    
    // tell the guielementview to filter what it shows
    // collect indices
    if (platform=="global") {
        view_->showAllElements( elements_.size() );
        return;
    }
    
    vector<int> elementIndices;
    for ( int i=0; i<elements_.size(); ++i )
    {
        if (elements_[i]->platform!=platform)
            elementIndices.push_back(i);
    }
    view_->hideElements( elements_.size(), elementIndices );
}

void
GuiElementModel::removeAllGuiElements()
{
    for ( int i=0; i<elements_.size(); ++i )
    {
        // don't delete the permanent elements e.g. grid
        if ( elements_[i]->id != "::local::Grid" ) {
            removeRows( i, 1, QModelIndex() );
        }
    }
}

void
GuiElementModel::updateGuiElements()
{
    for ( int i=0; i<elements_.size(); ++i )
    {
        if ( elements_[i]->element ) {
            std::stringstream ss;
            try {
                elements_[i]->element->update();
            }
            catch ( Ice::Exception &e )
            {
                ss<<"TRACE(guielementmodel.h): Caught some ice exception during update of "
                  <<elements_[i]->id.toStdString()<<": " << e << std::endl;
                humanManager_->showStatusMsg(orcaqgui::Warning,ss.str().c_str());
            }
            catch ( std::exception &e )
            {
                ss<<"TRACE(guielementmodel.h): Caught some std exception during update of "
                  <<elements_[i]->id.toStdString()<<": " << e.what() << std::endl;
                humanManager_->showStatusMsg(orcaqgui::Warning,ss.str().c_str());
            }
            catch ( std::string &e )
            {
                ss<<"TRACE(guielementmodel.h): Caught std::string during update of "
                  <<elements_[i]->id.toStdString()<<": " << e << std::endl;
                humanManager_->showStatusMsg(orcaqgui::Warning,ss.str().c_str());
            }
            catch ( char *e )
            {
                ss<<"TRACE(guielementmodel.h): Caught char * during update of "
                  <<elements_[i]->id.toStdString()<<": " << e << std::endl;
                humanManager_->showStatusMsg(orcaqgui::Warning,ss.str().c_str());
            }
            catch ( ... )
            {
                ss<<"TRACE(guielementmodel.h): Caught some other exception during update of "
                  <<elements_[i]->id.toStdString()<<": " << std::endl;
                humanManager_->showStatusMsg(orcaqgui::Warning,ss.str().c_str());
            }
        }
    }
}

void
GuiElementModel::setCoordinateFramePlatform( int guiElementIndex )
{
    if ( guiElementIndex<0 || guiElementIndex >= elements_.size() ) return;

    // by convention, Grid's platform is "global"
    coordinateFramePlatform_ = elements_[guiElementIndex]->platform;
}

void
GuiElementModel::executeGuiElement( int guiElementIndex, int actionIndex )
{

    if ( guiElementIndex<0 || guiElementIndex >= elements_.size() || elements_[guiElementIndex]->element==NULL ) return;
    // debug
    cout<<"GuiElementModel:executeGuiElement: executing action #"<<actionIndex<<endl;

    elements_[guiElementIndex]->element->execute( actionIndex );
}

void 
GuiElementModel::setView( GuiElementView* view )
{ 
    view_ = view;
}

void
GuiElementModel::selectedAdaptersInView( vector<int> &indices )
{
    view_->selectedAdaptersInView( elements_.size(), indices );
}


std::string toString( const GuiElementModel::InterfaceNode &n )
{
    stringstream ss;
    ss << "reg: " << n.registry.toStdString()
       << ", platform: " << n.platform.toStdString()
       << ", comp: " << n.component.toStdString()
       << ", iface: " << n.interface.toStdString()
       << ", id: " << n.id.toStdString();
    return ss.str();
}

}




