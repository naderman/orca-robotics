/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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
    
class OrcaGuiException : public std::exception
{ 
public:
    OrcaGuiException(const char *message)
        : message_(message)
        {}
    OrcaGuiException(const std::string &message)
        : message_(message)
        {}
    ~OrcaGuiException()throw(){}
    virtual const char* what() const throw() { return message_.c_str(); }
private:
    std::string  message_;
};
       
/*!
 *
 * @brief Anyone who wants to get painted should derive from this.
 *
 * GuiElement is a basic unit which knows how to paint itself and stores its data.
 * The GuielementModel holds a list of these GuiElements and periodically paints and updates them.
*/
class GuiElement
{                                 
public:
    
    GuiElement() : platform_(""),
                   details_(""),
                   type_("")
                   {};
    
    virtual ~GuiElement() {};

    //! Determines whether the element should be painted in the global coordinate system.
    //! If returns FALSE, we will search for a localization element on the same platform.
    virtual bool isInGlobalCS()=0;

    //! Update is called periodically. All processing different from paint can be done here
    virtual void update () {};

    //! Optional function to populate the menu list on right-click. 
    //! Default implementation does nothing.
    virtual QStringList contextMenu() { return QStringList(); };
    
    //! Optional callback function which will be called when one of the context menu items
    //! is selected. Default implementation does nothing.
    virtual void execute( int action ) {};
    
    //! Set the colour of the element.
    //! This sets the general colour scheme, not necessarily the colour of the entire
    //! element. In fact, the guiElement is free to ignore this colour.
    virtual void setColor( QColor color ) {};

    //! Optional function to set focus of element. Default implementation does nothing.
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
    
    //! Tell the GuiElement whether to paint itself using transparent colors
    virtual void setTransparency( bool useTransparency ) {};
    
    //! Access and set functions for platform name
    QString platform() { return platform_; };
    void setPlatform( QString platform ) { platform_ = platform; };
    
    //! Access and set functions for details string displayed in table, e.g. ogmap@hemp/ogmaploader
    QString details() { return details_; };
    void setDetails( QString details ) { details_ = details; };
    
    //! Access and set functions for type string displayed in table, e.g. OgMap
    QString type() { return type_; };
    void setName( QString type ) { type_ = type; };
    
private:
    QString platform_;
    QString details_;
    QString type_;
    
};

} // namespace

#endif
