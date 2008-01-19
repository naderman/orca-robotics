/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_DEFINITIONS2D_H
#define ORCAGUI_DEFINITIONS2D_H

/*!
    \brief Defines levels for z-buffering in views.
    Simple z-buffering, higher-z items obscure (are in front of) lower-z items (just like QCanvas)
    only z levels between Z_BACKGROUND and Z_FOREGROUND inclusively are painted.
    
    Note: It's important that the paintable levels are 1->12 inclusive.  If this changes, remember
            to change qgraphics2d also!
*/

namespace orcaqgui2d
{ 

enum GuiZBufferLevels {
    // environment
    Z_HIDDEN_BEHIND=0,
    Z_BACKGROUND,
    Z_OG_MAP,
    Z_GRID,
    Z_LASER,
    Z_PATH,
    Z_POSE,
    Z_VELOCITY,
    Z_LIKELIHOOD_MAP,
    Z_SLAM_MAP,
    Z_LASER_FEATURES,
    Z_ORIGIN,
    Z_FOREGROUND,
    Z_HIDDEN_IN_FRONT
    // do not put anything after, it will not paint
};
    
}

#endif

