/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <sstream>
#include <QFile>
#include <QTextStream>
#include <orcaobjutil/pathfileutil.h>
#include <orcaobj/pathfollower2d.h>
#include <hydroqguielementutil/ihumanmanager.h>
#include <orcaqgui2dfactory/pathconversionutil.h>

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
PathFileHandler::savePath( const QString                &filename, 
                           const hydroqguipath::GuiPath &guiPath, 
                           int                           numLoops, 
                           float                         timeOffset ) 
{
    if (guiPath.size()==0)
    {
        humanManager_.showDialogWarning( "Path has no waypoints!" );
        return;
    }

    if (numLoops==0)
    {
        humanManager_.showDialogWarning( "Path is empty because number of loops is 0!" );
        return;
    }
    
    QFile file( filename );
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        humanManager_.showDialogError( "Cannot create file " + filename );
        return;
    }
    
    lastSavedPathFile_ = filename;
    
    // convert gui path to an orca path
    orca::Path2d orcaPath;
    guiPathToOrcaPath( guiPath, orcaPath, numLoops, timeOffset );
    
    // write to a file
    QTextStream out(&file);
    out << QString(pathfileutil::toLogString( orcaPath ).c_str());
    file.close();
    humanManager_.showStatusInformation( "Path successfully saved to " + filename );
}

void 
PathFileHandler::loadPath( const QString          &filename, 
                           hydroqguipath::GuiPath &guiPath ) 
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        humanManager_.showStatusError( "Problem opening file " + filename );
        return;
    }
        
    orca::Path2d orcaPath;
    orca::Waypoint2d wp;
        
    QTextStream in(&file);
    while (!in.atEnd()) 
    {
        stringstream ss( in.readLine().toStdString() );
        pathfileutil::fromLogString( ss, wp );
        orcaPath.push_back(wp);
    }
    file.close();
        
    orcaPathToGuiPath( orcaPath, guiPath );
        
    humanManager_.showStatusInformation( "Successfully loaded file " + filename );
}

void 
PathFileHandler::loadPreviousPath( hydroqguipath::GuiPath &guiPath )
{
    if (lastSavedPathFile_!="") 
    {
        loadPath( lastSavedPathFile_, guiPath );    
    } 
    else 
    {
        humanManager_.showStatusWarning( "No path saved yet!" ); 
    }
}
    
}
