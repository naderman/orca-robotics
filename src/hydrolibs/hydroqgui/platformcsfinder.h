/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef PLATFORMCSFINDER_H
#define PLATFORMCSFINDER_H

#include <QString>
#include <QList>
#include <hydroqguielementutil/guielement.h>

namespace hydroqgui {

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
    bool findPlatformCS( const QList< ::hydroqguielementutil::GuiElement* > &elements,
                         const QString                                       &platform,
                         float                                               &x,
                         float                                               &y,
                         float                                               &theta ) const;

private: 


};

}

#endif
