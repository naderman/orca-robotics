/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <hydroutil/sysutils.h>

#include "guielementmodel.h"
#include "guielementview.h"

using namespace std;

namespace orcaqgemv {

GuiElementModel::GuiElementModel(   const std::vector<hydroqgui::IGuiElementFactory*> &factories,
                                    hydroqguielementutil::IHumanManager               &humanManager,
                                    hydroqguielementutil::MouseEventManager           &mouseEventManager,
                                    hydroqguielementutil::ShortcutKeyManager          &shortcutKeyManager,
                                    hydroqgui::CoordinateFrameManager                 &coordinateFrameManager,
                                    hydroqgui::PlatformFocusManager                   &platformFocusManager,
                                    hydroqgui::GuiElementSet                          &guiElementSet,
                                    hydroqgui::IStringToColorMap                      &platformColorMap,
                                    QObject                                           *parent )
    : QAbstractTableModel(parent),
      PlatformFocusChangeReceiver(platformFocusManager),
      guiElementSet_(guiElementSet),
      factories_(factories),
      humanManager_(humanManager),
      mouseEventManager_(mouseEventManager),
      shortcutKeyManager_(shortcutKeyManager),
      coordinateFrameManager_(coordinateFrameManager),
      platformFocusManager_(platformFocusManager),
      platformColorMap_(platformColorMap),
      view_(0)
{    
    headers_ << "Type" << "Details";
//     coordinateFramePlatform_ = "global";
//     ignoreCoordinateFrameRotation_ = false;
//    platformInFocus_ = "global";
}

int GuiElementModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return elements().size();
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
            QString fqIName = elements()[idx.row()]->type();
            return fqIName;
        }
        case 1 :
            return elements()[idx.row()]->details();
        }
    }  
    else if ( role == InterfaceIdRole ) {
        // for all columns
        return elements()[idx.row()]->details();
    }  
    else if ( role == ContextMenuRole ) {
        // for all columns
        if ( elements()[idx.row()]==0 ) {
            return QVariant();
        }
        return elements()[idx.row()]->contextMenu();
    }
    else if ( role == FocusRole ) {
        // for all columns
        // if it's in Global CS we can set the coordinate frame to it.
        return elements()[idx.row()]->isInGlobalCS();
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
    QString platform = elements()[row]->platform();
    
    assert( row>-1 && row<elements().size() && "row number must exist" );
    // too lazy to implement multi-row removal
    assert( count==1 && "only removal of one row at a time is implemented" );
    assert( parent==QModelIndex() && "this is a table model, parent must be root" );

    // if it's the mouse-event-receiver, reset the mouse-event-receiver
    if ( elements()[row] == mouseEventManager_.mouseEventReceiver() )
        mouseEventManager_.mouseEventReceiver()->noLongerMouseEventReceiver();

    beginRemoveRows( QModelIndex(), row, row );
    guiElementSet_.removeGuiElement( row );
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
GuiElementModel::instantiateFromFactories( hydroqguielementutil::IGuiElement* &element,
                                           const QString                      &elementType,
                                           const QColor                       &platformColor,
                                           const QStringList                  &elementDetails )
{
    for ( unsigned int i=0; i < factories_.size(); i++ )
    {
        // if this interface is not supported, skip this factory
        if ( !factories_[i]->isSupported( elementType ) )
            continue;
        
        // if we get here the interface is supported
        try 
        {
            element = factories_[i]->create( elementType, elementDetails, platformColor, humanManager_, mouseEventManager_, shortcutKeyManager_, guiElementSet_ );
        } 
        catch (gbxsickacfr::gbxutilacfr::Exception &e)
        {
            stringstream ss;
            ss << "GuiElementModel: Problem when trying to create element of type " << elementType.toStdString() <<": " << e.what() << std::endl;
            humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Error,ss.str().c_str());
        }
        return true; 
    }
    return false;
}

bool
GuiElementModel::doesElementExist( const QStringList& elementDetails, int numElements )
{
    //cout << "proxyStrList.join " << proxyStrList.join(" ").toStdString() << endl;
    
    for ( int i=0; i<elements().size(); i++)
    {
        cout << "element name: " << elements()[i]->details().toStdString() << endl;
        if ( elements()[i]->details() == elementDetails.join(" ") + " " ) return true;
    }
    
    return false;     
}

void 
GuiElementModel::createGuiElementFromSelection( const QList<QStringList> & interfacesInfo )
{    
    // get interface data out of stringlist
    QStringList ids;
    QStringList elementDetails;
    for (int i=0; i<interfacesInfo.size(); i++)
    {
        QStringList info = interfacesInfo[i]; 
        ids << info[4];
        //cout << "ids: " << info[3].toStdString() << endl;
        elementDetails << info[3]+"@"+info[1]+"/"+info[2];
        //cout << "proxylist: " << (info[3]+"@"+info[1]+"/"+info[2]).toStdString() << endl;
    }
    
    //TOBI: this needs to be fixed for config files
    bool haveThisElement = doesElementExist( elementDetails, interfacesInfo.size() );
    if (haveThisElement) {
        humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Warning,"Interface " + elementDetails.join(" ") + " exists already. Not connecting again!");
        return;
    }
    
    QString elementType = lookupTypeFromFactories( ids );
    if (elementType!="") {
        createGuiElement( elementType, elementDetails );
    } else {
        humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Warning,"Looking up element type from factory resulted in nothing");
    }
}

QString 
GuiElementModel::lookupTypeFromFactories( QStringList &ids )
{
    QString elementType = "";
    for ( unsigned int i=0; i < factories_.size(); i++ )
    {
        if ( !factories_[i]->lookupElementType( ids, elementType ) )
            continue; 
    } 
    return elementType;
}

void 
GuiElementModel::determinePlatform( QStringList &elementDetails,
                                    QString     &platform )
{
    QStringList platformStrList;
    for (int i=0; i<elementDetails.size(); i++)
    {
        QString str = elementDetails[i]; 
        str = str.section('@',1,1);
        str = str.section('/',0,0);
        // replace local with our host name
        if (str=="local") {
            str = QString(hydroutil::getHostname().c_str());
            elementDetails[i].replace("@local", "@"+str);
        }
        platformStrList << str;
    }
    platform = platformStrList[0];
    
    // if we have several interfaces and they disagree on the platform,
    // we set it to global
    for (int i=1; i<platformStrList.size(); i++)
    {   
        if (platformStrList[i] != platform)
            platform="global";
    }
    
    // tell everybody we got a new platform
    if ( !doesPlatformExist( platform ) )
    {
        emit ( newPlatform(platform) );
    }
}
    
hydroqguielementutil::IGuiElement*
GuiElementModel::createGuiElement( const QString &elementType, 
                                   QStringList   &elementDetails )
{    
    QString platform;
    determinePlatform( elementDetails, platform );
    QColor platformColor = platformColorMap_.getColor( platform );
    
    // instantiate element
    hydroqguielementutil::IGuiElement* element = NULL;
    bool isSupported = instantiateFromFactories( element, elementType, platformColor, elementDetails );
    if (!isSupported || element==NULL)
    {
        if (!isSupported) 
            humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Warning, "Element type " + elementType + " is not supported by any factory.");
        else if (element==NULL) 
            humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Warning, "Element " + elementDetails.join(" ") + " is supported but the factory returned NULL pointer.");
        delete element;
        if (!doesPlatformExist( platform ) ) 
            emit platformNeedsRemoval(platform);
        return NULL;   
    }
    
    // set properties of guielement
    element->setPlatform( platform );
    QString details = "";
    for (int i=0; i<elementDetails.size(); i++)
    {
        details = details + elementDetails[i] + " ";
    }
    element->setDetails( details );
    element->setName(elementType);
    
    //
    // We need to tell the new element whether it's in focus or not
    //
    const bool isInFocus =  ( platformFocusManager_.platformInFocus() == platform ||
                              platformFocusManager_.platformInFocus() == "global" );
    element->setFocus( isInFocus );
        
    int ii = elements().indexOf( element );
    if ( ii==-1 ) {    
        ii = elements().size();
        //cout<<"TRACE(guielementmodel.cpp): creating element "<<ii<<endl;

        //
        // stick new node into the list
        //
        beginInsertRows( QModelIndex(), ii,ii );
        guiElementSet_.addGuiElement( element );
        endInsertRows();
        
        // hide if not in focus
        if ( view_ && !isInFocus )
            view_->hideRow( ii );
        
    }
    // resize the columns so we can read the text
    if ( view_ )
        view_->resizeColumnsToContents();

    return element;
}

void
GuiElementModel::removeAndDeleteGuiElement( hydroqguielementutil::IGuiElement *guiElement )
{
    int row=-1;
    for ( int i=0; i < elements().size(); i++ )
    {
        if ( elements()[i] == guiElement )
        {
            row = i;
            break;
        }
    }
    
    if ( row == -1 )
    {
        throw hydroqgui::Exception( ERROR_INFO, "Tried to deleteGuiElement for non-existent element." );
    }

    removeRows( row, 1 );
}

bool
GuiElementModel::doesPlatformExist( QString &platformName )
{
    for ( int i=0; i<elements().size(); ++i )
    {
        if ( elements()[i]->platform() == platformName ) return true;
    }
    return false;    
}


void 
GuiElementModel::platformFocusChanged( const QString &newPlatformName )
{
    for ( int i=0; i<elements().size(); ++i )
    {
        // special case: show everybody with their colours
        if (newPlatformName=="global") 
        {
            elements()[i]->setFocus(true);
        }
        else
        {
            const bool isInFocus = (elements()[i]->platform() == newPlatformName);
            elements()[i]->setFocus( isInFocus );
        }
    }
    
    // tell the guielementview to filter what it shows
    // collect indices
    if ( newPlatformName=="global" && view_ ) {
        view_->showAllElements( elements().size() );
        return;
    }
    
    vector<int> elementIndices;
    for ( int i=0; i<elements().size(); ++i )
    {
        if (elements()[i]->platform() != newPlatformName)
            elementIndices.push_back(i);
    }
    if ( view_ )
        view_->hideElements( elements().size(), elementIndices );
}

void
GuiElementModel::removeAllGuiElements()
{
    QVector<int> indeces;
    
    // compile a list of indeces
    for ( int i=0; i<elements().size(); ++i )
    {
        // don't delete the permanent elements e.g. grid
        if ( !elements()[i]->isPermanentElement() )
        {
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
    for ( int i=0; i<elements().size(); ++i )
    {
        if ( elements()[i] ) {
            std::stringstream ss;
            try {
                elements()[i]->update();
            }
            catch ( std::exception &e )
            {
                ss<<"GuiElementModel: during update of "
                <<elements()[i]->details().toStdString()<<": " << e.what() << std::endl;
                humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Warning,ss.str().c_str());
            }
            catch ( std::string &e )
            {
                ss<<"GuiElementModel: during update of "
                   <<elements()[i]->details().toStdString()<<": " << e << std::endl;
                humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Warning,ss.str().c_str());
            }
            catch ( char *e )
            {
                ss<<"GuiElementModel: during update of "
                   <<elements()[i]->details().toStdString()<<": " << e << std::endl;
                humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Warning,ss.str().c_str());
            }
            catch ( ... )
            {
                ss<<"GuiElementModel: Caught unknown exception during update of "
                   <<elements()[i]->details().toStdString()<<": " << std::endl;
                humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Warning,ss.str().c_str());
            }
        }
    }
}

void
GuiElementModel::setCoordinateFramePlatform( int guiElementIndex )
{
    assert ( guiElementIndex>=0 && guiElementIndex < elements().size() );
    
    // by convention, Grid's platform is "global"
    coordinateFrameManager_.setCoordinateFramePlatform( elements()[guiElementIndex]->platform() );
}

void
GuiElementModel::setOriginPlatform( int guiElementIndex )
{
    assert ( guiElementIndex>=0 && guiElementIndex < elements().size() );

    // by convention, Grid's platform is "global"
    coordinateFrameManager_.setOriginPlatform( elements()[guiElementIndex]->platform() );
}

bool 
GuiElementModel::isElementRemovable( int guiElementIndex )
{
    assert ( guiElementIndex>=0 && guiElementIndex < elements().size() );
    return !elements()[guiElementIndex]->isPermanentElement();
}

void
GuiElementModel::executeGuiElement( int guiElementIndex, int actionIndex )
{
    assert ( guiElementIndex>=0 && guiElementIndex < elements().size() );
    // debug
    // cout<<"GuiElementModel:executeGuiElement: executing action #"<<actionIndex<<endl;

    elements()[guiElementIndex]->execute( actionIndex );
}

void 
GuiElementModel::setView( GuiElementView* view )
{ 
    view_ = view;
}

void
GuiElementModel::selectedAdaptersInView( vector<int> &indices )
{
    if ( view_ )
        view_->selectedAdaptersInView( elements().size(), indices );
}

}




