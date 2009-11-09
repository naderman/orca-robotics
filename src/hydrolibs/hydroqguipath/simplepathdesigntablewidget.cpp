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
#include <QSettings>
#include <sstream>
#include <hydroqgui/guiicons.h>
#include <hydroqguipath/ipathinput.h>
#include <hydroqguipath/pathdesignscreen.h>
#include <hydroqguielementutil/ihumanmanager.h>

#include "simplepathdesigntablewidget.h"

using namespace std;

namespace hydroqguielementutil {
    class IHumanManager;
}

namespace hydroqguipath {

SimplePathDesignTableWidget::SimplePathDesignTableWidget( IPathInput *pathInput,
                                                          GuiPath    &guiPath,
                                                          hydroqguipath::PathDesignScreen &designScreen,
                                                          double maxApproachSpeedSetting,
                                                          double distToleranceSetting,
                                                          hydroqguielementutil::IHumanManager &humanManager  )
    : pathInput_(pathInput),
      designScreen_(designScreen)
{
    // icons
    QPixmap openIcon(fileopen_xpm);
    QPixmap savePathIcon(filesave_path_xpm);
    
    setWindowTitle("Path Design");
    wpTable_ = new SimplePathDesignTable( this, pathInput, guiPath, humanManager );
    
    QPushButton *savePath = new QPushButton(savePathIcon, tr("Save Path"), this);
    QPushButton *loadPath = new QPushButton(openIcon, tr("Load Path"), this);
    QPushButton *loadPreviousPath = new QPushButton(openIcon, tr("Load Previous Path"), this);
    QObject::connect(savePath,SIGNAL(clicked()),this,SLOT(savePath()));
    QObject::connect(loadPath,SIGNAL(clicked()),this,SLOT(loadPath()));
    QObject::connect(loadPreviousPath,SIGNAL(clicked()),this,SLOT(loadPreviousPath()));
    
    QVBoxLayout *globalLayout = new QVBoxLayout;
    globalLayout->addWidget(wpTable_);

    QLabel *speedLabel = new QLabel("Approach speed default [m/s]:", this);
    QDoubleSpinBox *maxApproachSpeedSpin = new QDoubleSpinBox(this);
    maxApproachSpeedSpin->setFixedSize ( 70, 22 );
    maxApproachSpeedSpin->setMinimum(0.0);
    maxApproachSpeedSpin->setMaximum(3.0);
    maxApproachSpeedSpin->setValue(maxApproachSpeedSetting);
    maxApproachSpeedSpin->setSingleStep(0.1);
    QObject::connect(maxApproachSpeedSpin,SIGNAL(valueChanged(double)),this,SLOT(updateMaxApproachSpeedSetting(double)));

    QLabel *tolLabel = new QLabel("Distance tolerance default [m]:", this);
    QDoubleSpinBox *maxDistToleranceSpin = new QDoubleSpinBox(this);
    maxDistToleranceSpin->setFixedSize ( 70, 22 );
    maxDistToleranceSpin->setMinimum(0.0);
    maxDistToleranceSpin->setMaximum(20.0);
    maxDistToleranceSpin->setValue(distToleranceSetting);
    maxDistToleranceSpin->setSingleStep(0.1);
    QObject::connect(maxDistToleranceSpin,SIGNAL(valueChanged(double)),this,SLOT(updateDistToleranceSetting(double)));

    QGridLayout *grid = new QGridLayout;
    grid->addWidget( speedLabel, 0, 0, Qt::AlignLeft);
    grid->addWidget( maxApproachSpeedSpin, 0, 1, Qt::AlignLeft);
    grid->addWidget( tolLabel, 1, 0, Qt::AlignLeft );
    grid->addWidget( maxDistToleranceSpin, 1, 1, Qt::AlignLeft);
    grid->addWidget( loadPath, 2, 0);
    grid->addWidget( loadPreviousPath, 3, 0);
    grid->addWidget( savePath, 4, 0);

    globalLayout->addLayout(grid);

    // the second column can be stretched much more than the first (which stays fixed)
    grid->setColumnStretch(1,1000);
    setLayout( globalLayout );

    readSettings();
    this->show();
}

SimplePathDesignTableWidget::~SimplePathDesignTableWidget()
{
    writeSettings();
}

void SimplePathDesignTableWidget::readSettings()
{
    QSettings settings("Marathon Robotics", "RoverControl");
    QPoint pos = settings.value("simplepathdesigntable/pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("simplepathdesigntable/size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);
}

void SimplePathDesignTableWidget::writeSettings()
{
    QSettings settings("Marathon Robotics", "RoverControl");
    settings.setValue("simplepathdesigntable/pos", pos());
    settings.setValue("simplepathdesigntable/size", size());
}

void 
SimplePathDesignTableWidget::refreshTable()
{
//     wpTable_->computeVelocities();
    wpTable_->refreshTable();
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

// void 
// SimplePathDesignTableWidget::sendPath()
// {
//     pathInput_->sendPath();
// }

// void 
// SimplePathDesignTableWidget::cancelPath()
// {
//     pathInput_->cancelPath();
// }


void 
SimplePathDesignTableWidget::updateMaxApproachSpeedSetting(double value)
{
    designScreen_.updateMaxApproachSpeedSetting( value );
}

void 
SimplePathDesignTableWidget::updateDistToleranceSetting(double value)
{
    designScreen_.updateDistToleranceSettingSetting( value );
}

// ==================================================================


enum ColumnDataType {
    WaypointX = 0,
    WaypointY,
    ApproachSpeed,
    DistanceTolerance,
    NumColumns
};

void align( QTableWidgetItem *item )
{
    item->setTextAlignment( Qt::AlignRight );
}

void formatDoubleItem( QTableWidgetItem *item )
{
    QString str;
    QTextStream ss(&str);
    ss.setRealNumberNotation( QTextStream::FixedNotation );
    ss.setRealNumberPrecision(1);
    ss << item->text().toDouble();
    item->setText( *ss.string() );
}
    
SimplePathDesignTable::SimplePathDesignTable( QWidget *parent,
                                              IPathInput *pathInput,
                                              GuiPath &guiPath,
                                              hydroqguielementutil::IHumanManager &humanManager )
    : QTableWidget( parent ),
      pathInput_(pathInput),
      guiPath_(guiPath),
      humanManager_(humanManager),
      isLocked_(true)
{ 
    setColumnCount(NumColumns);
        
    QStringList columnHeaders;
    columnHeaders << "X [m]" << "Y [m]" << "Approach\nspeed [m/s]" << "Distance\ntolerance [m]";
    setHorizontalHeaderLabels( columnHeaders );

    resizeColumnsToContents();
    
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
        str.setNum(guiPath_.at(row).position.x(),'f',1);
        item = new QTableWidgetItem(str);
        align( item );
        setItem(row, WaypointX, item);
        
        str.setNum(guiPath_.at(row).position.y(),'f',1);
        item = new QTableWidgetItem(str);
        align( item );
        setItem(row, WaypointY, item);

        str.setNum(guiPath_.at(row).distanceTolerance,'f',1);
        item = new QTableWidgetItem(str);
        align( item );
        setItem(row, DistanceTolerance, item);
        
        str.setNum(guiPath_.at(row).maxSpeed,'f',1);
        item = new QTableWidgetItem(str);
        align( item );
        setItem(row, ApproachSpeed, item);
    }
    
    isLocked_ = false;
}

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
            double newDistTol = item->text().toDouble();
            if (newDistTol<0.0)
            {
                humanManager_.showDialogError( "Distance tolerance cannot be negative");
                stringstream ss; ss << data[row].distanceTolerance;
                item->setText( QString(ss.str().c_str()) );
            }
            else
            {
                data[row].distanceTolerance = newDistTol;
            }
            break;
        }
        case ApproachSpeed:
        {
            double newAppSpeed = item->text().toDouble();
            if (newAppSpeed<0.0)
            {
                humanManager_.showDialogError( "Approach speed cannot be negative");
                stringstream ss; ss << data[row].maxSpeed;
                item->setText( QString(ss.str().c_str()) );
            }
            else
            {
                data[row].maxSpeed = newAppSpeed;
            }
            break;
        }
        default: 
        {
            cout << "SimplePathDesignTable:: This column is not supported!" << endl;
            return;
        }
    }
    
    // this fct formats the appearance of the entries in the table
    formatDoubleItem( item );
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
