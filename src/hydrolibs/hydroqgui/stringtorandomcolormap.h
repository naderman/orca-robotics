/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_HYDROQGUI_STRING_TO_RANDOM_COLOR_MAP_H
#define HYDRO_HYDROQGUI_STRING_TO_RANDOM_COLOR_MAP_H

#include <vector>
#include <QColor>
#include <map>
#include <hydroqgui/istringtocolormap.h>

namespace hydroqgui 
{
    // Manages a mapping from strings to random colours.
    // Remembers colours that have previously been assigned.
    // Author: Tobias Kaupp, Alex Brooks
    class StringToRandomColorMap : public IStringToColorMap
    {
        public:
            StringToRandomColorMap();
            ~StringToRandomColorMap() {};
            
            // gets the color for a given string, allocating a new one if necessary
            QColor getColor( const QString &str );
            
        private:
            
            QColor generateRandomColor();

            std::map<QString, QColor> colorMap_;
            unsigned int colorCounter_;
            std::vector<QColor> colorVector_;

        // AlexB: what's this seed gear for??
        // unsigned int seed_;
    };



}

#endif
