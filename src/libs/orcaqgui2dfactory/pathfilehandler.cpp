/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <sstream>
#include <QFile>
#include <QTextStream>
#include <orca/pathfollower2d.h>
#include <orcalogfactory/logstringutils.h>
#include "pathfilehandler.h"

using namespace std;

namespace orcaqgui2d {
    
PathFileHandler::PathFileHandler( hydroqguielementutil::IHumanManager &humanManager,
                                  const QString                       &lastSavedPathFile )
    : humanManager_(humanManager),
      lastSavedPathFile_(lastSavedPathFile)
{
}
        
    
void 
PathFileHandler::savePath( const QString &filename, 
                           const GuiPath &guiPath, 
                           int            numLoops, 
                           float          timeOffset ) 
{
    if (guiPath.size()==0)
    {
        humanManager_.showDialogMsg(hydroqguielementutil::IHumanManager::Warning, "Path has no waypoints!");
        return;
    }
    
    QFile file( filename );
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        humanManager_.showDialogMsg(hydroqguielementutil::IHumanManager::Error,
                                  "Cannot create file " + filename );
        return;
    }
    
    lastSavedPathFile_ = filename;
    cout << "TRACE(PathFileHandler): lastSavedPathFile_ " << lastSavedPathFile_.toStdString() << endl;
    
    // convert gui path to an orca path
    orca::Path2d orcaPath;
    guiPathToOrcaPath( guiPath, orcaPath, numLoops, timeOffset );
    
    // write to a file
    QTextStream out(&file);
    out << QString(orcalogfactory::toLogString( orcaPath ).c_str());
    file.close();
    humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Information, "Path successfully saved to " + filename );
}

void 
PathFileHandler::loadPath( const QString &filename, GuiPath &guiPath ) 
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Error, "Problem opening file " + filename );
        return;
    }
        
    orca::Path2d orcaPath;
    orca::Waypoint2d wp;
        
    QTextStream in(&file);
    while (!in.atEnd()) 
    {
        stringstream ss( in.readLine().toStdString() );
        orcalogfactory::fromLogString( ss, wp );
        orcaPath.push_back(wp);
    }
    file.close();
        
    orcaPathToGuiPath( orcaPath, guiPath );
        
    humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Information, "Successfully loaded file " + filename );
}

void 
PathFileHandler::loadPreviousPath( GuiPath &guiPath )
{
    cout << "TRACE(PathFileHandler): lastSavedPathFile_ " << lastSavedPathFile_.toStdString() << endl;
    if (lastSavedPathFile_!="") 
    {
        loadPath( lastSavedPathFile_, guiPath );    
    } 
    else 
    {
        humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Warning, "No path saved yet!" ); 
    }
}


// void 
// PathDesignScreen::savePath( const QString &filename, int numLoops )
// {  
//     if (guiPath_.size()==0)
//     {
//         humanManager_.showDialogMsg(hydroqguielementutil::IHumanManager::Warning, "Path has no waypoints!");
//         return;
//     }
//     
//     QFile file(filename);
//     if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
//     {
//         humanManager_.showDialogMsg(hydroqguielementutil::IHumanManager::Error,
//                                   "Cannot create file " + filename );
//         return;
//     }
//     
//     lastSavedPathFile_ = filename;
//     
//     // offset time if we have several loops
//     const float timeOffset = guiPath_.last().timeTarget + secondsToCompleteLoop();
// 
//     // convert gui path to an orca path
//     orca::Path2d orcaPath;
//     guiPathToOrcaPath( guiPath_, orcaPath, numLoops, timeOffset );
// 
//     // write to a file
//     QTextStream out(&file);
//     out << QString(orcalogfactory::toLogString( orcaPath ).c_str());
//     file.close();
//     humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Information, "Path successfully saved to " + filename );
// }


// void PathDesignScreen::loadPath( const QString& filename )
// {    
//     resizeData(0);
//     
//     QFile file(filename);
//     if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
//     {
//         humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Error, "Problem opening file " + filename );
//         return;
//     }
//     
//     orca::Path2d orcaPath;
//     orca::Waypoint2d wp;
//     
//     QTextStream in(&file);
//     while (!in.atEnd()) 
//     {
//         stringstream ss( in.readLine().toStdString() );
//         orcalogfactory::fromLogString( ss, wp );
//         orcaPath.push_back(wp);
//     }
//     file.close();
//     
//     orcaPathToGuiPath( orcaPath, guiPath_ );
//     
//     humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Information, "Successfully loaded file " + filename );
//     emit guiPathChanged();
// //     wpWidget_->refreshTable();
// }

// void PathDesignScreen::loadPreviousPath()
// {
//     if (lastSavedPathFile_!="") {
//         loadPath( lastSavedPathFile_ );    
//     } else {
//         humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Warning, "No path saved yet!" ); 
//     }
//     
// }
    
}
