/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaqgui/orcaguiuserevent.h>
#include <orcaice/orcaice.h>

#include "ipermanentelement.h"
#include "guielementmodel.h"
#include "guielementfactory.h"
#include "guielementview.h"
#include "ihumanmanager.h"
#include "platformcolor.h"

using namespace std;

namespace orcaqgui {

GuiElementModel::GuiElementModel( const std::vector<orcaqgui::GuiElementFactory*> &factories,
                                  const orcaice::Context & context, 
                                  IHumanManager *messageDisplayer,
                                  QObject *parent )
    : QAbstractTableModel(parent),
      factories_(factories),
      context_(context),
      humanManager_(messageDisplayer),
      view_(0),
      currentTransparency_(true)
{    
    platformColor_ = new PlatformColor;
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
            QString fqIName = elements_[idx.row()]->name();
            return fqIName;
        }
        case 1 :
            return elements_[idx.row()]->details();
        }
    }  
    else if ( role == InterfaceIdRole ) {
        // for all columns
        return elements_[idx.row()]->details();
    }  
    else if ( role == ContextMenuRole ) {
        // for all columns
        if ( elements_[idx.row()]==0 ) {
            return QVariant();
        }
        return elements_[idx.row()]->contextMenu();
    }
    else if ( role == FocusRole ) {
        // for all columns
        // if it's in Global CS we can set the coordinate frame to it.
        return elements_[idx.row()]->isInGlobalCS();
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
    
    // save the platform name
    QString platform = elements_[row]->platform();
    
    assert( row>-1 && row<elements_.size() && "row number must exist" );
    // too lazy to implement multi-row removal
    assert( count==1 && "only removal of one row at a time is implemented" );
    assert( parent==QModelIndex() && "this is a table model, parent must be root" );

    // if it's the mode owner, reset the mode
    if ( elements_[row] == humanManager_->modeOwner() )
        humanManager_->modeOwner()->lostMode();

    beginRemoveRows( QModelIndex(), row, row );
    // the elements_ list contains pointers, we have to delete the object
    delete elements_[row];
    // now remove the entry in the list
    elements_.removeAt( row );
    endRemoveRows();
    
    
    // if the removed element was the last one on its platform, we need to tell
    // mainwin, so it can remove it from the combo box
    if (!doesPlatformExist(platform))
    {
        //cout << "platform needs removal" << endl;
        emit platformNeedsRemoval(platform);
    }
    
    return true;
}

bool
GuiElementModel::instantiateFromFactories( GuiElement* &element, const QStringList &ids, const QColor &platformColor, const QStringList &proxyStrList )
{
    for ( unsigned int i=0; i < factories_.size(); i++ )
    {
        // if this interface is not supported, skip this factory
        if ( !factories_[i]->isSupported( ids ) )
            continue;
        
        // if we get here the interface is supported
        element = factories_[i]->create( context_, ids, proxyStrList, platformColor, humanManager_  );
        return true; 
    }
    return false;
}

bool
GuiElementModel::doesInterfaceExist( const QStringList& proxyStrList, int numElements )
{
    //cout << "proxyStrList.join " << proxyStrList.join(" ").toStdString() << endl;
    
    for ( int i=0; i<elements_.size(); i++)
    {
        cout << "element name: " << elements_[i]->name().toStdString() << endl;
        if ( elements_[i]->name() == proxyStrList.join(" ") + " " ) return true;
    }
    
    return false;     
}

void 
GuiElementModel::createGuiElement( const QList<QStringList> & interfacesInfo )
{    
    // get interface data out of stringlist
    QStringList ids;
    QStringList proxyStrList;
    QStringList platformStrList;
    for (int i=0; i<interfacesInfo.size(); i++)
    {
        QStringList info = interfacesInfo[i]; 
        ids << info[4];
        //cout << "ids: " << info[3].toStdString() << endl;
        proxyStrList << info[3]+"@"+info[1]+"/"+info[2];
        //cout << "proxylist: " << (info[3]+"@"+info[1]+"/"+info[2]).toStdString() << endl;
        platformStrList << info[1];
        //cout << "platform: " << info[1].toStdString() << endl;
    }
    
    bool haveThisInt = doesInterfaceExist( proxyStrList, interfacesInfo.size() );
    if (haveThisInt) {
        humanManager_->showStatusMsg(Warning,"Interface " + proxyStrList.join(" ") + " exists already. Not connecting again!");
        return;
    }
    
    // set platform name: if they disagree, set to global
    QString platform = platformStrList[0];
    for (int i=1; i<platformStrList.size(); i++)
    {   
        if (platformStrList[i] != platform)
            platform="global";
    }        
    
    // 
    // Set color for all elements on the platform
    //
    QColor platformColor;
    if ( !doesPlatformExist( platform ) )
    {
        platformColor_->setNewPlatform( platform );
        emit ( newPlatform(platform) );
    }
    platformColor_->getColor( platform, platformColor );
    
    GuiElement* element = NULL;
    bool isSupported = instantiateFromFactories( element, ids, platformColor, proxyStrList );
    if (!isSupported || element==NULL)
    {
        if (!isSupported) humanManager_->showStatusMsg(orcaqgui::Warning, "Element " + proxyStrList.join(" ") + " is not supported by any factory. Needed id: " + ids.join(" "));
        if (element==NULL) humanManager_->showStatusMsg(orcaqgui::Warning, "Element " + proxyStrList.join(" ") + " returned from factory is NULL");
        delete element;
        if (!doesPlatformExist( platform ) ) 
            emit platformNeedsRemoval(platform);
        return;   
    }
    
    // set properties of guielement
    element->setTransparency( currentTransparency_ );
    element->setPlatform( platform );
    QString details = "";
    QString name = "";
    for (int i=0; i<ids.size(); i++)
    {
        details = details + ids[i] + " ";
        name = name + proxyStrList[i] + " ";
    }
    element->setDetails( details );
    element->setName(name);
    
    //
    // We need to tell the new element whether it's in focus or not
    //
    if (platform == platformInFocus_ || platformInFocus_== "global" ) {
        element->setFocus( true );
    } else  {
        element->setFocus( false );
    }
        
    int ii = elements_.indexOf( element );
    if ( ii==-1 ) {    
        ii = elements_.size();
        //cout<<"TRACE(guielementmodel.cpp): creating element "<<ii<<endl;

        //
        // stick new node into the list
        //
        beginInsertRows( QModelIndex(), ii,ii );
        elements_.append( element );
        endInsertRows();
        
        // hide if not in focus
        if ( (platform != platformInFocus_) && (view_!=NULL) && (platformInFocus_ != "global") ) {
            view_->hideRow( ii );
        }
        
    }
}
        

bool
GuiElementModel::doesPlatformExist( QString &platformName )
{
    for ( int i=0; i<elements_.size(); ++i )
    {
        if ( elements_[i]->platform() == platformName ) return true;
    }
    return false;    
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
            elements_[i]->setFocus(true);
            continue;
        }
        
        if ( elements_[i]->platform() == platform )
        {
            elements_[i]->setFocus(true);
        }
        else
        {
            elements_[i]->setFocus(false);  
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
        if (elements_[i]->platform() != platform)
            elementIndices.push_back(i);
    }
    view_->hideElements( elements_.size(), elementIndices );
}

void
GuiElementModel::removeAllGuiElements()
{
    QVector<int> indeces;
    
    // compile a list of indeces
    for ( int i=0; i<elements_.size(); ++i )
    {
        // don't delete the permanent elements e.g. grid
        IPermanentElement *permElement = dynamic_cast<IPermanentElement*>(elements_[i]);
        if ( permElement==NULL ) {
            indeces.push_back(i);
        }
    }
    
    // sort list
    qSort(indeces);
    
    // remove rows from bottom to top
    for ( int i=indeces.size()-1; i>=0; i--)
    {
        removeRows( indeces[i], 1, QModelIndex() );
    }
    
}

void
GuiElementModel::updateGuiElements()
{
    for ( int i=0; i<elements_.size(); ++i )
    {
        if ( elements_[i] ) {
            std::stringstream ss;
            try {
                elements_[i]->update();
            }
            catch ( Ice::Exception &e )
            {
                ss<<"TRACE(guielementmodel.cpp): Caught some ice exception during update of "
                <<elements_[i]->details().toStdString()<<": " << e << std::endl;
                humanManager_->showStatusMsg(orcaqgui::Warning,ss.str().c_str());
            }
            catch ( std::exception &e )
            {
                ss<<"TRACE(guielementmodel.cpp): Caught some std exception during update of "
                <<elements_[i]->details().toStdString()<<": " << e.what() << std::endl;
                humanManager_->showStatusMsg(orcaqgui::Warning,ss.str().c_str());
            }
            catch ( std::string &e )
            {
                ss<<"TRACE(guielementmodel.cpp): Caught std::string during update of "
                   <<elements_[i]->details().toStdString()<<": " << e << std::endl;
                humanManager_->showStatusMsg(orcaqgui::Warning,ss.str().c_str());
            }
            catch ( char *e )
            {
                ss<<"TRACE(guielementmodel.cpp): Caught char * during update of "
                   <<elements_[i]->details().toStdString()<<": " << e << std::endl;
                humanManager_->showStatusMsg(orcaqgui::Warning,ss.str().c_str());
            }
            catch ( ... )
            {
                ss<<"TRACE(guielementmodel.cppp): Caught some other exception during update of "
                   <<elements_[i]->details().toStdString()<<": " << std::endl;
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
    coordinateFramePlatform_ = elements_[guiElementIndex]->platform();
}

void
GuiElementModel::executeGuiElement( int guiElementIndex, int actionIndex )
{

    if ( guiElementIndex<0 || guiElementIndex >= elements_.size() || elements_[guiElementIndex]==NULL ) return;
    // debug
    cout<<"GuiElementModel:executeGuiElement: executing action #"<<actionIndex<<endl;

    elements_[guiElementIndex]->execute( actionIndex );
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

void 
GuiElementModel::setTransparency(bool transparency)
{
    cout << "TRACE(guielementmodel.cpp): setTransparency to " << transparency << endl;
    currentTransparency_ = transparency;
    
    for ( int i=0; i<elements_.size(); ++i )
    {
        elements_[i]->setTransparency( transparency );
    }    
}

}




