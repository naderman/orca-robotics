/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_OCM_MODEL_H
#define ORCA_OCM_MODEL_H

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
    enum IconType { Registry, Platform, Component, Provided, Required, Operation, Result };
    QIcon icon(IconType type) const;

private:
    QIcon registry;
    QIcon platform;
    QIcon component;
    QIcon provided;
    QIcon required;
    QIcon operation;
    QIcon result;
};

//! A model (as in model-view-controller) holding information about the Orca
//! component model (i.e. a hierarchy of platforms, components, interfaces).
//! 
//! Determines the paint style of individual entries in the model based on data.
//!
//! @see OcmView, OcmDelegate
class OcmModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Roles {
        StatusIconRole = Qt::DecorationRole,
        TypeRole = Qt::UserRole + 1,
        ConnectionRole,
        SummaryRole,
        DetailRole
    };

    explicit OcmModel(QObject *parent = 0);

    // from QAbstractItemModel
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    //bool setData(const QModelIndex &index, const QVariant &value, int role);
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual bool hasChildren(const QModelIndex &index) const;

    // from QObject
    //! Processes events sent to us by the GetComponentsJob.
    virtual void customEvent( QEvent* e );

    // OcmModel specific API
    
    //! Find registry node by its name. If not found and create=TRUE, create a new one.
    QModelIndex registryIndex( const QString& registry, bool create=false );

    //! Find platform node by its name and the name of its registry. If not found and 
    //! create=TRUE, create a new one.
    QModelIndex platformIndex( const QString& registry, const QString &platform, bool create=false );
            
    //! Find component node by its name, and the names of its parents. If not found and 
    //! create=TRUE, create a new one.
    QModelIndex componentIndex( const QString& registry, const QString& platform, 
                        const QString& component, bool create=false );

    //! Find interface node by its name, and the names of its parents. If not found and 
    //! create=TRUE, create a new one.
    QModelIndex interfaceIndex( const QString& registry, const QString& platform, 
                        const QString& component, const QString& interface, bool create=false );

    //! Find operation node by its name, and the names of its parents. If not found and 
    //! create=TRUE, create a new one.
    QModelIndex operationIndex( const QString& registry, const QString& platform, const QString& component, 
                       const QString& interface, const QString& operation, bool create=false );

    //! Find result node by its name, and the names of its parents. If not found and 
    //! create=TRUE, create a new one.
    QModelIndex resultIndex( const QString& registry, const QString& platform, const QString& component, 
                       const QString& interface, const QString& operation, const QString& result, bool create=false );

    //! Set info for a registry node.
    void setRegistry( const QString& registry, const QString& regAddress, bool connected );

    //! Set info for a platform node.
    void setPlatform( const QString& registry, 
                      const QString &platform );
            
    //! Set info for a component node.
    void setComponent( const QString& registry, const QString& platform,
                       const QString& component, const QString& compAddress, bool connected, int timeUp );

    //! Set info for a interface node.
    void setInterface( const QString& registry, const QString& platform, const QString& component,
                       const QString& interface, const bool isProvided, const QString& ids, bool isEnabled );

    //! Set info for a operation node.
    void setOperation( const QString& registry, const QString& platform, const QString& component, 
                                    const QString& interface, 
                       const QString& operation, const QString& signature );

    //! Set info for a result node.
    void setResult( const QString& registry, const QString& platform, const QString& component, 
                                    const QString& interface, const QString& operation, 
                       const QString& result, const QString& text );

    //! Custom function for getting information about an interface.
    //! Returns 0 if the data was retrieved properly or 1 if something went wrong, e.g.
    //! the index does not point to an InterfaceType.
    int interfaceData( const QModelIndex& ind,
                       QString& registry, QString& platform, QString& component,
                       QString& interface, QString& id );
    
    //! Custom function for getting information about interfaces on a platform.
    //! Returns 0 if the data was retrieved properly or 1 if something went wrong, e.g.
    //! the index does not point to a PlatformType.
    int interfacesOnPlatform( const QModelIndex& ind,
                              QStringList& registries, QStringList& platforms, QStringList& components,
                              QStringList& interfaces, QStringList& ids );
    
    //! As above but for interfaces of a component
    int interfacesOnComponent( const QModelIndex& ind,
                              QStringList& registries, QStringList& platforms, QStringList& components,
                              QStringList& interfaces, QStringList& ids );
    
    // OBSOLETE OcmModel specific API

    void setPlatform( const QString& registry, const QString& regAddress, const QString &platform )
            { setPlatformPrivate( registry, regAddress, platform ); };
            
    void setComponent( const QString& registry, const QString& regAddress, const QString& platform,
                        const QString& component, const QString& compAddress, bool connected, int timeUp )
            { setComponentPrivate( registry, regAddress, platform, component, compAddress, connected, timeUp ); };

    void setInterface( const QString& registry, const QString& regAddress,
                       const QString& platform, const QString& component,
                       const QString& interface, const bool isProvided,
                       const QString& compAddress, const QString& ids,
                       bool connected, int timeUp )
            { setInterfacePrivate( registry, regAddress, platform, component, interface,
                    isProvided, compAddress, ids, connected, timeUp ); };

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
        OperationType,
        ResultType
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
    class ResultNode;

    class ResultNode : public Node
    {
    public:
        ResultNode( const QString &n, OperationNode* o )
            : name(n), operation(o) {};
        // to be used in list search, name is sufficient
        bool operator==( const ResultNode& other ) const
        {
            return name==other.name;
        }
        QString name;
        OperationNode* operation;
        QString result;
        virtual NodeType type() { return ResultType; };
    };

    class OperationNode : public Node
    {
    public:
        OperationNode( const QString &n, InterfaceNode* i )
            : name(n), interface(i) {};
        // to be used in list search, name is sufficient
        bool operator==( const OperationNode& other ) const
        {
            return name==other.name;
        }
        QString name;
        QString signature;
        InterfaceNode* interface;
        QList<ResultNode> results;
        virtual NodeType type() { return OperationType; };
    };

    class InterfaceNode : public Node
    {
    public:
        InterfaceNode( const QString &n, ComponentNode* c )
            : name(n), component(c) {};
        // to be used in list search, name is sufficient
        bool operator==( const InterfaceNode& other ) const
        {
            return name==other.name;
        }
        QString name;
        ComponentNode* component;
        InterfaceSubtype subtype;
        // not very clean: only provided interfaces can have operations 
        QList<OperationNode> operations;
        QString ids;
        bool isEnabled;
        virtual NodeType type() { return InterfaceType; };
    };
    
    class ComponentNode : public Node
    {
    public:
        ComponentNode( const QString &n, PlatformNode* p, 
            const QString &a="", bool connected=true, int t=0 );
        // to be used in list search, name is sufficient
        bool operator==( const ComponentNode& other ) const
        {
            return name==other.name;
        }
        QString name;
        PlatformNode* platform;
        QList<InterfaceNode> interfaces;
        QString address;
        bool isEnabled;
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
        bool operator==( const PlatformNode& other ) const
        {
            return name==other.name;
        }
        QString name;
        RegistryNode* registry;
        QList<ComponentNode> components;
        bool isEnabled;
        virtual NodeType type() { return PlatformType; };
    };

    class RegistryNode : public Node
    {
    public:
        RegistryNode( const QString &n, const QString &a="", bool connected=true )
            : name(n), address(a), isEnabled(connected) {};
        // to be used in list search, name is sufficient
        bool operator==( const RegistryNode& other ) const
        {
            return name==other.name;
        }
        QString name;
        QList<PlatformNode> platforms;
        QString address;
        bool isEnabled;
        virtual NodeType type() { return RegistryType; };
    };

    // top level of data storage
    QList<RegistryNode> registries_;

    QStringList headers_;

    OcmIconProvider iconProvider_;

    // OBSOLETE
    QModelIndex setPlatformPrivate( const QString& registry, const QString& regAddress,
                                    const QString &platform );
    
    QModelIndex setComponentPrivate( const QString& registry, const QString& regAddress,
                                const QString& platform, const QString& component,
                                const QString& compAddress, bool connected, int timeUp );

    QModelIndex setInterfacePrivate( const QString& registry, const QString& regAddress,
                                const QString& platform, const QString& component, const QString& interface,
                                const bool isProvided, const QString& compAddress, const QString& ids,
                                bool connected, int timeUp );

};

} // namespace

#endif
