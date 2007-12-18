/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAQGUI_STRING_TO_COLOR_MAP_H
#define ORCA_ORCAQGUI_STRING_TO_COLOR_MAP_H

#include <vector>
#include <QColor>
#include <map>

namespace orcaqgui 
{
    // Manages a mapping from strings to colours.
    // Remembers colours that have previously been assigned.
    // Author: Tobias Kaupp, Alex Brooks
    class StringToColorMap
    {
        public:
            StringToColorMap();
            ~StringToColorMap() {};
            
            // gets the color for a given string, allocating a new one if necessary
            QColor getColor( const QString &str );
            
        private:
            
            QColor generateRandomColor();

            std::map<QString, QColor> colorMap_;
            unsigned int colorCounter_;
            std::vector<QColor> colorVector_;
            unsigned int seed_;
            
        
    };



}

#endif
