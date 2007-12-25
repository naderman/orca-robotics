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
#include <hydroqgui/hydroqgui.h>

namespace orcaqgemv
{

class GuiElementView;

//!
//! @brief Stores the set of GuiElements
//!
class GuiElementModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Roles {
        InterfaceIdRole = Qt::UserRole + 1,
        ContextMenuRole,
        FocusRole
    };

    explicit GuiElementModel( const std::vector<hydroqgui::IGuiElementFactory*> &factories,
                              hydroqgui::IHumanManager                          &humanManager,
                              hydroqgui::MouseEventManager                      &mouseEventManager,
                              hydroqgui::ShortcutKeyManager                     &shortcutKeyManager,
                              hydroqgui::CoordinateFrameManager                 &coordinateFrameManager,
                              hydroqgui::GuiElementSet                          &guiElementSet,
                              hydroqgui::IStringToColorMap                      &platformColorMap,
                              QObject                                           *parent = 0);

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
    
//     // access method for platform in focus
//     const QString& coordinateFramePlatform() { return coordinateFramePlatform_; }
//     QString coordinateFramePlatform() const { return coordinateFramePlatform_; }
// 
//    bool ignoreCoordinateFrameRotation() const { return ignoreCoordinateFrameRotation_; }
    
//    const QList<hydroqgui::IGuiElement*> &elements() { return elements_; };
    
    void createGuiElement( const QString &elementType, QStringList &elementDetails );
    
signals:
    void newPlatform( const QString& );
    void platformNeedsRemoval( const QString& );

public slots:
    void removeAllGuiElements();
    void createGuiElementFromSelection( const QList<QStringList> & );
    // void setUseTransparency(bool transparency);

private:

    // returns true if supported by a factory otherwise false
    bool instantiateFromFactories( hydroqgui::IGuiElement* &element, 
                                   const QString &elementType, 
                                   const QColor &platformColor, 
                                   const QStringList &elementDetails );
    
    void determinePlatform( QStringList &elementDetails,
                            QString     &platform );

    // QList<hydroqgui::IGuiElement*> elements_;
    hydroqgui::GuiElementSet &guiElementSet_;

    const QList<hydroqgui::IGuiElement*> &elements() const { return guiElementSet_.elements(); }
    // QList<hydroqgui::IGuiElement*> &elements() { return guiElementSet_.elements(); }
    
    const std::vector<hydroqgui::IGuiElementFactory*> factories_;
    QStringList headers_;
    hydroqgui::IHumanManager      &humanManager_;
    hydroqgui::MouseEventManager  &mouseEventManager_;
    hydroqgui::ShortcutKeyManager &shortcutKeyManager_;
    hydroqgui::CoordinateFrameManager &coordinateFrameManager_;
    bool doesPlatformExist( QString &platformName );
    bool doesElementExist( const QStringList& elementDetails, int numElements );
    
    hydroqgui::IStringToColorMap &platformColorMap_;
    
    QString platformInFocus_;
    GuiElementView* view_;
    
    QString lookupTypeFromFactories( QStringList &ids );
};


}
#endif