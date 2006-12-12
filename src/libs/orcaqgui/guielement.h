/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAQGUI_GUI_ELEMENT_H
#define ORCA2_ORCAQGUI_GUI_ELEMENT_H

#include <string>
#include <assert.h>
#include <QString>
#include <QColor>
#include <QPointF>
#include <QMouseEvent>

namespace orcaqgui
{
       
/*!
 *
 * @brief Anyone who wants to get painted should derive from this.
 *
 * GuiElement is a basic unit which knows how to paint itself and stores its data.
 * The GuielementModel holds a list of these GuiElements and periodically paints and updates them.
 *
 * @author Tobias Kaupp, Alex Brooks
*/
class GuiElement
{                                 
public:
    
    GuiElement() : platform_(""),
                   component_(""),
                   interface_(""),
                   id_("")
                   {};
    
    virtual ~GuiElement() {};

    //! Determines whether the element should be painted in the global coordinate system
    virtual bool isInGlobalCS()=0;

    //! Update is called periodically. All processing different from paint can be done here
    virtual void update () {};

    virtual QStringList contextMenu() { return QStringList(); };
    
    virtual void execute( int action ) {};
    
    //! Set the colour of the element.
    //! Note: this sets the general
    //!       colour scheme, not necessarily the colour of the entire
    //!       element.
    //!       In fact, the guiElement is free to ignore this colour.
    virtual void setColor( QColor color ) {};

    //! Set focus of element 
    //! Focus determines how the element is painted (color or gray) 
    //! and whether users can interact with it (buttons or no buttons)
    virtual void setFocus( bool inFocus ) {};
    
    //! Called when the GuiElement ceases to be the owner of the global user-input mode 
    //! NOTE: This is _not_ called if the GuiElement voluntarily relinquishes the mode.
    virtual void lostMode() {assert(false&&"unimplemented lostMode");}

    virtual void mousePressEvent(QMouseEvent *e) {assert(false&&"unimplemented mouse function");}
    virtual void mouseMoveEvent(QMouseEvent *e) {assert(false&&"unimplemented mouse function");}
    virtual void mouseReleaseEvent(QMouseEvent *e) {assert(false&&"unimplemented mouse function");}
    virtual void mouseDoubleClickEvent(QMouseEvent *e) {assert(false&&"unimplemented mouse function");} 
    
    QString platform() { return platform_; };
    void setPlatform( QString platform ) { platform_ = platform; };
    
    QString component() { return component_; };
    void setComponent( QString component ) { component_ = component; };
    
    QString interface() { return interface_; };
    void setInterface( QString interface ) { interface_ = interface; };
    
    QString id() { return id_; };
    void setId( QString id ) { id_ = id; };
    
    private:
        QString platform_;
        QString component_;
        QString interface_;
        QString id_;
    
};

} // namespace

#endif
