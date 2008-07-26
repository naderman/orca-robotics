#include "pathplannerinput.h"

namespace orcaqgui2d {

PathPlannerInput::PathPlannerInput( QObject    *parent,
                      WaypointSettings         *wpSettings,
                      hydroqguielementutil::IHumanManager &humanManager )
{
    guiPath_.reset( new GuiPath() );
    
    pathDesignScreen_.reset( new PathDesignScreen( *guiPath_.get(), wpSettings, humanManager ) );
}

orca::PathPlanner2dTask
PathPlannerInput::getTask() const
{       
    orca::PathPlanner2dTask task;
    guiPathToOrcaPath( *guiPath_.get(), task.coarsePath );
    return task;
}
            
            
}
