/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAQGUI_ISTRING_TO_COLOR_MAP_H
#define ORCA_ORCAQGUI_ISTRING_TO_COLOR_MAP_H

#include <vector>
#include <QColor>
#include <map>

namespace orcaqgui 
{
    // Manages a mapping from strings to colours.
    // Author: Tobias Kaupp, Alex Brooks
    class IStringToColorMap
    {
    public:
        IStringToColorMap() {}
        virtual ~IStringToColorMap() {};
            
        // gets the color for a given string.
        virtual QColor getColor( const QString &str )=0;
    };



}

#endif
