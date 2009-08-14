/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef HYDROQGUI_GUIELEMENTSET_H
#define HYDROQGUI_GUIELEMENTSET_H

#include <QStringList>
#include <hydroutil/uncopyable.h>
#include <hydroqguielementutil/guielement.h>
#include <hydroqgui/platformfocusmanager.h>

namespace hydroqgui {

//!
//! @brief Stores the set of all GuiElements
//!
//! @author Alex Brooks
//!
class GuiElementSet : public hydroutil::Uncopyable
{
public: 

    GuiElementSet();

    void addGuiElement( hydroqguielementutil::GuiElement *newGuiElement );

    // Removes (and deletes) the index'th Gui Element
    void removeGuiElement( int index );
    // Removes (and deletes) the named Gui Element
    void removeGuiElement( hydroqguielementutil::GuiElement *guiElement );

    const QList<hydroqguielementutil::GuiElement*> &elements() const { return elements_; }

    void setUseTransparency(bool useTransparency);

private: 

    bool currentlyUsingTransparency_;
    QList<hydroqguielementutil::GuiElement*> elements_;

};

}

#endif
