/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDROQGUI_GUI_ELEMENT_H
#define HYDROQGUI_GUI_ELEMENT_H

#include <string>
#include <assert.h>
#include <QString>
#include <QColor>
#include <QMouseEvent>
#include <hydroutil/uncopyable.h>

namespace hydroqguielementutil
{

//! Essential information required by all gui elements
struct GuiElementInfo {
    GuiElementInfo( const char *pPlatform,
                    const char *pUniqueId,
                    const char *pType )
        : platform(pPlatform),
          uniqueId(pUniqueId),
          type(pType)
        {}
    QString platform;
    QString uniqueId;
    QString type;
};

/*!
 *
 * @brief A basic GUI component with a type and a unique ID.
 *
 * A GuiElement may receive or send data and may paint itself. It is allocated to a physical (robotic) platform.
 * The GuiElementModel holds a list of these and periodically updates them.
 *
 * @author Tobias Kaupp
*/
class GuiElement : public hydroutil::Uncopyable
{                                 
public:
    
    GuiElement( const GuiElementInfo &guiElementInfo )
        : platform_(guiElementInfo.platform),
          uniqueId_(guiElementInfo.uniqueId),
          type_(guiElementInfo.type)
        {}
    
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
    virtual void noLongerMouseEventReceiver() {assert(false&&"unimplemented noLongerMouseEventReceiver");}

    virtual void mousePressEvent(QMouseEvent *e) {assert(false&&"unimplemented mouse function");}
    virtual void mouseMoveEvent(QMouseEvent *e) {assert(false&&"unimplemented mouse function");}
    virtual void mouseReleaseEvent(QMouseEvent *e) {assert(false&&"unimplemented mouse function");}
    virtual void mouseDoubleClickEvent(QMouseEvent *e) {assert(false&&"unimplemented mouse function");} 
    
    //! Tell the GuiElement whether to paint itself using transparent colors
    virtual void setUseTransparency( bool useTransparency ) {};
    
    //! Access function for platform name
    QString platform() { return platform_; };
    
    //! Access function for unique ID string displayed in table, e.g. ogmap@hemp/ogmaploader
    QString uniqueId() { return uniqueId_; };
    
    //! Access function for type string displayed in table, e.g. OgMap
    QString type() { return type_; };
    
    //! If true, the user shouldn't be able to remove this GUI element. 
    virtual bool isPermanentElement() const { return false; }
    
private:
    QString platform_;
    QString uniqueId_;
    QString type_;
    
};

} // namespace

#endif
