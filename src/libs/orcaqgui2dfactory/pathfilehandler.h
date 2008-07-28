/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef PATH_FILE_HANDLER_H
#define PATH_FILE_HANDLER_H

#include <QString>
#include <orcaqgui2dfactory/pathutils.h>

namespace orcaqgui2d {
    
class IHumanManager;

//!
//! A class which handles loading and saving paths from/to a file
//!
//! @author Tobias Kaupp
//! 
class PathFileHandler
{
    public:
        
        PathFileHandler( hydroqguielementutil::IHumanManager &humanManager,
                         const QString                       &lastSavedPathFile = "" );
        
        void savePath( const QString &filename, 
                       const GuiPath &guiPath, 
                       int            numLoops = 1, 
                       float          timeOffset = 0.0 );
        
        void loadPath( const QString &filename, 
                       GuiPath       &guiPath );
        
        void loadPreviousPath( GuiPath &guiPath );
        
        
    private:
        
        hydroqguielementutil::IHumanManager &humanManager_;
        QString lastSavedPathFile_;
};

}


#endif

