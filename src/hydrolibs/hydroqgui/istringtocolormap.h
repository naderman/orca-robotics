/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_HYDROQGUI_ISTRING_TO_COLOR_MAP_H
#define HYDRO_HYDROQGUI_ISTRING_TO_COLOR_MAP_H

#include <vector>
#include <QColor>
#include <map>

namespace hydroqgui 
{
    //!
    //! @brief Manages a mapping from strings to colours.
    //!
    //! @author Tobias Kaupp, Alex Brooks
    //!
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
