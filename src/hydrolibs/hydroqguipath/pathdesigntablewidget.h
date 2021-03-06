/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef PATHDESIGN_TABLE_H
#define PATHDESIGN_TABLE_H

#include <vector>
#include <QWidget>
#include <QTableWidget>

#include <hydroqguipath/pathutils.h>

class QSpinBox;

namespace hydroqguipath {
    
class PathDesignTable;
class IPathInput;

//!
//! A class used to design paths by manipulating values in a table
//!
//! @author Tobias Kaupp
//! 
class PathDesignTableWidget : public QWidget
{
    Q_OBJECT
    
    public:
        PathDesignTableWidget ( IPathInput *pathInput,
                                GuiPath    &guiPath );
        int numberOfLoops();
    
    private:
        IPathInput *pathInput_;
        PathDesignTable *wpTable_;
        QSpinBox *numLoopsSpin_;
        
    public slots:
        void refreshTable();
    
    private slots:
        void savePath();
        void loadPath();
        void loadPreviousPath();
        void sendPath();
        void cancelPath();
    
};

class PathDesignTable : public QTableWidget
{
    Q_OBJECT
            
    public:
        PathDesignTable( QWidget     *parent,
                         IPathInput  *pathInput,
                         GuiPath     &guiPath );
        void refreshTable();
        void computeVelocities();
        
    private:
        
        IPathInput *pathInput_;
        GuiPath &guiPath_;
        
        // not part of the path representation
        std::vector<float> velocities_;
        
        // lock up the cellUpdate signal: it should only be emitted if the user changes cell entries
        // not if we programmatically change them (as in refreshTable)
        bool isLocked_;
    
    private slots:
        
        // called after user interaction
        void updateDataStorage(int row, int column);
        void setWaypointFocus(int row, int column);
        void setWaypointFocus(int currentRow, int currentColumn, int previousRow, int previousColumn);
        
};

}

#endif
