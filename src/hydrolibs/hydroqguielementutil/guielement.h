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
#include <hydroqguielementutil/ihumanmanager.h>
#include <hydroqguielementutil/mouseeventmanager.h>
#include <hydroqguielementutil/shortcutkeymanager.h>

class QSplitter;

namespace hydroqgui {
    class GuiElementSet;
};

namespace hydroqguielementutil
{

//! Essential information required by all GUI elements
struct GuiElementInfo 
{
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


//! Information common to all GUI elements.
struct GuiElementStuff 
{
    GuiElementStuff() :
        humanManager(0),
        mouseEventManager(0),
        shortcutKeyManager(0),
        guiElementSet(0),
        spaceBottomRight(0)
    {};

    IHumanManager*      humanManager;
    MouseEventManager*  mouseEventManager;
    ShortcutKeyManager* shortcutKeyManager;
    hydroqgui::GuiElementSet* guiElementSet;
    QSplitter* spaceBottomRight;
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
    //! If returns FALSE (default), we will search for a localization element on the same platform.
    virtual bool isInGlobalCS() { return false; };

    //! If FALSE (default), the user can remove this GUI element from the set. 
    virtual bool isPermanentElement() const { return false; }

    //! Update is called periodically. All processing different from paint can be done here.
    //! Default implementation does nothing.
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

    //! Tell the GuiElement whether to paint itself using transparent colors
    virtual void setUseTransparency( bool useTransparency ) {};

    //! Optional function to set focus of element. Default implementation does nothing.
    //! Focus determines how the element is painted (color or gray) 
    //! and whether users can interact with it (buttons or no buttons)
    virtual void setFocus( bool inFocus ) {};
    
    //! Called when the GuiElement ceases to be the owner of the global user-input mode 
    //! NOTE: This is @b not called if the GuiElement voluntarily relinquishes the mode.
    virtual void noLongerMouseEventReceiver() {assert(!"unimplemented noLongerMouseEventReceiver");}

    virtual void mousePressEvent(QMouseEvent *e) {assert(!"unimplemented mouse function");}
    virtual void mouseMoveEvent(QMouseEvent *e) {assert(!"unimplemented mouse function");}
    virtual void mouseReleaseEvent(QMouseEvent *e) {assert(!"unimplemented mouse function");}
    virtual void mouseDoubleClickEvent(QMouseEvent *e) {assert(!"unimplemented mouse function");} 
    
    //! Access function for platform name
    QString platform() { return platform_; };
    
    //! Access function for unique ID string displayed in table, e.g. ogmap@hemp/ogmaploader
    QString uniqueId() { return uniqueId_; };
    
    //! Access function for type string displayed in table, e.g. OgMap
    QString type() { return type_; };  

    static void setGuiElementStuff( const GuiElementStuff& stuff ) { _stuff=stuff; };
    
protected :

    static GuiElementStuff _stuff;

private:
    QString platform_;
    QString uniqueId_;
    QString type_;
};

} // namespace

#endif
