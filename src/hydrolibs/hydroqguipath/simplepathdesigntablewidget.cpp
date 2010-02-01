#include <iostream>
#include <QTextStream>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSettings>
#include <QHeaderView>
#include <QCloseEvent>
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
    const int buttonSizeX = 100;
    const int buttonSizeY = 30;
    const int buttonSpacing = 10;

    // icons
    QPixmap openIcon(fileopen_xpm);
    QPixmap savePathIcon(filesave_path_xpm);
    QPixmap sendIcon(send_xpm);
    QPixmap cancelIcon(cancel_xpm);
    
    setWindowTitle("Path Design");
    wpTable_ = new SimplePathDesignTable( this, pathInput, guiPath, humanManager );
    
    QPushButton *savePath = new QPushButton(savePathIcon, tr("Save As..."), this);
    savePath->setFixedSize(buttonSizeX, buttonSizeY );
    QPushButton *loadPath = new QPushButton(openIcon, tr("Load..."), this);
    loadPath->setFixedSize(buttonSizeX, buttonSizeY );
    QPushButton *loadPreviousPath = new QPushButton(openIcon, tr("Previous"), this);
    loadPreviousPath->setFixedSize(buttonSizeX, buttonSizeY );
    QPushButton *sendPath = new QPushButton(sendIcon, tr("Send"), this);
    sendPath->setFixedSize(1.5*buttonSizeX,buttonSizeY);
    QPushButton *cancelPath = new QPushButton(cancelIcon, tr("Discard"), this);
    cancelPath->setFixedSize(1.5*buttonSizeX, buttonSizeY );
    QObject::connect(savePath,SIGNAL(clicked()),this,SLOT(savePath()));
    QObject::connect(loadPath,SIGNAL(clicked()),this,SLOT(loadPath()));
    QObject::connect(loadPreviousPath,SIGNAL(clicked()),this,SLOT(loadPreviousPath()));
    QObject::connect(sendPath,SIGNAL(clicked()),this,SLOT(sendPath()));
    QObject::connect(cancelPath,SIGNAL(clicked()),this,SLOT(cancelPath()));

    QVBoxLayout *globalLayout = new QVBoxLayout;
    globalLayout->addWidget(wpTable_);

    QLabel *speedLabel = new QLabel("Approach speed [m/s]:", this);
    QDoubleSpinBox *maxApproachSpeedSpin = new QDoubleSpinBox(this);
    maxApproachSpeedSpin->setFixedSize ( 70, 22 );
    maxApproachSpeedSpin->setMinimum(0.0);
    maxApproachSpeedSpin->setMaximum(3.0);
    maxApproachSpeedSpin->setValue(maxApproachSpeedSetting);
    maxApproachSpeedSpin->setSingleStep(0.1);
    QObject::connect(maxApproachSpeedSpin,SIGNAL(valueChanged(double)),this,SLOT(updateMaxApproachSpeedSetting(double)));

    QLabel *tolLabel = new QLabel("Distance tolerance [m]:", this);
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
    globalLayout->addLayout( grid );
    // the second column can be stretched much more than the first (which stays fixed)
    grid->setColumnStretch(1,1000);

    QHBoxLayout *hBox1 = new QHBoxLayout;
    hBox1->setSpacing( 2*buttonSpacing );
    hBox1->addWidget( sendPath );
    hBox1->addWidget( cancelPath );
    hBox1->addStretch();
    globalLayout->addLayout( hBox1 );

    QHBoxLayout *hBox2 = new QHBoxLayout;
    hBox2->setSpacing( buttonSpacing );
    hBox2->addWidget( loadPath );
    hBox2->addWidget( loadPreviousPath );
    hBox2->addWidget( savePath );
    hBox2->addStretch();
    globalLayout->addLayout( hBox2 );

    setLayout( globalLayout );

    readSettings();

    this->show();
}

SimplePathDesignTableWidget::~SimplePathDesignTableWidget()
{
//     cout << "SimplePathDesignTableWidget: destructor" << endl;
    writeSettings();
}

void 
SimplePathDesignTableWidget::closeEvent( QCloseEvent * event )
{
//     cout << "SimplePathDesignTableWidget: closeEvent" << endl;
    pathInput_->cancelPath();
    event->accept();
}

void SimplePathDesignTableWidget::readSettings()
{
    QSettings settings;
    settings.beginGroup("SimplePathDesignTableWidget");
    resize(settings.value("size", QSize(400, 400)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();
}

void SimplePathDesignTableWidget::writeSettings()
{
    QSettings settings;
    settings.beginGroup("SimplePathDesignTableWidget");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}

void 
SimplePathDesignTableWidget::refreshTable()
{
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
    this->horizontalHeader()->setStretchLastSection( true );
    
    QObject::connect(this,SIGNAL(cellChanged(int,int)),this,SLOT(updateDataStorage(int,int)));
    QObject::connect(this,SIGNAL(cellClicked(int,int)),this,SLOT(setWaypointFocus(int,int)));
    
    // This one is needed, so we cell changes with the keyboard are detected
    // Note that cellChanged refers to the cell content having changed whereas currentCellChanged does not
    QObject::connect(this,SIGNAL(currentCellChanged (int,int,int,int)),this,SLOT(setWaypointFocus(int,int,int,int)));
    
}

SimplePathDesignTable::~SimplePathDesignTable()
{
//     cout << "SimplePathDesignTable: destructor" << endl;
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
