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
#include <orcaqgui/guiicons.h>
#include <orcaqgui2dfactory/ipathinput.h>

#include "pathdesigntablewidget.h"

using namespace std;

namespace orcaqgui2d {
    
enum ColumnDataType {
    AbsoluteTime = 0,
    Velocity,
    WaypointX,
    WaypointY,
    Heading,
    DistanceTolerance,
    HeadingTolerance,
    MaxApproachSpeed,
    MaxApproachTurnRate,
    NumColumns
};
    
PathDesignTableWidget::PathDesignTableWidget( IPathInput *pathInput, 
                                              GuiPath    &guiPath )
    : pathInput_(pathInput),
      pathFileSet_(false),
      pathFileName_("/tmp")
{
    // icons
    QPixmap sendIcon(send_xpm);
    QPixmap openIcon(fileopen_xpm);
    QPixmap savePathIcon(filesave_path_xpm);
    QPixmap goIcon(go_xpm);
    QPixmap stopIcon(stop_xpm);
    QPixmap cancelIcon(cancel_xpm);
    
    setWindowTitle("List of Waypoints");
    wpTable_ = new PathDesignTable( this, pathInput, guiPath );
    
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
    
    numLoopsSpin_ = new QSpinBox(this);
    numLoopsSpin_->setMinimum(1);
    numLoopsSpin_->setMaximum(10000);
    numLoopsSpin_->setValue(1);
    
    QVBoxLayout *globalLayout = new QVBoxLayout;
    globalLayout->addWidget(wpTable_);
    
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(loadPath);
    hLayout->addWidget(loadPreviousPath);
    hLayout->addWidget(savePath);
    hLayout->addWidget(sendPath);
    hLayout->addWidget(cancelPath);
    globalLayout->addLayout(hLayout);
    
    QHBoxLayout *hLayout2 = new QHBoxLayout;
    QLabel *label = new QLabel("<b>Number of loops:</b>");
    hLayout2->addWidget(label);
    hLayout2->addWidget(numLoopsSpin_);
    hLayout2->addStretch();
    globalLayout->addLayout(hLayout2);
    
    setLayout(globalLayout);
    this->setMinimumWidth(800);
    this->show();
}

void 
PathDesignTableWidget::refreshTable()
{
    wpTable_->computeVelocities();
    wpTable_->refreshTable();    
}

int 
PathDesignTableWidget::numberOfLoops() 
{ 
    return numLoopsSpin_->value(); 
}

void 
PathDesignTableWidget::savePathAs()
{
    QString fileName = QFileDialog::getSaveFileName(
            0,
            "Choose a filename to save under",
            pathFileName_,
            "*.txt");
    
    if (!fileName.isEmpty())
    {
        pathInput_->savePath( fileName );
        pathFileName_ = fileName;
        pathFileSet_ = true;
    }
}

void 
PathDesignTableWidget::savePath()
{
    if (!pathFileSet_)
    {   
        savePathAs();
    }
    else
    {
        pathInput_->savePath( pathFileName_ );
    }
}

void 
PathDesignTableWidget::loadPath()
{
    QString fileName = QFileDialog::getOpenFileName(
            0,
            "Choose a path file to open", 
            pathFileName_,
            "*.txt");

    if (!fileName.isEmpty())
    {
        pathInput_->loadPath( fileName );
        pathFileName_ = fileName;
        pathFileSet_ = true;
    }
}

void
PathDesignTableWidget::loadPreviousPath()
{
    pathInput_->loadPreviousPath();   
}

void 
PathDesignTableWidget::sendPath()
{
    pathInput_->sendPath();
}

void 
PathDesignTableWidget::cancelPath()
{
    pathInput_->cancelPath();
}

// ==================================================================
    
PathDesignTable::PathDesignTable( QWidget    *parent,
                                  IPathInput *pathInput,
                                  GuiPath    &guiPath )
    : QTableWidget( parent ),
      pathInput_(pathInput),
      guiPath_(guiPath),
      isLocked_(true)
{ 
    setColumnCount(NumColumns);
        
    QStringList columnHeaders;
    columnHeaders << "Abs. time [s]" << "Velocity [m/s]" << "Wp x [m]" << "Wp y [m]" << "Heading [deg]" << "Dist. tol. [m]" << "Head. tol. [deg]" << "Max App. Speed [m/s]" << "Max App. Turnrate [deg/s]";
    setHorizontalHeaderLabels( columnHeaders );
    
    QObject::connect(this,SIGNAL(cellChanged(int,int)),this,SLOT(updateDataStorage(int,int)));
    QObject::connect(this,SIGNAL(cellClicked(int,int)),this,SLOT(setWaypointFocus(int,int)));
}



void PathDesignTable::refreshTable()
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
        str.setNum(guiPath_.at(row).timeTarget,'g',4);
        item = new QTableWidgetItem(str);
        setItem(row, AbsoluteTime, item);
        
        str.setNum(velocities_[row],'g',4);
        item = new QTableWidgetItem(str);
        setItem(row, Velocity, item);
        
        str.setNum(guiPath_.at(row).position.x(),'g',4);
        item = new QTableWidgetItem(str);
        setItem(row, WaypointX, item);
        
        str.setNum(guiPath_.at(row).position.y(),'g',4);
        item = new QTableWidgetItem(str);
        setItem(row, WaypointY, item);
        
        str.setNum(guiPath_.at(row).heading/QT_ANGLE_MULTIPLIER);
        item = new QTableWidgetItem(str);
        setItem(row, Heading, item);
        
        str.setNum(guiPath_.at(row).distanceTolerance,'g',4);
        item = new QTableWidgetItem(str);
        setItem(row, DistanceTolerance, item);
        
        str.setNum(guiPath_.at(row).headingTolerance/QT_ANGLE_MULTIPLIER);
        item = new QTableWidgetItem(str);
        setItem(row, HeadingTolerance, item);
        
        str.setNum(guiPath_.at(row).maxSpeed,'g',4);
        item = new QTableWidgetItem(str);
        setItem(row, MaxApproachSpeed, item);
        
        str.setNum(guiPath_.at(row).maxTurnrate);
        item = new QTableWidgetItem(str);
        setItem(row, MaxApproachTurnRate, item);
    }
    
    isLocked_ = false;
}

void PathDesignTable::computeVelocities()
{
    velocities_.resize(1);
    velocities_[0] = 0.0;
    for (int i=1; i<guiPath_.size(); i++)
    {
        float deltaS = straightLineDist( guiPath_.at(i).position - guiPath_.at(i-1).position);
        float deltaT = guiPath_.at(i).timeTarget - guiPath_.at(i-1).timeTarget;
        // need to bound deltaT to avoid division by zero
        if (deltaT==0.0)
            deltaT=10e-6;
        velocities_.push_back(deltaS/deltaT);
    }
}

void PathDesignTable::updateDataStorage(int row, int column)
{
    if (isLocked_) return;
    
    QTableWidgetItem *item = this->item(row,column);
    
    // get a pointer to first element
    GuiWaypoint *data = guiPath_.data();
    
    switch (column)
    {
        case AbsoluteTime: 
        {       
            float currentTimeSec = data[row].timeTarget;
            float newTimeSec = item->text().toDouble();
            float deltaTime = newTimeSec-currentTimeSec;
            
            // set this row and all subsequent ones
            data[row].timeTarget = newTimeSec;
            for (int i=row+1; i<rowCount(); i++)
            {
                data[i].timeTarget = data[i].timeTarget + deltaTime;
            }
            computeVelocities();
            refreshTable();
            break;
        }
        case Velocity: 
        {       
            // first row is not editable
            if (row==0) break;
            
            // set velocity of this row
            float velocity =  item->text().toDouble();
            velocities_[row] = velocity;
            
            // update time from previous wp to this wp
            float deltaS = straightLineDist( guiPath_.at(row).position - guiPath_.at(row-1).position );
            float oldTime = data[row].timeTarget;
            // need to bound velocity to avoid division by zero
            if (velocity==0.0)
                velocity = 10e-6;
            
            float newTime = deltaS/velocity + data[row-1].timeTarget;
            
            // set new absolute time in our row
            data[row].timeTarget = newTime;
            
            // all subsequent times will be updated too
            for (int i=row+1; i<rowCount(); i++)
                data[i].timeTarget = data[i].timeTarget + (newTime-oldTime);    
            
            // finally, refresh the table
            refreshTable();
            break;
        }
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
        case Heading: 
        {       
            data[row].heading = item->text().toInt()*QT_ANGLE_MULTIPLIER;
            break;
        }
        case DistanceTolerance: 
        {       
            data[row].distanceTolerance = item->text().toDouble();
            break;
        }
        case HeadingTolerance: 
        {       
            data[row].headingTolerance = item->text().toInt()*QT_ANGLE_MULTIPLIER;
            break;
        }
        case MaxApproachSpeed: 
        {       
            data[row].maxSpeed = item->text().toDouble();
            break;
        }
        case MaxApproachTurnRate: 
        {       
            data[row].maxTurnrate = item->text().toInt();
            break;
        }
        default: 
        {
            cout << "PathDesignTable:: This column is not supported!" << endl;
        }
    } 
}

void
PathDesignTable::setWaypointFocus( int row, int column )
{
    pathInput_->setWaypointFocus( row );
}


}
