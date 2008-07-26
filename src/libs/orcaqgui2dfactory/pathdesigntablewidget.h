/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef PATHDESIGN_TABLE_H
#define PATHDESIGN_TABLE_H

#include <QWidget>
#include <QSpinBox>
#include <QTableWidget>

#include <orcaqgui2dfactory/pathdesignscreen.h>
#include <orcaqgui2dfactory/pathutils.h>

namespace orcaqgui2d {
    
class PathDesignTable;
class IPathInput;

class PathDesignWidget : public QWidget
{
    Q_OBJECT
    
    public:
        PathDesignWidget(IPathInput *pathInput,
                         GuiPath    &guiPath );
        int numberOfLoops() { return numLoopsSpin_->value(); };
    
    private:
        IPathInput *pathInput_;
        PathDesignTable *wpTable_;
        bool pathFileSet_;
        QString pathFileName_;
        QSpinBox *numLoopsSpin_;
        
    public slots:
        void refreshTable();
    
    private slots:
        void savePathAs();
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
        PathDesignTable( PathDesignWidget *parent,
                         IPathInput       *pathInput,
                         GuiPath          &guiPath );
        void refreshTable();
        void computeVelocities();
        
    private:
        PathDesignWidget *parent_;
        IPathInput       *pathInput_;
        
        // data which is shared with pathinput
        GuiPath &guiPath_;
        
        // this one is only local
        QVector<float> velocities_;
        
        // lock up the cellUpdate signal: it should only be emitted if the user changes cell entries
        // not if we programmatically change them (as in refreshTable)
        bool isLocked_;
    
    private slots:
        // called after user interaction
        void updateDataStorage(int row, int column);
        void setWaypointFocus(int, int);
        
};

}

#endif
