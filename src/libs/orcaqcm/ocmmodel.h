/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_OCM_MODEL_H
#define ORCA2_OCM_MODEL_H

#include <QAbstractItemModel>
#include <QDateTime>
#include <QStringList>
#include <QIcon>

namespace orcaqcm
{

class OcmIconProvider
{
public:
    OcmIconProvider();
    enum IconType { Registry, Platform, Component, Provided, Required, Operation };
    QIcon icon(IconType type) const;

private:
    QIcon registry;
    QIcon platform;
    QIcon component;
    QIcon provided;
    QIcon required;
    QIcon operation;
};


class OcmModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Roles {
        StatusIconRole = Qt::DecorationRole,
        TypeRole = Qt::UserRole + 1,
        ConnectionRole
    };

    explicit OcmModel(QObject *parent = 0);
    ~OcmModel();

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    //bool setData(const QModelIndex &index, const QVariant &value, int role);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    bool hasChildren(const QModelIndex &index) const;

    // OcmModel specific API
    
    QModelIndex registryIndex( const QString & registry ) const;

    QModelIndex platformIndex( const QString & registry, const QString &platform ) const;
            
    QModelIndex componentIndex( const QString & registry, const QString & platform, 
                        const QString & component ) const;

    QModelIndex interfaceIndex( const QString & registry, const QString & platform, 
                        const QString & component, const QString & interface ) const;

    QModelIndex operationIndex( const QString & registry, const QString & platform, const QString & component, 
                       const QString & interface, const QString & operation );

    void setRegistry( const QString & registry, const QString & regAddress, bool connected )
            { setRegistryPrivate(registry, regAddress, connected); };

    void setPlatform( const QString & registry, 
                      const QString &platform )
            { setPlatformPrivate( registry, platform ); };
            
    void setComponent( const QString & registry, const QString & platform,
                       const QString & component, const QString & compAddress, bool connected, int timeUp )
            { setComponentPrivate( registry, platform, component, compAddress, connected, timeUp ); };

    void setInterface( const QString & registry, const QString & platform, const QString & component,
                       const QString & interface, const bool isProvided, const QString & ids, bool isReachable )
            { setInterfacePrivate( registry, platform, component, interface, isProvided, ids, isReachable ); };

    void setOperation( const QString & registry, const QString & platform, const QString & component, const QString & interface, 
                       const QString & name, const QString & result )
            { setOperationPrivate( registry, platform, component, interface, name, result ); };

    // OBSOLETE OcmModel specific API

    void setPlatform( const QString & registry, const QString & regAddress, const QString &platform )
            { setPlatformPrivate( registry, regAddress, platform ); };
            
    void setComponent( const QString & registry, const QString & regAddress, const QString & platform,
                        const QString & component, const QString & compAddress, bool connected, int timeUp )
            { setComponentPrivate( registry, regAddress, platform, component, compAddress, connected, timeUp ); };

    void setInterface( const QString & registry, const QString & regAddress,
                       const QString & platform, const QString & component,
                       const QString & interface, const bool isProvided,
                       const QString & compAddress, const QString & ids,
                       bool connected, int timeUp )
            { setInterfacePrivate( registry, regAddress, platform, component, interface,
                    isProvided, compAddress, ids, connected, timeUp ); };

    //! Returns 0 if the data was retrieved properly or 1 if something went wrong, e.g.
    //! the index does not point to an InterfaceType.
    int getInterface( const QModelIndex& ind,
                       QString & registry, QString & platform, QString & component,
                       QString & interface, QString & id );

public slots:
    //! Delete all data from the model
    void clear();

private:

    enum NodeType
    {
        RegistryType,
        PlatformType,
        ComponentType,
        InterfaceType,
        OperationType
    };

    enum InterfaceSubtype
    {
        ProvidedInterfaceSubtype,
        RequiredInterfaceSubtype
    };

    class Node
    {
    public:
        virtual ~Node() {};
        virtual NodeType type()=0;
    };
    
    class RegistryNode;
    class PlatformNode;
    class ComponentNode;
    class InterfaceNode;
    class OperationNode;

    class OperationNode : public Node
    {
    public:
        OperationNode( const QString &n, InterfaceNode* i )
            : name(n), interface(i) {};
        // to be used in list search, name is sufficient
        bool operator==( const OperationNode & other ) const
        {
            return name==other.name;
        }
        QString name;
        InterfaceNode* interface;
        virtual NodeType type() { return OperationType; };
    };

    class InterfaceNode : public Node
    {
    public:
        InterfaceNode( const QString &n, ComponentNode* c )
            : name(n), component(c) {};
        // to be used in list search, name is sufficient
        bool operator==( const InterfaceNode & other ) const
        {
            return name==other.name;
        }
        QString name;
        ComponentNode* component;
        InterfaceSubtype subtype;
        QString ids;
        bool isConnected;
        virtual NodeType type() { return InterfaceType; };
    };
    
    class ComponentNode : public Node
    {
    public:
        ComponentNode( const QString &n, PlatformNode* p, const QString &a, bool connected, int t );
        // to be used in list search, name is sufficient
        bool operator==( const ComponentNode & other ) const
        {
            return name==other.name;
        }
        QString name;
        PlatformNode* platform;
        QList<InterfaceNode> interfaces;
        QString address;
        bool isConnected;
        int daysUp;
        QTime timeUp;
        virtual NodeType type() { return ComponentType; };
    };
    
    class PlatformNode : public Node
    {
    public:
        PlatformNode( const QString &n, RegistryNode* r )
            : name(n), registry(r) {};
        // to be used in list search, name is sufficient
        bool operator==( const PlatformNode & other ) const
        {
            return name==other.name;
        }
        QString name;
        RegistryNode* registry;
        QList<ComponentNode> components;
        bool isConnected;
        virtual NodeType type() { return PlatformType; };
    };

    class RegistryNode : public Node
    {
    public:
        RegistryNode( const QString &n, const QString &a, bool connected )
            : name(n), address(a), isConnected(connected) {};
        // to be used in list search, name is sufficient
        bool operator==( const RegistryNode & other ) const
        {
            return name==other.name;
        }
        QString name;
        QList<PlatformNode> platforms;
        QString address;
        bool isConnected;
        virtual NodeType type() { return RegistryType; };
    };

    // top level of data storage
    QList<RegistryNode> registries_;

    QStringList headers_;

    OcmIconProvider iconProvider_;

    
    QModelIndex setRegistryPrivate( const QString & registry, const QString & regAddress, bool connected );
    
    QModelIndex setPlatformPrivate( const QString & registry,
                const QString &platform );
    
    QModelIndex setComponentPrivate( const QString & registry, const QString & platform, 
                const QString & component, const QString & compAddress, bool connected, int timeUp );

    QModelIndex setInterfacePrivate( const QString & registry, const QString & platform, const QString & component, 
                const QString & interface, const bool isProvided, const QString & ids, bool isReachable );

    QModelIndex setOperationPrivate( const QString & registry, const QString & platform, const QString & component, const QString & interface, 
                const QString & name, const QString & result );

    // OBSOLETE
    QModelIndex setPlatformPrivate( const QString & registry, const QString & regAddress,
                                    const QString &platform );
    
    QModelIndex setComponentPrivate( const QString & registry, const QString & regAddress,
                                const QString & platform, const QString & component,
                                const QString & compAddress, bool connected, int timeUp );

    QModelIndex setInterfacePrivate( const QString & registry, const QString & regAddress,
                                const QString & platform, const QString & component, const QString & interface,
                                const bool isProvided, const QString & compAddress, const QString & ids,
                                bool connected, int timeUp );

};

} // namespace

#endif
