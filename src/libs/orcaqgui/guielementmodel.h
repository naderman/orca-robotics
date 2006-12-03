/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_GUI_ELEMENT_MODEL_H
#define ORCA2_GUI_ELEMENT_MODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include <QColor>

#include <orcaice/context.h>
#include <orcaqgui/guielement.h>

class QPainter;

namespace orcaqgui
{

class IHumanManager;    
class GuiElement;
class GuiElementFactory;
class OrcaGuiUserEvent;
class GuiElementView;

class GuiElementModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Roles {
        InterfaceIdRole = Qt::UserRole + 1,
        ContextMenuRole,
        FocusRole
    };
    
    explicit GuiElementModel( const std::vector<orcaqgui::GuiElementFactory*> &factories,
                              const orcaice::Context                         &context, 
                              IHumanManager                                  *humanManager,
                              QObject                                        *parent = 0);
    ~GuiElementModel();

    //
    // STANDARD MODEL API
    //
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool removeRows( int row, int count, const QModelIndex & parent = QModelIndex() );

    //
    // CUSTOM API
    //
    void setView( GuiElementView* view );
    void updateGuiElements();
    void executeGuiElement( int guiElementIndex, int actionIndex );
    
    // sets the coordinate frame to the guielement's platform
    void setCoordinateFramePlatform( int guiElementIndex );
    
    void selectedAdaptersInView( std::vector<int> &indices );
    void changePlatformFocus( const QString &platform );
    
    // access method for platform in focus
    const QString& coordinateFramePlatform() { return coordinateFramePlatform_; };
    QString coordinateFramePlatform() const { return coordinateFramePlatform_; }
    

    class InterfaceNode
    {
        public:
            InterfaceNode( const QString &r, const QString &p, const QString &c,
                           const QString &iface, const QString &i  );
            ~InterfaceNode();
        
            bool operator==( const InterfaceNode & other ) const;
        
            QString registry;
            QString platform;
            QString component;
            QString interface;
            QString id;
            orcaqgui::GuiElement *element;
    };

     // access list of elements
    const QList<InterfaceNode*> elements() { return elements_; };
    
signals:
    void newPlatform(const QString&);

public slots:
    void removeAllGuiElements();
    void createGuiElement( const QStringList & interfaceInfo );

private:

    GuiElement *instantiateFromFactories( const QString &id, const QColor &platformColor, const QString &proxyStr );

    QList<InterfaceNode*> elements_;
    const std::vector<orcaqgui::GuiElementFactory*> factories_;
    QStringList headers_;
    QString coordinateFramePlatform_;
    orcaice::Context context_;
    IHumanManager *humanManager_;
    bool isNewPlatform( QString &platformName );
    QColor generateRandomColor();
    QMap<QString, QColor> colorMap_;
    uint colorCounter_;
    std::vector<QColor> colorVector_;
    uint seed_;
    QString platformInFocus_;
    GuiElementView* view_;
};

std::string toString( const GuiElementModel::InterfaceNode &n );

}
#endif
