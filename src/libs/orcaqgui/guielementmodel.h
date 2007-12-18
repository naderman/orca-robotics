/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAQGUI_GUI_ELEMENT_MODEL_H
#define ORCA_ORCAQGUI_GUI_ELEMENT_MODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include <QColor>

#include <orcaice/context.h>
#include <orcaqgui/guielement.h>

namespace orcaqgui
{

class IHumanManager;    
class GuiElement;
class GuiElementFactory;
class OrcaGuiUserEvent;
class GuiElementView;
class StringToColorMap;

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
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual bool removeRows( int row, int count, const QModelIndex & parent = QModelIndex() );

    //
    // CUSTOM API
    //
    void setView( GuiElementView* view );
    void updateGuiElements();
    void executeGuiElement( int guiElementIndex, int actionIndex );
    
    // sets the coordinate frame to the guielement's platform
    void setCoordinateFramePlatform( int guiElementIndex );
    void setOriginPlatform( int guiElementIndex );
    
    void selectedAdaptersInView( std::vector<int> &indices );
    void changePlatformFocus( const QString &platform );
    
    // access method for platform in focus
    const QString& coordinateFramePlatform() { return coordinateFramePlatform_; }
    QString coordinateFramePlatform() const { return coordinateFramePlatform_; }

    bool ignoreCoordinateFrameRotation() const { return ignoreCoordinateFrameRotation_; }
    
    const QList<GuiElement*> elements() { return elements_; };
    
    void createGuiElement( const QString &elementType, QStringList &elementDetails );
    
signals:
    void newPlatform( const QString& );
    void platformNeedsRemoval( const QString& );

public slots:
    void removeAllGuiElements();
    void createGuiElementFromSelection( const QList<QStringList> & );
    void setTransparency(bool transparency);

private:

    // returns true if supported by a factory otherwise false
    bool instantiateFromFactories( GuiElement* &element, 
                                   const QString &elementType, 
                                   const QColor &platformColor, 
                                   const QStringList &elementDetails );
    
    void determinePlatform( QStringList &elementDetails,
                            QString     &platform );

    QList<GuiElement*> elements_;
    
    const std::vector<orcaqgui::GuiElementFactory*> factories_;
    QStringList headers_;
    QString coordinateFramePlatform_;
    bool ignoreCoordinateFrameRotation_;
    orcaice::Context context_;
    IHumanManager *humanManager_;
    bool doesPlatformExist( QString &platformName );
    bool doesElementExist( const QStringList& elementDetails, int numElements );
    
    StringToColorMap *platformColors_;
    
    QString platformInFocus_;
    GuiElementView* view_;
    bool currentTransparency_;
    
    QString lookupTypeFromFactories( QStringList &ids );
};


}
#endif
