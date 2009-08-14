/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include "guielementset.h"
#include <iostream>
#include <gbxutilacfr/exceptions.h>

using namespace std;

namespace hydroqgui {

GuiElementSet::GuiElementSet()
    : currentlyUsingTransparency_(false)
{
}

void
GuiElementSet::setUseTransparency( bool useTransparency )
{
//     cout << "TRACE(guielementset.cpp): setUseTransparency to " << useTransparency << endl;
    if ( useTransparency == currentlyUsingTransparency_ ) return;

    currentlyUsingTransparency_ = useTransparency;
    
    for ( int i=0; i<elements().size(); ++i )
    {
        elements_[i]->setUseTransparency( useTransparency );
    }        
}

void 
GuiElementSet::addGuiElement(hydroqguielementutil::GuiElement *newGuiElement )
{
    // Set properties of new element, so all properties are uniform
    newGuiElement->setUseTransparency( currentlyUsingTransparency_ );

    elements_.append( newGuiElement );
}

void
GuiElementSet::removeGuiElement( int index )
{
    assert( index >= 0 && index < (int)(elements_.size()) );

    // the elements() list contains pointers, we have to delete the object
    delete elements_[index];
    // now remove the entry in the list
    elements_.removeAt( index );    
}

void 
GuiElementSet::removeGuiElement( hydroqguielementutil::GuiElement *guiElement )
{
    int i = elements_.indexOf( guiElement );
    if ( i == -1 )
        throw gbxutilacfr::Exception( ERROR_INFO, "Attempt to remove non-existent guiElement" );

    removeGuiElement( i );
}

}

