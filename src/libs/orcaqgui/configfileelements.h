/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCQQGUI_CONFIGFILEELEMENTS_H
#define ORCQQGUI_CONFIGFILEELEMENTS_H

#include <orcaqguielementmodelview/guielementmodel.h>
#include <orcaice/context.h>

namespace orcaqgui {

    // Returns a pointer to the grid
    hydroqguielementutil::IGuiElement* 
    loadGrid( orcaqgemv::GuiElementModel &guiElementModel );

    void loadElementsFromConfigFile( orcaqgemv::GuiElementModel &guiElementModel,
                                     const orcaice::Context     &context );

}

#endif
