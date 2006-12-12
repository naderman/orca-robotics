/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI3D_PLATFORMCSFINDER_H
#define ORCAGUI3D_PLATFORMCSFINDER_H

#include <orcaqgui/guielementmodel.h>
#include <QString>
#include <QList>

namespace orcaqgui3d {

//!
//! @brief Class to work out the coordinate-system of a platform.  
//!        Applies a bunch of heuristics.
//!
//! Keeping this thing in a separate class keeps the dodgy hack separate
//! from other things, and makes it easy to supply a different dodgy hack.
//!
//! @author Alex Brooks
//!
class PlatformCSFinder
{

public: 

    PlatformCSFinder() {};

    // returns false if it doesn't know.
    bool findPlatformCS( const QList<orcaqgui::GuiElement*> elements,
                         const QString                                &platform,
                         float                                        &x,
                         float                                        &y,
                         float                                        &z,
                         float                                        &roll,
                         float                                        &pitch,
                         float                                        &yaw );

private: 


};

}

#endif
