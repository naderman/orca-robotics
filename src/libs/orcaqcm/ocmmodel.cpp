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

    registry  = QIcon(style->standardPixmap(QStyle::SP_FileIcon));
    platform  = QIcon(QPixmap(ocm_platform_xpm));
    component = QIcon(QPixmap(ocm_component_xpm));
    provided  = QIcon(QPixmap(ocm_provided_xpm));
    required  = QIcon(QPixmap(ocm_required_xpm));
    operation = QIcon(QPixmap(ocm_operation_xpm));
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
    case Operation:
        return operation;
    case Result:
        return result;
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
    case InterfaceType :
    {
        InterfaceNode* N = (InterfaceNode*)n;
        //cout<<"index: looking for "<<row<<","<<column<<" of "<<cn->name.toStdString()<<endl;
        return createIndex( row, column, (void*)&(N->operations.at(row)) );
    }
    case OperationType :
    {
        OperationNode* N = (OperationNode*)n;
        return createIndex( row, column, (void*)&(N->results.at(row)) );
    }
    case ResultType :
    {
        cout<<"*********** index() for ResultType should not be called! *********"<<endl;
        return QModelIndex();
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
    case OperationType :
    {
        // parent is an interface
        OperationNode* me = (OperationNode*)n;      // me
        InterfaceNode* pa = me->interface;          // parent
        ComponentNode* gp = pa->component;          // grandparent
        return createIndex( gp->interfaces.indexOf(*pa), 0, pa );
    }
    case ResultType :
    {
        // parent is an interface
        ResultNode* me = (ResultNode*)n;            // me
        OperationNode* pa = me->operation;          // parent
        InterfaceNode* gp = pa->interface;          // grandparent
        return createIndex( gp->operations.indexOf(*pa), 0, pa );
    }
    } // end switch
    cout<<"warning: unknown node type in parent()"<<endl;
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
        InterfaceNode* N = (InterfaceNode*)n;
        //cout<<"hasChildren: for "<<cn->name.toStdString()<<": "<<!cn->interfaces.isEmpty()<<endl;
        return !N->operations.isEmpty();
    }
    case OperationType :
    {
        OperationNode* N = (OperationNode*)n;
        //cout<<"hasChildren: for "<<cn->name.toStdString()<<": "<<!cn->interfaces.isEmpty()<<endl;
        return !N->results.isEmpty();
    }
    case ResultType :
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
        InterfaceNode* N = (InterfaceNode*)n;
        return N->operations.size();
    }
    case OperationType :
    {
        OperationNode* N = (OperationNode*)n;
        return N->results.size();
    }
    case ResultType :
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
        case OperationType :
        {
            OperationNode* N = (OperationNode*)n;
            switch ( idx.column() ) {
            case 0 :
                return N->name;
            case 1 :
            case 2 :
            case 3 :
                return QVariant();
            }
        }
        case ResultType :
        {
            ResultNode* N = (ResultNode*)n;
            switch ( idx.column() ) {
            case 0 :
                return N->result;
            case 1 :
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
        case OperationType :
            return iconProvider_.icon( OcmIconProvider::Operation );
        case ResultType :
            return iconProvider_.icon( OcmIconProvider::Result );
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
        case OperationType :
            return "Operation";
        case ResultType :
            return "Result";
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
            for ( int i=0; i<N->components.size(); ++i ) {
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
        case OperationType :
        {
            OperationNode* N = (OperationNode*)n;
            // regardless of the column
            return ( int(N->interface->isConnected));
        }
        case ResultType :
        {
            return true;
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

QModelIndex 
OcmModel::registryIndex( const QString & registry, bool create )
{
    RegistryNode rn(registry);
    int ri = registries_.indexOf( rn );

    if ( ri==-1 && create ) {
        // the registry is not in the model yet, so create a new one
        ri = registries_.size();

        //cout<<endl<<"about to insert registry "<<registry.toStdString()<<" at "<<ri<<endl;
        beginInsertRows( QModelIndex(), ri,ri );
        registries_.append( rn );
        endInsertRows();
        cout<<"DEBUG: created registry "<<registry.toStdString()<<endl;
    }
    return index( ri, 0, QModelIndex() );
}

// Returns the model index of the registry with specified name.
void
OcmModel::setRegistry( const QString &registry, const QString & regAddress, bool connected )
{
    QModelIndex ind = registryIndex( registry, true );

    if ( ind.isValid() ) 
    {
        RegistryNode* node = static_cast<RegistryNode*>(ind.internalPointer());
        node->address       = regAddress;
        node->isConnected   = connected;
        emit dataChanged( ind,ind );
    }
}

QModelIndex 
OcmModel::platformIndex( const QString & registry, const QString &platform, bool create )
{
    QModelIndex rindex = registryIndex( registry, false );
    // failed to find
    if ( !rindex.isValid() ) {
        cout<<"failed to find registry "<<registry.toStdString()<<endl;
        return QModelIndex();
    }
    RegistryNode* rnode = static_cast<RegistryNode*>(rindex.internalPointer());
    
    PlatformNode pnode( platform, rnode );
    int pi = rnode->platforms.indexOf( pnode );

    if ( pi==-1 && create ) {
        // the platform is not in the model yet, so create a new one
        pi = rnode->platforms.size();

        //cout<<endl<<"about to insert platform "<<platform.toStdString()<<" at "<<pi<<endl;
        beginInsertRows( rindex, pi,pi );
        rnode->platforms.append( pnode );
        endInsertRows();
        cout<<"DEBUG: created platform "<<platform.toStdString()<<endl;
        
        // @todo sort the list alphabetically, not easy because it will break parent links
        //cout<<"created a platform node "<<pi<<endl;
    }
    return index( pi , 0, rindex );
}

void
OcmModel::setPlatform( const QString & registry,
                const QString & platform )
{
    QModelIndex ind = platformIndex( registry, platform, true );

//     if ( ind.isValid() ) 
//     {
//         PlatformNode* node = static_cast<PlatformNode*>(ind.internalPointer());
//         node->isConnected = connected;
//         emit dataChanged( ind,ind );
//     }
}
  
QModelIndex 
OcmModel::componentIndex( const QString & registry, const QString & platform, 
                    const QString & component, bool create )
{
    // special case: can create platform because we have all the info
    QModelIndex pindex = platformIndex( registry, platform, true );
    // failed to find
    if ( !pindex.isValid() ) {
        cout<<"DEBUG: failed to find platform "<<platform.toStdString()<<endl;
        return QModelIndex();
    }
    PlatformNode* pnode = static_cast<PlatformNode*>(pindex.internalPointer());
    
    ComponentNode cnode( component, pnode );
    int ci = pnode->components.indexOf( cnode );

    if ( ci==-1 && create ) {
        ci = pnode->components.size();
    
        //cout<<endl<<"about to insert component "<<component.toStdString()<<" at "<<ci<<endl;
        beginInsertRows( pindex, ci,ci );
        pnode->components.append( cnode );
        endInsertRows();
        cout<<"DEBUG: created component "<<component.toStdString()<<endl;
        
//         @todo sort the list alphabetically, not easy because it will break parent links      
    }
    // this will return an invalid index if the platform is not found
    return index( ci , 0, pindex );
}

void
OcmModel::setComponent( const QString & registry, const QString & platform, 
            const QString & component, const QString & compAddress, bool connected, int timeUp )
{
//     cout<<"adding reg="<<registry.toStdString()<<" ptf="<<platform.toStdString()
//         <<" cmp="<<component.toStdString()<<" caddr="<<compAddress.toStdString()
//         <<" con="<<connected<<" time="<<timeUp<<endl;

    // this will find our platform
    QModelIndex ind = componentIndex( registry, platform, component, true );

    if ( ind.isValid() ) {
        ComponentNode* node = static_cast<ComponentNode*>(ind.internalPointer());
        node->isConnected = connected;
        node->daysUp = (int)floor( (double)timeUp / (24.0*60.0*60.0) );
        QTime t;
        node->timeUp = t.addSecs( timeUp - node->daysUp*24*60*60 );
        emit dataChanged( ind,ind );
    }
}

QModelIndex 
OcmModel::interfaceIndex( const QString & registry, const QString & platform, const QString & component,
                    const QString & interface, bool create )
{
    QModelIndex cindex = componentIndex( registry, platform, component );
    // failed to find
    if ( !cindex.isValid() ) {
        cout<<"DEBUG: failed to find component "<<component.toStdString()<<endl;
        return QModelIndex();
    }
    ComponentNode* cnode = static_cast<ComponentNode*>(cindex.internalPointer());    
    
    // find our interface or make a new one
    InterfaceNode inode( interface, cnode );
    int ii = cnode->interfaces.indexOf( inode );

    if ( ii==-1 && create ) {
        ii = cnode->interfaces.size();
        
        beginInsertRows( cindex, ii,ii );
        cnode->interfaces.append( inode );
        endInsertRows();
        cout<<"DEBUG: created interface "<<interface.toStdString()<<endl;
        
        // @todo sort the list alphabetically, not easy because it will break parent link
    }

    // this will return an invalid index if the platform is not found
    return index( ii , 0, cindex );
}

void
OcmModel::setInterface( const QString & registry, const QString & platform, const QString & component, 
            const QString & interface, const bool isProvided, const QString & ids, bool isReachable )
{
    QModelIndex ind = interfaceIndex( registry, platform, component, interface, true );

    if ( ind.isValid() ) {
        InterfaceNode* node = static_cast<InterfaceNode*>(ind.internalPointer());
        if ( isProvided ) {
            node->subtype = ProvidedInterfaceSubtype;
        }
        else {
            node->subtype = RequiredInterfaceSubtype;
        }
        node->ids = ids;
        node->isConnected = isReachable;
        emit dataChanged( ind,ind );
    }
}

QModelIndex 
OcmModel::operationIndex( const QString & registry, const QString & platform, const QString & component, 
                    const QString & interface, const QString & operation, bool create )
{
    QModelIndex iindex = interfaceIndex( registry, platform, component, interface );
    // failed to find
    if ( !iindex.isValid() ) {
        cout<<"DEBUG: failed to find interface "<<interface.toStdString()<<endl;
        return QModelIndex();
    }
    InterfaceNode* inode = static_cast<InterfaceNode*>(iindex.internalPointer());    
    
    OperationNode onode( operation, inode );
    int oi = inode->operations.indexOf( onode );

    if ( oi==-1 && create ) {
        oi = inode->operations.size();
        
        beginInsertRows( iindex, oi,oi );
        inode->operations.append( onode );
        endInsertRows();
        cout<<"DEBUG: created operation "<<operation.toStdString()<<" ("<<oi<<")"<<endl;
        
        // @todo sort the list alphabetically, not easy because it will break parent link
    }

    // this will return an invalid index if the platform is not found
    return index( oi, 0, iindex );
}

void 
OcmModel::setOperation( const QString & registry, const QString & platform, const QString & component, const QString & interface, 
            const QString & operation )
{    
    QModelIndex ind = operationIndex( registry, platform, component, interface, operation, true );

//     if ( ind.isValid() ) {
//         OperationNode* node = static_cast<OperationNode*>(ind.internalPointer());
//         // nothing to add yet
//         emit dataChanged( ind,ind );
//     }
}

QModelIndex 
OcmModel::resultIndex( const QString & registry, const QString & platform, const QString & component, 
                    const QString & interface, const QString & operation, const QString & result, bool create )
{
    QModelIndex oindex = operationIndex( registry, platform, component, interface, operation );
    // failed to find
    if ( !oindex.isValid() ) {
        cout<<"DEBUG: failed to find operation "<<operation.toStdString()<<endl;
        cout<<"\treg="<<registry.toStdString()
            <<" ptfm="<<platform.toStdString()
            <<"\tcomp="<<component.toStdString()
            <<"\tiface="<<interface.toStdString()
            <<"\toper="<<operation.toStdString()<<endl;
        return QModelIndex();
    }
    OperationNode* onode = static_cast<OperationNode*>(oindex.internalPointer());    
    
    ResultNode rnode( result, onode );
    int ri = onode->results.indexOf( rnode );

    if ( ri==-1 && create ) {
        ri = onode->results.size();
        
        beginInsertRows( oindex, ri,ri );
        onode->results.append( rnode );
        endInsertRows();
        cout<<"DEBUG: created result "<<result.toStdString()<<" ("<<ri<<")"<<endl;
        
        // @todo sort the list alphabetically, not easy because it will break parent link
    }

    // this will return an invalid index if the platform is not found
    return index( ri, 0, oindex );
}

void 
OcmModel::setResult( const QString & registry, const QString & platform, const QString & component, const QString & interface, 
            const QString & operation, const QString & result, const QString & text )
{    
    QModelIndex ind = resultIndex( registry, platform, component, interface, operation, result, true );

    if ( ind.isValid() ) {
        ResultNode* node = static_cast<ResultNode*>(ind.internalPointer());
        node->result = text;
        emit dataChanged( ind,ind );
    }
}

// OBSOLETE

// Returns the index of the platform with specified name.
QModelIndex
OcmModel::setPlatformPrivate( const QString & registry, const QString & regAddress,
                const QString & platform )
{
    // this will find our registry or make a new one
    QModelIndex rindex = registryIndex( registry, true );

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
//     cout<<"adding reg="<<registry.toStdString()<<" raddr="<<regAddress.toStdString()
//         <<" ptf="<<platform.toStdString()<<" cmp="<<component.toStdString()
//         <<" caddr="<<compAddress.toStdString()<<" con="<<connected<<" time="<<timeUp<<endl;
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

} // namespace
