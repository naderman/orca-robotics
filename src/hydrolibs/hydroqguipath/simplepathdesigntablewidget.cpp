#include <iostream>
#include <QPainter>
#include <QDialog>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QComboBox>
#include <QShortcut>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <hydroqgui/guiicons.h>
#include <hydroqguipath/ipathinput.h>

#include "simplepathdesigntablewidget.h"

using namespace std;

namespace hydroqguipath {
    
SimplePathDesignTableWidget::SimplePathDesignTableWidget( IPathInput *pathInput,
                                              GuiPath    &guiPath )
    : pathInput_(pathInput)
{
    // icons
    QPixmap sendIcon(send_xpm);
    QPixmap openIcon(fileopen_xpm);
    QPixmap savePathIcon(filesave_path_xpm);
    QPixmap goIcon(go_xpm);
    QPixmap stopIcon(stop_xpm);
    QPixmap cancelIcon(cancel_xpm);
    
    setWindowTitle("List of Waypoints");
    wpTable_ = new SimplePathDesignTable( this, pathInput, guiPath );
    
    QPushButton *savePath = new QPushButton(savePathIcon, tr("Save Path"), this);
    QPushButton *loadPath = new QPushButton(openIcon, tr("Load Path"), this);
    QPushButton *loadPreviousPath = new QPushButton(openIcon, tr("Load Previous Path"), this);
    QObject::connect(savePath,SIGNAL(clicked()),this,SLOT(savePath()));
    QObject::connect(loadPath,SIGNAL(clicked()),this,SLOT(loadPath()));
    QObject::connect(loadPreviousPath,SIGNAL(clicked()),this,SLOT(loadPreviousPath()));
    
    QPushButton *sendPath = new QPushButton(sendIcon, tr("Send Path"), this);
    QShortcut *sendShortcut = new QShortcut(QKeySequence(tr("F11")), this );
    QPushButton *cancelPath = new QPushButton(cancelIcon, tr("Cancel Path"), this);
    QObject::connect(sendShortcut,SIGNAL(activated()),this,SLOT(sendPath()));
    QObject::connect(sendPath,SIGNAL(clicked()),this,SLOT(sendPath()));
    QObject::connect(cancelPath,SIGNAL(clicked()),this,SLOT(cancelPath()));
    
//     numLoopsSpin_ = new QSpinBox(this);
//     numLoopsSpin_->setMinimum(1);
//     numLoopsSpin_->setMaximum(10000);
//     numLoopsSpin_->setValue(1);
    
    QVBoxLayout *globalLayout = new QVBoxLayout;
    globalLayout->addWidget(wpTable_);
    
//     QHBoxLayout *hLayout = new QHBoxLayout;
    globalLayout->addWidget(loadPath);
    globalLayout->addWidget(loadPreviousPath);
    globalLayout->addWidget(savePath);
    globalLayout->addWidget(sendPath);
    globalLayout->addWidget(cancelPath);
//     globalLayout->addLayout(hLayout);
    
//     QHBoxLayout *hLayout2 = new QHBoxLayout;
//     QLabel *label = new QLabel("<b>Number of loops:</b>");
//     hLayout2->addWidget(label);
//     hLayout2->addWidget(numLoopsSpin_);
//     hLayout2->addStretch();
//     globalLayout->addLayout(hLayout2);
    
    setLayout(globalLayout);
//     this->setMinimumWidth(1000);
    this->show();
}

void 
SimplePathDesignTableWidget::refreshTable()
{
//     wpTable_->computeVelocities();
    wpTable_->refreshTable();
}

int 
SimplePathDesignTableWidget::numberOfLoops()
{ 
//     return numLoopsSpin_->value();
    return 1;
}

void 
SimplePathDesignTableWidget::savePath()
{
    pathInput_->savePath();
}

void
SimplePathDesignTableWidget::loadPath()
{
    pathInput_->loadPath();   
}

void
SimplePathDesignTableWidget::loadPreviousPath()
{
    pathInput_->loadPreviousPath();   
}

void 
SimplePathDesignTableWidget::sendPath()
{
    pathInput_->sendPath();
}

void 
SimplePathDesignTableWidget::cancelPath()
{
    pathInput_->cancelPath();
}

// ==================================================================


enum ColumnDataType {
    ApproachSpeed = 0,
    WaypointX,
    WaypointY,
    DistanceTolerance,
    NumColumns
};
    
SimplePathDesignTable::SimplePathDesignTable( QWidget    *parent,
                                  IPathInput *pathInput,
                                  GuiPath    &guiPath )
    : QTableWidget( parent ),
      pathInput_(pathInput),
      guiPath_(guiPath),
      isLocked_(true)
{ 
    setColumnCount(NumColumns);
        
    QStringList columnHeaders;
    columnHeaders << "Approach speed [m/s]" << "X [m]" << "Y [m]" << "Tolerance [m]";
    setHorizontalHeaderLabels( columnHeaders );
    
    QObject::connect(this,SIGNAL(cellChanged(int,int)),this,SLOT(updateDataStorage(int,int)));
    QObject::connect(this,SIGNAL(cellClicked(int,int)),this,SLOT(setWaypointFocus(int,int)));
    
    // This one is needed, so we cell changes with the keyboard are detected
    // Note that cellChanged refers to the cell content having changed whereas currentCellChanged does not
    QObject::connect(this,SIGNAL(currentCellChanged (int,int,int,int)),this,SLOT(setWaypointFocus(int,int,int,int)));
    
}

void SimplePathDesignTable::refreshTable()
{
    isLocked_ = true;
    
    // set the row size to the current number of waypoints
    // all entries will subsequently be overwritten, so we don't clear the contents
    int size = guiPath_.size();
    setRowCount(size);
    
    QString str;
    QTableWidgetItem *item;
    
    for (int row=0; row<size; row++)
    {
        str.setNum(guiPath_.at(row).position.x(),'g',4);
        item = new QTableWidgetItem(str);
        setItem(row, WaypointX, item);
        
        str.setNum(guiPath_.at(row).position.y(),'g',4);
        item = new QTableWidgetItem(str);
        setItem(row, WaypointY, item);

        str.setNum(guiPath_.at(row).distanceTolerance,'g',4);
        item = new QTableWidgetItem(str);
        setItem(row, DistanceTolerance, item);
        
        str.setNum(guiPath_.at(row).maxSpeed,'g',4);
        item = new QTableWidgetItem(str);
        setItem(row, ApproachSpeed, item);
    }
    
    isLocked_ = false;
}

// void SimplePathDesignTable::computeVelocities()
// {
//     velocities_.resize(1);
//     velocities_[0] = 0.0;
//     for (unsigned int i=1; i<guiPath_.size(); i++)
//     {
//         float deltaS = straightLineDist( guiPath_.at(i).position - guiPath_.at(i-1).position);
//         float deltaT = guiPath_.at(i).timeTarget - guiPath_.at(i-1).timeTarget;
//         // need to bound deltaT to avoid division by zero
//         if (deltaT==0.0)
//             deltaT=10e-6;
//         velocities_.push_back(deltaS/deltaT);
//     }
// }

void SimplePathDesignTable::updateDataStorage(int row, int column)
{
    if (isLocked_) return;
    
    QTableWidgetItem *item = this->item(row,column);
    
    // get a pointer to first element
    GuiWaypoint *data = guiPath_.data();
    
    switch (column)
    {
        case WaypointX:
        {
            data[row].position.setX( item->text().toDouble() );
            break;
        }
        case WaypointY:
        {
            data[row].position.setY( item->text().toDouble() );
            break;
        }
        case DistanceTolerance:
        {
            data[row].distanceTolerance = item->text().toDouble();
            break;
        }
        case ApproachSpeed:
        {
            data[row].maxSpeed = item->text().toDouble();
            break;
        }
        default: 
        {
            cout << "SimplePathDesignTable:: This column is not supported!" << endl;
        }
    } 
}

void 
SimplePathDesignTable::setWaypointFocus(int currentRow,
                                  int currentColumn, 
                                  int previousRow, 
                                  int previousColumn )
{
    setWaypointFocus( currentRow, currentColumn );
}

void
SimplePathDesignTable::setWaypointFocus( int row, int column )
{
    pathInput_->setWaypointFocus( row );
}


}
