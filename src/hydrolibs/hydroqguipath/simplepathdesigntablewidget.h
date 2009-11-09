/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef SIMPLEPATHDESIGN_TABLE_H
#define SIMPLEPATHDESIGN_TABLE_H

#include <vector>
#include <QWidget>
#include <QTableWidget>

#include <hydroqguipath/pathutils.h>

class QSpinBox;

namespace hydroqguielementutil {
    class IHumanManager;
}

namespace hydroqguipath {
    
class SimplePathDesignTable;
class IPathInput;
class PathDesignScreen;

//!
//! A class used to design paths by manipulating values in a table
//!
//! @author Tobias Kaupp
//! 
class SimplePathDesignTableWidget : public QWidget
{
    Q_OBJECT
    
    public:
        SimplePathDesignTableWidget ( IPathInput *pathInput,
                                      GuiPath    &guiPath,
                                      PathDesignScreen &designScreen,
                                      double maxApproachSpeedSetting,
                                      double distToleranceSetting,
                                      hydroqguielementutil::IHumanManager &humanManager );
        virtual ~SimplePathDesignTableWidget();
    
    private:
        IPathInput *pathInput_;
        SimplePathDesignTable *wpTable_;
        hydroqguipath::PathDesignScreen &designScreen_;
        void readSettings();
        void writeSettings();
        
    public slots:
        void refreshTable();
    
    private slots:
        void savePath();
        void loadPath();
        void loadPreviousPath();
//         void sendPath();
//         void cancelPath();
        void updateMaxApproachSpeedSetting(double);
        void updateDistToleranceSetting(double);
    
};

class SimplePathDesignTable : public QTableWidget
{
    Q_OBJECT
            
    public:
        SimplePathDesignTable( QWidget     *parent,
                               IPathInput  *pathInput,
                               GuiPath     &guiPath,
                               hydroqguielementutil::IHumanManager &humanManager );
        void refreshTable();
        
    private:
        
        IPathInput *pathInput_;
        GuiPath &guiPath_;
        hydroqguielementutil::IHumanManager &humanManager_;
        
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
