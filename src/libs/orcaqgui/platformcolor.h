/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAQGUI_PLATFORM_COLOR_H
#define ORCA2_ORCAQGUI_PLATFORM_COLOR_H

#include <vector>
#include <QColor>
#include <QMap>

namespace orcaqgui 
{
    // Handles everything related to platform colors
    // Author: Tobias Kaupp
    class PlatformColor
    {
        public:
            PlatformColor();
            ~PlatformColor() {};
            
            // gets the color of a given platform, returns false if platform doesn't exist
            // otherwise true
            bool getColor( const QString &platform, QColor &color );
            
            // add a new platform, color is chosen internally
            void setNewPlatform( QString &platform );
            
        private:
            QColor generateRandomColor();
            QMap<QString, QColor> colorMap_;
            unsigned int colorCounter_;
            std::vector<QColor> colorVector_;
            unsigned int seed_;
            
        
    };



}

#endif
