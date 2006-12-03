/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <cmath>        // for floor()

#include "ocmmodel.h"

#include "ocmicons.h"

#include <QApplication>
#include <QStyle>

using namespace std;

namespace orcaqcm
{

OcmIconProvider::OcmIconProvider()
{
    QStyle *style = QApplication::style();

    registry = QIcon(style->standardPixmap(QStyle::SP_FileIcon));
    platform = QIcon(QPixmap(ocm_platform_xpm));
    component = QIcon(QPixmap(ocm_component_xpm));
    provided = QIcon(QPixmap(ocm_provided_xpm));
    required = QIcon(QPixmap(ocm_required_xpm));
}

QIcon
OcmIconProvider::icon(IconType type) const
{
    switch (type) {
    case Registry:
        return registry;
    case Platform:
        return platform;
    case Component:
        return component;
    case Provided:
        return provided;
    case Required:
        return required;
    default:
        break;
    };
    return QIcon();
}


OcmModel::ComponentNode::ComponentNode( const QString &n, PlatformNode* p, const QString &a, bool connected, int tsec )
    : name(n), platform(p), address(a), isConnected(connected) 
{
    daysUp = (int)floor( (double)tsec / (24.0*60.0*60.0) );
    QTime t;
    timeUp = t.addSecs( tsec - daysUp*24*60*60 );
}

OcmModel::OcmModel( QObject *par )
    : QAbstractItemModel(par)
{
    headers_ << "Name" << "Details";
}

OcmModel::~OcmModel()
{
}

QModelIndex
OcmModel::index(int row, int column, const QModelIndex &parent) const
{
//cout<<"index: looking for "<<row<<","<<column<<endl;
//cout<<"row count : "<<rowCount(parent)<<endl;
    if ( row<0 || row>=rowCount(parent) || column<0 || column>=columnCount(parent)) {
        //cout<<"index: invalid range "<<row<<","<<column<<endl;
        return QModelIndex();
    }
    
    Node *n = static_cast<Node*>(parent.internalPointer());
    
    if ( n==0 ) { // root
        //cout<<"index: looking for "<<row<<","<<column<<" of root"<<endl;
        return createIndex( row, column, (void*)&(registries_.at(row)) );
    }
   
    switch ( n->type() )
    {
    case RegistryType :
    {
        RegistryNode* N = (RegistryNode*)n;
        //cout<<"index: looking for "<<row<<","<<column<<" of "<<rn->name.toStdString()<<endl;
        return createIndex( row, column, (void*)&(N->platforms.at(row)) );
    }
    case PlatformType :
    {
        PlatformNode* N = (PlatformNode*)n;
        //cout<<"index: looking for "<<row<<","<<column<<" of "<<pn->name.toStdString()<<endl;
        return createIndex( row, column, (void*)&(N->components.at(row)) );
    }
    case ComponentType :
    {
        ComponentNode* N = (ComponentNode*)n;
        //cout<<"index: looking for "<<row<<","<<column<<" of "<<cn->name.toStdString()<<endl;
        return createIndex( row, column, (void*)&(N->interfaces.at(row)) );
    }
    } // type
    return QModelIndex();
}

QModelIndex
OcmModel::parent(const QModelIndex &child) const
{
//cout<<"OcmModel::parent: r,c="<<child.row()<<","<<child.column()<<endl;
    if (!child.isValid()) {
        //cout<<"invalid child"<<endl;
        return QModelIndex();
    }
    
    Node *n = static_cast<Node*>(child.internalPointer());
    if ( !n ) {
        cout<<"r,c="<<child.row()<<","<<child.column()<<endl;
        Q_ASSERT(n);
    }
    //cout<<"parent: looking for parent of type '"<<n->type()<<"'"<<endl;
    
    switch ( n->type() )
    {
    case RegistryType :
    {
        // parent is the root
        return QModelIndex();
    }
    case PlatformType :
    {
        // parent is a registry
        PlatformNode* me = (PlatformNode*)n;        // me
        RegistryNode* pa = me->registry;            // parent
        return createIndex( registries_.indexOf(*pa), 0, pa );
    }
    case ComponentType :
    {
        // parent is a platform
        ComponentNode* me = (ComponentNode*)n;      // me
        PlatformNode* pa = me->platform;            // parent
        RegistryNode* gp = pa->registry;            // grandparent
        return createIndex( gp->platforms.indexOf(*pa), 0, pa );
    }
    case InterfaceType :
    {
        // parent is a component
        InterfaceNode* me = (InterfaceNode*)n;      // me
        ComponentNode* pa = me->component;          // parent
        PlatformNode* gp = pa->platform;            // grandparent
        return createIndex( gp->components.indexOf(*pa), 0, pa );
    }
    } // end switch
    //cout<<"warning: unknown node type in parent()"<<endl;
    return QModelIndex();
}

bool
OcmModel::hasChildren(const QModelIndex &parent) const
{
    Node *n = static_cast<Node*>(parent.internalPointer());
    if ( !n ) { // root
        //cout<<"hasChildren: found children of root"<<endl;
        return !registries_.isEmpty();
    }

    switch ( n->type() )
    {
    case RegistryType :
    {
        RegistryNode* N = (RegistryNode*)n;
        //cout<<"hasChildren: for "<<pn->name.toStdString()<<": "<<!pn->components.isEmpty()<<endl;
        return !N->platforms.isEmpty();
    }
    case PlatformType :
    {
        PlatformNode* N = (PlatformNode*)n;
        //cout<<"hasChildren: for "<<pn->name.toStdString()<<": "<<!pn->components.isEmpty()<<endl;
        return !N->components.isEmpty();
    }
    case ComponentType :
    {
        ComponentNode* N = (ComponentNode*)n;
        //cout<<"hasChildren: for "<<cn->name.toStdString()<<": "<<!cn->interfaces.isEmpty()<<endl;
        return !N->interfaces.isEmpty();
    }
    case InterfaceType :
    {
        // no children
        return false;
    }
    } // switch
    
    //cout<<"warning: unknown node type in hasChildren()"<<endl;
    return false;
}

int
OcmModel::rowCount(const QModelIndex &parent) const
{
    Node *n = static_cast<Node*>(parent.internalPointer());
    if ( !n ) { // root
        //cout<<"rowCount: found children of root"<<endl;
        return registries_.size();
    }

    switch ( n->type() )
    {
    case RegistryType :
    {
        RegistryNode* N = (RegistryNode*)n;
        return N->platforms.size();
    }
    case PlatformType :
    {
        PlatformNode* N = (PlatformNode*)n;
        return N->components.size();
    }
    case ComponentType :
    {
        ComponentNode* N = (ComponentNode*)n;
        return N->interfaces.size();
    }
    case InterfaceType :
    {
        // no children
        return 0;
    }
    } // switch
    
    //cout<<"warning: unknown node type in rowSize()"<<endl;
    return 0;
}

int
OcmModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return headers_.size();
}

QVariant
OcmModel::data(const QModelIndex &idx, int role) const
{
    if (!idx.isValid()) {
        return QVariant();
    }
    if (idx.row()>rowCount( idx.parent() ) && idx.row()<0
                && idx.column()>columnCount( idx.parent() ) && idx.column()<0) {
        //cout<<"data: didn't pass test "<<idx.row()<<","<<idx.column()<<endl;
        return QVariant();
    }

    Node *n = static_cast<Node*>(idx.internalPointer());
    //cout<<"data: node type "<<n->type()<<" (r,c)=("<<idx.row()<<","<<idx.column()<<")"<<endl;

    if (role == Qt::DisplayRole) {
        switch ( n->type() )
        {        
        case RegistryType :
        {
            //cout<<"data: registry type"<<endl;
            RegistryNode* N = (RegistryNode*)n;
            switch ( idx.column() ) {
            case 0 :
                return N->name;
            case 1 :
                return N->address;
            case 2 :
            case 3 :
                return QVariant();
            }
        }
        case PlatformType :
        {
            //cout<<"data: platform type"<<endl;
            PlatformNode* N = (PlatformNode*)n;
            switch ( idx.column() ) {
            case 0 :
            {
                return N->name;
                /*
                QString s = pn->name;
                if ( pn->isConnected && pn->components.isEmpty() ) {
                    s += " (0)";
                }
                else if ( pn->isConnected ) {
                    s += " (" + QString::number(pn->components.size()) + ")";
                }
                return s;
                */
            }
            case 1 :
            case 2 :
            case 3 :
                return QVariant();
            }
        }
        case ComponentType :
        {
            ComponentNode* N = (ComponentNode*)n;
            switch ( idx.column() ) {
            case 0 :
                return N->name;
                /*
                if ( cn->messages.isEmpty() ) {
                    return cn->name + " (0)";
                }
                else {
                    return cn->name + " (" + QString::number(cn->messages.size()) + ")";
                }
                */
            case 1 :
                return "up:"+QString::number(N->daysUp) + ":" + N->timeUp.toString( "HH:mm:ss" ) + ", addr:"+N->address;
            case 2 :
            case 3 :
                return QVariant();
            }
        }        
        case InterfaceType :
        {
            InterfaceNode* N = (InterfaceNode*)n;
            switch ( idx.column() ) {
            case 0 :
                return N->name;
            case 1 :
                return N->ids;
            case 2 :
            case 3 :
                return QVariant();
            }
        }
        } // type
    }
    else if ( role == Qt::DecorationRole && idx.column()==0 ) {        
        switch ( n->type() )
        {
        case RegistryType :
            return iconProvider_.icon( OcmIconProvider::Registry );
        case PlatformType :
            return iconProvider_.icon( OcmIconProvider::Platform );
        case ComponentType :
            return iconProvider_.icon( OcmIconProvider::Component );
        case InterfaceType :
        {
            InterfaceNode* N = (InterfaceNode*)n;
            if ( N->subtype == ProvidedInterfaceSubtype ) {
                return iconProvider_.icon( OcmIconProvider::Provided );
            }
            else if ( N->subtype == RequiredInterfaceSubtype ) {
                return iconProvider_.icon( OcmIconProvider::Required );
            }
        }
        } // type
        //cout<<"warning: unknown node type in data() : "<<n->type()<<" (r,c)=("<<idx.row()<<","<<idx.column()<<")"<<endl;
    } // role
    else if ( role == TypeRole ) {
        // regardless of the column
        switch ( n->type() )
        {
        case RegistryType :
            return "Registry";
        case PlatformType :
            return "Platform";
        case ComponentType :
            return "Component";
        case InterfaceType :
            return "Interface";
        default :
            return QVariant();
        }
    } // role
    else if ( role == ConnectionRole ) {
        switch ( n->type() )
        {
        case RegistryType :
        {
            RegistryNode* N = (RegistryNode*)n;
            // regardless of the column
            return int(N->isConnected);
        }        
        case PlatformType :
        {
            PlatformNode* N = (PlatformNode*)n;
            // regardless of the column
            for ( unsigned int i=0; i<N->components.size(); ++i ) {
                if ( N->components[i].isConnected ) {
                    return 1;
                }
            }
            return 0;
        }        
        case ComponentType :
        {
            ComponentNode* N = (ComponentNode*)n;
            // regardless of the column
            return int(N->isConnected);
        }        
        case InterfaceType :
        {
            InterfaceNode* N = (InterfaceNode*)n;
            // regardless of the column
            return ( int(N->component->isConnected) && int(N->isConnected) );
        }
        default :
            return 1;
        }
    } // role
    
    return QVariant();
}

QVariant
OcmModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal ) {
        return QVariant();
    }
    return headers_.at( section );
}

// Returns the model index of the registry with specified name.
QModelIndex
OcmModel::setRegistryPrivate( const QString &registry, const QString & regAddress,
                bool connected )
{
    // find our registry or make a new one
    RegistryNode rn( registry, regAddress, connected );
    int ri = registries_.indexOf( rn );
    if ( ri==-1 ) {
        // the registry is not in the model yet, so create a new one
        ri = registries_.size();

        //cout<<endl<<"about to insert registry "<<registry.toStdString()<<" at "<<ri<<endl;
        beginInsertRows( QModelIndex(), ri,ri );
        registries_.append( rn );
        endInsertRows();
        
        // @todo sort the list alphabetically, not easy because it will break parent links
        registries_[ri].isConnected = connected;
        //cout<<"created a platform node "<<pi<<endl;
    }
    else {
        //cout<<"found existing registry "<<ri<<" "<<name.toStdString()<<endl;
        //registries_[ri].isConnected = connected;
        //emit dataChanged( index(ri,0,QModelIndex()), index(ri,0,QModelIndex()) );
    }
    return index(ri,0,QModelIndex());
}

// Returns the index of the platform with specified name.
QModelIndex
OcmModel::setPlatformPrivate( const QString & registry, const QString & regAddress,
                const QString & platform )
{
    // this will find our registry or make a new one
    QModelIndex rindex = setRegistryPrivate( registry, regAddress, true );
    RegistryNode* r = static_cast<RegistryNode*>(rindex.internalPointer());
    
    // find our platform or make a new one
    PlatformNode pn( platform, r );
    int pi = r->platforms.indexOf( pn );
    if ( pi==-1 ) {
        // the platform is not in the model yet, so create a new one
        pi = r->platforms.size();

        //cout<<endl<<"about to insert platform "<<platform.toStdString()<<" at "<<pi<<endl;
        beginInsertRows( rindex, pi,pi );
        r->platforms.append( pn );
        endInsertRows();
        
        // @todo sort the list alphabetically, not easy because it will break parent links
        //cout<<"created a platform node "<<pi<<endl;
    }
    else {
        //cout<<"found existing platform "<<pi<<" "<<name.toStdString()<<endl;
        //emit dataChanged(index( pi,0,rindex ), index( pi,columnCount(QModelIndex()),rindex ));
    }
    return index(pi,0,rindex);
}

QModelIndex
OcmModel::setComponentPrivate( const QString & registry, const QString & regAddress,
                    const QString & platform, const QString & component,
                    const QString & compAddress, bool connected, int timeUp )
{
    // this will find our platform or make a new one
    QModelIndex pindex = setPlatformPrivate( registry, regAddress, platform );
    PlatformNode* p = static_cast<PlatformNode*>(pindex.internalPointer());
    
    // find our component or make a new one
    ComponentNode cn( component, p, compAddress, connected, timeUp );
    int ci = p->components.indexOf( cn );
    if ( ci==-1 ) {
        ci = p->components.size();
    
        //cout<<endl<<"about to insert component "<<component.toStdString()<<" at "<<ci<<endl;
        beginInsertRows( pindex, ci,ci );
        p->components.append( cn );
        endInsertRows();
        
        //! @todo sort the list alphabetically, not easy because it will break parent links
        //cout<<"created a component node "<<ci<<" at platform "<<pi<<endl;        
    }
    else {
        // don't create new one, just update attributes
        //cout<<"using component node "<<ci<<" at platform "<<pi<<endl;
        p->components[ci].isConnected = connected;
        p->components[ci].daysUp = (int)floor( (double)timeUp / (24.0*60.0*60.0) );
        QTime t;
        p->components[ci].timeUp = t.addSecs( timeUp - p->components[ci].daysUp*24*60*60 );
        
        emit dataChanged(index( ci,0,pindex ), index( ci,columnCount(QModelIndex()),pindex ));
    }
    return index(ci,0,pindex);
}

QModelIndex
OcmModel::setInterfacePrivate( const QString & registry, const QString & regAddress,
                                const QString & platform, const QString & component, const QString & interface,
                                const bool isProvided, const QString & address,
                                const QString & ids, bool connected, int timeUp )
{
    // this will find our platform and component or make new ones
    QModelIndex cindex = setComponentPrivate( registry, regAddress, platform, component, address, true, timeUp );
    ComponentNode* c = static_cast<ComponentNode*>(cindex.internalPointer());    
    
    // find our interface or make a new one
    InterfaceNode in( interface, c );
    if ( isProvided ) {
        in.subtype = ProvidedInterfaceSubtype;
    }
    else {
        in.subtype = RequiredInterfaceSubtype;
    }
    in.ids = ids;
    in.isConnected = connected;
    
    int ii = c->interfaces.indexOf( in );
    if ( ii==-1 ) {    
        ii = c->interfaces.size();
        
        beginInsertRows( cindex, ii,ii );
        c->interfaces.append( in );
        endInsertRows();
        
        // @todo sort the list alphabetically, not easy because it will break parent links
        //cout<<"created an interface node "<<ii<<" for component "<<c->name.toStdString()<<
        //        " ("<<c->interfaces.size()<<")"<<endl;                
    }
    else {
        //cout<<"using interface node "<<ii<<" for component "<<c->name.toStdString()<<endl;
    }
    return index(ii,0,cindex);
}


int
OcmModel::getInterface( const QModelIndex& ind,
                    QString & registry, QString & platform, QString & component,
                    QString & interface, QString & id )
{
    Node* n = static_cast<Node*>(ind.internalPointer());
    if ( n->type() != InterfaceType ) {
        return 1;
    }

    InterfaceNode* in = (InterfaceNode*)n;
    ComponentNode* cn = in->component;
    PlatformNode* pn = cn->platform;
    RegistryNode* rn = pn->registry;

    registry = rn->name;
    platform = pn->name;
    component = cn->name;
    interface = in->name;
    id = in->ids;
    return 0;
}

void
OcmModel::clear()
{
    if ( !hasChildren(QModelIndex()) ) {
        return;
    }
    
    beginRemoveRows( QModelIndex(), 0, rowCount(QModelIndex()) );
    registries_.clear();
    endRemoveRows();
}

} // namespace
