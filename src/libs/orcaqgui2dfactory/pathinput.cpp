/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <cmath>
 
#include <orcaice/orcaice.h>
#include <orcaqgui/guiicons.h>
#include <orcaqgui2d/paintutils.h>
#include <orcaqgui2dfactory/waypointdialog.h>

#include <QPainter>
#include <QString>
#include <QDialog>
#include <QFile>
#include <QTextStream>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QComboBox>

#include "pathinput.h"

using namespace std;
using namespace orcaqgui;

namespace orcaqgui2d {
    
enum ColumnDataType {
    AbsoluteTime = 0,
    WaitingTime,
    Velocity,
    Behaviour,
    WaypointX,
    WaypointY,
    Heading,
    DistanceTolerance,
    HeadingTolerance,
    MaxApproachSpeed,
    MaxApproachTurnRate,
    NumColumns
};


float straightLineDist( QPointF line )
{
    return sqrt(line.x()*line.x() + line.y()*line.y());
}

WpWidget::WpWidget( PathInput *pathInput,
                    QPolygonF *waypoints, 
                    QVector<int> *headings,
                    QVector<float> *times,
                    QVector<float> *waitingTimes,
                    QVector<float> *distTolerances,
                    QVector<int> *headingTolerances,
                    QVector<float> *maxSpeeds,
                    QVector<int> *maxTurnrates )
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
    wpTable_ = new WpTable( this, pathInput, waypoints, headings, times, waitingTimes, distTolerances, headingTolerances, maxSpeeds, maxTurnrates );
    
    QPushButton *generatePath = new QPushButton(tr("Generate Full Path"), this);
    QPushButton *savePath = new QPushButton(savePathIcon, tr("Save Path"), this);
    QPushButton *loadPath = new QPushButton(openIcon, tr("Load Path"), this);
    QPushButton *loadPreviousPath = new QPushButton(openIcon, tr("Load Previous Path"), this);
    QObject::connect(generatePath,SIGNAL(clicked()),pathInput,SLOT(generateFullPath()));
    QObject::connect(savePath,SIGNAL(clicked()),this,SLOT(savePath()));
    QObject::connect(loadPath,SIGNAL(clicked()),this,SLOT(loadPath()));
    QObject::connect(loadPreviousPath,SIGNAL(clicked()),pathInput,SLOT(loadPreviousPath()));
    
    QPushButton *sendPath = new QPushButton(sendIcon, tr("Send Path"), this);
    QPushButton *cancelPath = new QPushButton(cancelIcon, tr("Cancel Path"), this);
    QObject::connect(sendPath,SIGNAL(clicked()),pathInput,SIGNAL(sendPathClicked()));
    QObject::connect(cancelPath,SIGNAL(clicked()),pathInput,SIGNAL(cancelPathClicked()));
    
    numLoopsSpin_ = new QSpinBox(this);
    numLoopsSpin_->setMinimum(1);
    numLoopsSpin_->setMaximum(10000);
    numLoopsSpin_->setValue(1);
    
    QVBoxLayout *globalLayout = new QVBoxLayout;
    globalLayout->addWidget(wpTable_);
    
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(generatePath);
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

void WpWidget::refreshTable()
{
    wpTable_->computeVelocities();
    wpTable_->refreshTable();    
}

QString WpWidget::getBehaviour( int row ) 
{ 
    return wpTable_->getBehaviour( row );
}

void 
WpWidget::savePathAs()
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
WpWidget::savePath()
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
WpWidget::loadPath()
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
    
WpTable::WpTable( QWidget *parent,
                 PathInput *pathInput,
                 QPolygonF *waypoints, 
                 QVector<int> *headings,
                 QVector<float> *times,
                 QVector<float> *waitingTimes,
                 QVector<float> *distTolerances,
                 QVector<int> *headingTolerances,
                 QVector<float> *maxSpeeds,
                 QVector<int> *maxTurnrates)
    : QTableWidget( parent ),
      pathInput_(pathInput),
      waypoints_(waypoints),
      headings_(headings),
      times_(times),
      waitingTimes_(waitingTimes),
      distTolerances_(distTolerances),
      headingTolerances_(headingTolerances),
      maxSpeeds_(maxSpeeds),
      maxTurnrates_(maxTurnrates),
      isLocked_(true)
{ 
    setColumnCount(NumColumns);
        
    QStringList columnHeaders;
    columnHeaders << "Abs. time [s]" << "Waiting time [s]" << "Velocity [m/s]" << "Behaviour"
                  << "Wp x [m]" << "Wp y [m]" << "Heading [deg]" << "Dist. tol. [m]" << "Head. tol. [deg]" 
                  << "Max App. Speed [m/s]" << "Max App. Turnrate [deg/s]";
    setHorizontalHeaderLabels( columnHeaders );
    
    QObject::connect(this,SIGNAL(cellChanged(int,int)),this,SLOT(updateDataStorage(int,int)));
    QObject::connect(this,SIGNAL(cellClicked(int,int)),pathInput,SLOT(setWaypointFocus(int,int)));
}

void WpTable::refreshTable()
{
    isLocked_ = true;
    
    // set the row size to the current number of waypoints
    // all entries will subsequently be overwritten, so we don't clear the contents
    int size = waypoints_->size();
    setRowCount(size);
    
    QString str;
    QTableWidgetItem *item;
    
//     computeVelocities();
    
    for (int row=0; row<size; row++)
    {
        str.setNum(times_->at(row),'g',4);
        item = new QTableWidgetItem(str);
        setItem(row, AbsoluteTime, item);
        
        str.setNum(waitingTimes_->at(row),'g',4);
        item = new QTableWidgetItem(str);
        setItem(row, WaitingTime, item);
        
        str.setNum(velocities_[row],'g',4);
        item = new QTableWidgetItem(str);
        setItem(row, Velocity, item);
             
        // The behaviours are special: we don't keep a data storage for them.
        // We'll read their contents whenever the full path is generated.
        // Here, we check whether the combo already exists.
        QComboBox *combo = dynamic_cast<QComboBox*>(cellWidget(row,Behaviour));
        if (combo==NULL)
        {
            combo = new QComboBox(this);
            QStringList strList;
            strList << "Stationary" << "LeftRight" << "RightLeft" << "Turn360";
            combo->addItems(strList);
            setCellWidget(row, Behaviour, combo);
        }
        if (waitingTimes_->at(row)==0.0) combo->setEnabled(false);
        
        str.setNum(waypoints_->at(row).x(),'g',4);
        item = new QTableWidgetItem(str);
        setItem(row, WaypointX, item);
        
        str.setNum(waypoints_->at(row).y(),'g',4);
        item = new QTableWidgetItem(str);
        setItem(row, WaypointY, item);
        
        str.setNum(headings_->at(row)/16);
        item = new QTableWidgetItem(str);
        setItem(row, Heading, item);
        
        str.setNum(distTolerances_->at(row),'g',4);
        item = new QTableWidgetItem(str);
        setItem(row, DistanceTolerance, item);
        
        str.setNum(headingTolerances_->at(row)/16);
        item = new QTableWidgetItem(str);
        setItem(row, HeadingTolerance, item);
        
        str.setNum(maxSpeeds_->at(row),'g',4);
        item = new QTableWidgetItem(str);
        setItem(row, MaxApproachSpeed, item);
        
        str.setNum(maxTurnrates_->at(row));
        item = new QTableWidgetItem(str);
        setItem(row, MaxApproachTurnRate, item);
    }
    
    isLocked_ = false;
}

void WpTable::computeVelocities()
{
    velocities_.resize(1);
    velocities_[0] = 0.0;
    for (int i=1; i<waypoints_->size(); i++)
    {
        float deltaS = straightLineDist( waypoints_->at(i) - waypoints_->at(i-1));
        float deltaT = times_->at(i) - (times_->at(i-1) + waitingTimes_->at(i-1));
        velocities_.push_back(deltaS/deltaT);
    }
}

// void WpTable::computeAbsTimes()
// {
//     velocities_.resize(1);
//     velocities_[0] = 0.0;
//     for (int i=1; i<waypoints_->size(); i++)
//     {
//         float deltaS = straightLineDist( waypoints_->at(i) - waypoints_->at(i-1));
//         float deltaT = times_->at(i) - (times_->at(i-1) + waitingTimes_->at(i-1));
//         velocities_.push_back(deltaS/deltaT);
//     }
// }

QString WpTable::getBehaviour( int row )
{
    QComboBox *combo = dynamic_cast<QComboBox*>(cellWidget(row,Behaviour));
    if (combo==NULL)
    {
        cout << "ERROR(pathinput.cpp): Requested combo doesn't exist!" << endl;
        return "";
    }
    return combo->currentText();
}

void WpTable::updateDataStorage(int row, int column)
{
    if (isLocked_) return;
    
    QTableWidgetItem *item = this->item(row,column);
    
    switch (column)
    {
        case AbsoluteTime: 
        {       
            // get a pointer to first element
            float *data = times_->data();
            
            float currentTimeSec = data[row];
            float newTimeSec = item->text().toDouble();
            float deltaTime = newTimeSec-currentTimeSec;
            
            // set this row and all subsequent ones
            data[row] = newTimeSec;
            for (int i=row+1; i<rowCount(); i++)
            {
                data[i] = data[i] + deltaTime;
            }
            computeVelocities();
            refreshTable();
            break;
        }
        case WaitingTime: 
        {       
            float waitingTime =  item->text().toDouble();
            if (waitingTime>0.0) {
                cellWidget(row,Behaviour)->setEnabled(true);
            } else {
                cellWidget(row,Behaviour)->setEnabled(false);
            }
            float *data = waitingTimes_->data();
            data[row] = waitingTime;
            // update all subsequent times
            data = times_->data();
            for (int i=row+1; i<rowCount(); i++)
            {
                data[i] = data[i] + waitingTime;    
            }
            refreshTable();
            break;
        }
        case Velocity: 
        {       
            // first row is not editable
            if (row==0) break;
            float velocity =  item->text().toDouble();
            velocities_[row] = velocity;
            
            // update time to get to here
            float *data = times_->data();
            float deltaS = straightLineDist( waypoints_->at(row) - waypoints_->at(row-1) );
            float oldTime = data[row];
            float newTime = deltaS/velocity + (data[row-1] + waitingTimes_->at(row-1));
            // set new absolute time in our row
            data[row] = newTime;
            //all subsequent times will be updated too
            for (int i=row+1; i<rowCount(); i++)
            {
                data[i] = data[i] + (newTime-oldTime);    
            }
            refreshTable();
            break;
        }
        case Behaviour:
        {
            // This one is never called since there is not item emitting the cellChanged signal
            // We don't keep a permanent data storage for behaviours, just read them when a full path is generated   
            break;
        }
        case WaypointX: 
        {       
            QPointF *data = waypoints_->data();
            data[row].setX( item->text().toDouble() );
            break;
        }
        case WaypointY: 
        {       
            QPointF *data = waypoints_->data();
            data[row].setY( item->text().toDouble() );
            break;
        }
        case Heading: 
        {       
            int *data = headings_->data();
            data[row] = item->text().toInt()*16;
            break;
        }
        case DistanceTolerance: 
        {       
            float *data = distTolerances_->data();
            data[row] = item->text().toDouble();
            break;
        }
        case HeadingTolerance: 
        {       
            int *data = headingTolerances_->data();
            data[row] = item->text().toInt()*16;
            break;
        }
        case MaxApproachSpeed: 
        {       
            float *data = maxSpeeds_->data();
            data[row] = item->text().toDouble();
            break;
        }
        case MaxApproachTurnRate: 
        {       
            int *data = maxTurnrates_->data();
            data[row] = item->text().toInt();
            break;
        }
        default: 
        {
            cout << "WpTable:: This column is currently not supported!" << endl;
        }
    }
        
    
}
    
PathInput::PathInput( QObject *parent, WaypointSettings *wpSettings, orcaqgui::IHumanManager *humanManager, QString lastSavedPathFile )
    : wpSettings_(wpSettings),
      humanManager_(humanManager),      
      waypointInFocus_(-1),
      lastSavedPathFile_(lastSavedPathFile)
{   
    wpWidget_ = new WpWidget( this,
                            &waypoints_,
                            &headings_, 
                            &times_,
                            &waitingTimes_,
                            &distTolerances_,
                            &headingTolerances_,
                            &maxSpeeds_,
                            &maxTurnrates_);
    
    QObject::connect(this,SIGNAL(sendPathClicked()),parent,SLOT(send()));
    QObject::connect(this,SIGNAL(cancelPathClicked()),parent,SLOT(cancel()));
}


PathInput::~PathInput() 
{ 
    delete wpWidget_; 
}  


void PathInput::setWaypointFocus(int row, int column)
{
    waypointInFocus_ = row;    
}

void
PathInput::resizeData( int index )
{
    waypoints_.resize( index );
    headings_.resize( index );
    distTolerances_.resize( index );
    headingTolerances_.resize( index );
    maxSpeeds_.resize( index );
    maxTurnrates_.resize( index );
    times_.resize( index );
    waitingTimes_.resize( index );
}


void PathInput::paint( QPainter *p )
{
    QMatrix wm = p->matrix(); 
    wmInv_ = wm.inverted();
    
    if ( waypoints_.isEmpty() ) return;
    
    const float PATH_WIDTH = 0.05;
    
    QColor fillColor;
    QColor drawColor;
    
    if (useTransparency_) {
        fillColor = getTransparentVersion(Qt::green);
    } else {
        fillColor=Qt::green;
    }
    
    for ( int i=0; i<waypoints_.size(); i++)
    {
        p->save();
        p->translate( waypoints_[i].x(), waypoints_[i].y() );    // move to point

        if (i==waypointInFocus_) {
            drawColor = Qt::black;
        } else {
            drawColor = fillColor;
        }
        
        paintWaypoint( p, 
                       fillColor,
                       drawColor, 
                       headings_[i],
                       distTolerances_[i], 
                       headingTolerances_[i] );

        p->restore();
    }
    
    // ===== draw the waypoint in focus again, to be able to see the edge =======
    if ( (waypointInFocus_!=-1) && (waypoints_.size()>0) )
    {
        p->save();

        p->translate( waypoints_[waypointInFocus_].x(), waypoints_[waypointInFocus_].y() );    // move to point
        drawColor = Qt::black;
        paintWaypoint( p, 
                        fillColor,
                        drawColor, 
                        headings_[waypointInFocus_],
                        distTolerances_[waypointInFocus_], 
                        headingTolerances_[waypointInFocus_] );
        p->restore();
    }
    
    // draw connections between them
    if ( waypoints_.size()>1 )
    {
        p->setPen( QPen( fillColor, PATH_WIDTH ) );
        p->setBrush ( Qt::NoBrush );

        for ( int i=1; i<waypoints_.size(); ++i)
        {
            p->drawLine(waypoints_[i],waypoints_[i-1]);
        }
    }
}

void PathInput::updateWpSettings( WaypointSettings* wpSettings )
{
    wpSettings_ = wpSettings;   
}

void PathInput::processPressEvent( QMouseEvent* e)
{
    mouseDownPnt_ = e->pos();
    mouseMovePnt_ = mouseDownPnt_;  // initialisation of mouseMovePnt_ to same position   
}

void PathInput::processReleaseEvent( QMouseEvent* e)
{
    mouseUpPnt_ = e->pos();
    int button =  e->button();

    QPointF waypoint = wmInv_.map( mouseUpPnt_ );
    //cout << "mouseUpPnt_,waypoint: (" << mouseUpPnt_.x() << "," << mouseUpPnt_.y() <<")  (" << waypoint.x() << ","<< waypoint.y() << ")" << endl;
    
    if (button == Qt::LeftButton) 
    {         
        addWaypoint( waypoint );
    }
    else if (button == Qt::RightButton)
    {      
        removeWaypoint( waypoint );
    }
    else if (button == Qt::MidButton)
    {
        changeWpParameters( waypoint );    
    }
    
    if (wpWidget_->isHidden()) 
        wpWidget_->show();
    wpWidget_->refreshTable();
}

void PathInput::processDoubleClickEvent( QMouseEvent* e)
{
}

void PathInput::processMoveEvent( QMouseEvent* e)
{
}

void PathInput::addWaypoint (QPointF wp)
{      
    assert(wpSettings_!=NULL);
    
    int numWaypoints = waypoints_.size();
    
    waypoints_.append( wp );
     
    if (numWaypoints==0)
    {   // first waypoint is special
        headings_.append( 0 );
        headingTolerances_.append( 180*16 );
        times_.append( 0.0 );
    }
    else 
    {
        // times: dependent on spacing mode
        float timeDelta;
        if( wpSettings_->spacingProperty=="Time" ) {
            timeDelta = wpSettings_->spacingValue;
        } else {
            timeDelta = straightLineDist( waypoints_[numWaypoints]-waypoints_[numWaypoints-1] ) / (wpSettings_->spacingValue);
        }
        times_.append( timeDelta + times_[numWaypoints-1] + waitingTimes_[numWaypoints-1] );
        
        // heading: aligned with the line between the two last waypoints
        QPointF diff = waypoints_[numWaypoints] - waypoints_[numWaypoints - 1];
        int tmpHeading =(int)floor( atan2(diff.y(),diff.x() )/M_PI*180.0 );
        if (tmpHeading < 0) tmpHeading = tmpHeading + 360;
        headings_.append( tmpHeading*16 );
        headingTolerances_.append( wpSettings_->headingTolerance*16 );
        // correct past headings to the direction we're going
        if (numWaypoints>1)
        {
            headings_[numWaypoints-1] = tmpHeading*16;
        }
    }
    
    distTolerances_.append( wpSettings_->distanceTolerance );
    maxSpeeds_.append( wpSettings_->maxApproachSpeed );
    maxTurnrates_.append( wpSettings_->maxApproachTurnrate );
    waitingTimes_.append( 0.0 );
}

void PathInput::removeWaypoint( QPointF p1 )
{
    if ( waypoints_.isEmpty() ) return;   
    const double VICINITY = 1.0; // in meters
    
    // check if click is near the latest waypoint
    int lastIndex = waypoints_.size() - 1;
    QPointF p2;
    p2 = waypoints_[lastIndex];
    double dist = sqrt ( (p1.x()-p2.x()) * (p1.x()-p2.x()) + (p1.y()-p2.y()) * (p1.y()-p2.y()) );
    if (dist<VICINITY)
    {
        // delete the last element
        resizeData( lastIndex );
    }
    
}

void PathInput::changeWpParameters( QPointF p1 )
{

    if ( waypoints_.isEmpty() ) return;    
    const double VICINITY = 1.0; // in meters
    
    // check if click is near a waypoint
    for (int i=0; i<waypoints_.size(); i++)
    {
        QPointF p2;
        p2 = waypoints_[i];
        double dist = sqrt ( (p1.x()-p2.x()) * (p1.x()-p2.x()) + (p1.y()-p2.y()) * (p1.y()-p2.y()) );
        
        if (dist<VICINITY)
        {
            QDialog *myDialog = new QDialog;
            Ui::WaypointDialog ui;
            ui.setupUi(myDialog);
            
            // save waiting time
            float waitingTimeBefore = waitingTimes_[i];
            cout << "Waiting time before: " << waitingTimeBefore << endl;
            
            ui.xSpin->setValue( waypoints_[i].x() );
            ui.ySpin->setValue( waypoints_[i].y() );
            ui.headingSpin->setValue( headings_[i]/16 );
            ui.timeSpin->setValue( times_[i] );
            ui.waitingTimeSpin->setValue( waitingTimes_[i] );
            ui.distanceTolSpin->setValue( distTolerances_[i] );
            ui.headingTolSpin->setValue( headingTolerances_[i]/16 );
            ui.maxSpeedSpin->setValue( maxSpeeds_[i] );
            ui.maxTurnrateSpin->setValue( maxTurnrates_[i] );
            
            int ret = myDialog->exec();
            if (ret==QDialog::Rejected) return;
            
            waypoints_[i].setX( ui.xSpin->value() );
            waypoints_[i].setY( ui.ySpin->value() );
            headings_[i] = ui.headingSpin->value()*16;
            times_[i] = ui.timeSpin->value();
            waitingTimes_[i] = ui.waitingTimeSpin->value();
            distTolerances_[i] = ui.distanceTolSpin->value();
            headingTolerances_[i] = ui.headingTolSpin->value()*16;
            maxSpeeds_[i] = ui.maxSpeedSpin->value();
            maxTurnrates_[i] = ui.maxTurnrateSpin->value();
            
            // if waiting time has changed we need to update all subsequent times
            if ( (waitingTimes_[i]-waitingTimeBefore)<1e-5 || (i==waypoints_.size()-1)) return;
            for (int k=i+1; k<waypoints_.size(); k++)
            {
                times_[k] = times_[k] + waitingTimes_[i];
            }
            
        }
    }

}

void PathInput::expandPath( int index, int numInsert, int headingTolerance)
{
    // insert the same values numInsert times
    waypoints_.insert( index+1, numInsert, waypoints_[index]);
    distTolerances_.insert( index+1, numInsert, distTolerances_[index]);
    maxSpeeds_.insert( index+1, numInsert, maxSpeeds_[index]);
    maxTurnrates_.insert( index+1, numInsert, maxTurnrates_[index]);
    
    // heading tolerances
    headingTolerances_[index] = headingTolerance;
    headingTolerances_.insert( index+1, numInsert, headingTolerance);
    
    // times
    float epochTime = waitingTimes_[index]/numInsert;
    for (int i=1; i<=numInsert; i++)
    {
        times_.insert( index+i, times_[index] + i*epochTime );
    }
    
    // reset waiting times
    waitingTimes_[index] = 0.0;
    waitingTimes_.insert(index+1, numInsert, 0.0);
}
    
int PathInput::expandPathStationary(int index)
{
    const int numInsert = 1;
    const int headingTolerance = headingTolerances_[index];
    expandPath( index, numInsert, headingTolerance );
    
    // heading
    headings_.insert(index+1, headings_[index]);
    
    return numInsert;
}

int PathInput::expandPathRightLeft( int index )
{
    const int numInsert=4;
    const int headingTolerance=15*16;
    expandPath( index, numInsert, headingTolerance );
    
    // headings
    // right
    int heading = headings_[index]-80*16;
    if (heading < 0) heading = heading + 360*16;
    headings_.insert(index+1, heading);
    // back to middle
    headings_.insert(index+2, headings_[index]);
    // left
    heading = headings_[index]+80*16;
    if (heading >360*16) heading = heading - 360*16;
    headings_.insert(index+3, heading);
    // back to middle
    headings_.insert(index+4, headings_[index]);
    
    return numInsert;
}

int PathInput::expandPathLeftRight( int index )
{
    const int numInsert=4;
    const int headingTolerance=15*16;
    expandPath( index, numInsert, headingTolerance );
    
    // headings
    // left
    int heading = headings_[index]+80*16;
    if (heading >360*16) heading = heading - 360*16;
    headings_.insert(index+1, heading);
    // back to middle
    headings_.insert(index+2, headings_[index]);
    // right
    heading = headings_[index]-80*16;
    if (heading < 0) heading = heading + 360*16;
    headings_.insert(index+3, heading);
    // back to middle
    headings_.insert(index+4, headings_[index]);
    
    return numInsert;
}

int PathInput::expandPathTurn360( int index )
{
    const int numInsert=4;
    const int headingTolerance=15*16;
    expandPath( index, numInsert, headingTolerance );
    
    // headings
    int heading = headings_[index];
    for (int i=1; i<numInsert; i++)
    {
        heading = heading + 90*16;
        if (heading > 360*16) heading = heading - 360*16;
        headings_.insert(index+i, heading);
    }
    headings_.insert(index+4, headings_[index]);
    
    return numInsert;
}


void PathInput::generateFullPath()
{                
    // First: find all indices where we need to insert new waypoints   
    vector<int> indices;
    for (int i=0; i<waypoints_.size(); i++)
    {
        if (waitingTimes_[i] != 0.0)
        {
            indices.push_back( i );
        }
    }
    
    // we have to keep track how many waypoints we inserted
    int totalExpansion = 0;
    
    for (unsigned int i=0; i<indices.size(); i++)
    {
        QString behaviour = wpWidget_->getBehaviour( indices[i] ); 
        cout << "TRACE(pathinput.cpp): generatePath: Behaviour: " << behaviour.toStdString() << endl;
        
        int numExpand = 0;
        
        if (behaviour=="Stationary") {
            numExpand = expandPathStationary(indices[i]+totalExpansion);
        } 
        else if (behaviour=="LeftRight") {
            numExpand = expandPathLeftRight(indices[i]+totalExpansion);
        }
        else if (behaviour=="RightLeft") {
            numExpand = expandPathRightLeft(indices[i]+totalExpansion);
        }
        else if (behaviour=="Turn360") {
            numExpand = expandPathTurn360(indices[i]+totalExpansion);
        }
        else {
            cout << "Error(pathinput.cpp): Unknown behaviour: " << behaviour.toStdString() << endl;
            break;
        }
        
        totalExpansion = totalExpansion + numExpand;
    }
    
    if (indices.size()!=0) 
    {
        // This will flatten the table
        wpWidget_->refreshTable();
    } 
    else 
    {
        humanManager_->showStatusMsg(Warning, "No waiting times, no list expansion");
    }
}

void 
PathInput::savePath( const QString &fileName )
{
    int size=wpWidget_->numberOfLoops() * waypoints_.size();
    
    if (size==0)
    {
        humanManager_->showBoxMsg(Warning, "Path has no waypoints!");
        return;
    }
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        humanManager_->showBoxMsg(Error, "Cannot create file " + fileName );
        return;
    }
    
    lastSavedPathFile_ = fileName;
    
    // offset time if we have several loops
    const float timeOffset = times_.last() + secondsToCompleteLoop();

    QTextStream out(&file);
    for (int k=0; k<wpWidget_->numberOfLoops(); k++)
    {
        for (int i=0; i<waypoints_.size(); i++)
        {
            out << waypoints_[i].x() << " " << waypoints_[i].y() << " "
                    << headings_[i] << " "
                    << times_[i]+k*timeOffset << " "
                    << distTolerances_[i] << " "
                    << headingTolerances_[i]<< " "
                    << maxSpeeds_[i]<< " "
                    << maxTurnrates_[i]<< "\n";
        }
    }
    
    file.close();
    humanManager_->showStatusMsg(Information, "Path successfully saved to " + fileName );
}

float
PathInput::secondsToCompleteLoop() const
{
    // time from last waypoint to first waypoint to complete the loop
    float timeToCompleteLoop;
    if( wpSettings_->spacingProperty=="Time" ) {
        timeToCompleteLoop = wpSettings_->spacingValue;
    } else {
        timeToCompleteLoop = straightLineDist( waypoints_.last()-waypoints_.first() ) / (wpSettings_->spacingValue);
    }
    return timeToCompleteLoop;
}
    
void PathInput::loadPreviousPath()
{
    if (lastSavedPathFile_!="") {
        loadPath( lastSavedPathFile_ );    
    } else {
        humanManager_->showStatusMsg(Warning, "No path saved yet!" ); 
    }
    
}

void PathInput::loadPath( const QString& fileName )
{    
    resizeData(0);
    
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        humanManager_->showStatusMsg(Error, "Problem opening file " + fileName );
        return;
    }
    
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        waypoints_.append( QPointF(line.section(' ',0,0).toFloat(),line.section(' ',1,1).toFloat()) );
        headings_.append( line.section(' ',2,2).toInt() );
        times_.append( line.section(' ',3,3).toFloat() );
        distTolerances_.append( line.section(' ',4,4).toFloat() );
        headingTolerances_.append( line.section(' ',5,5).toInt() );
        maxSpeeds_.append( line.section(' ',6,6).toFloat() );
        maxTurnrates_.append( line.section(' ',7,7).toInt() );
        waitingTimes_.append( 0.0 );
    }
    humanManager_->showStatusMsg(Information, "Successfully loaded file " + fileName );
    wpWidget_->refreshTable();
}

bool
PathFollowerInput::getPath( orca::PathFollower2dData &pathData ) const
{    
    int size = wpWidget_->numberOfLoops() * waypoints_.size();
    cout << "DEBUG(pathinput.cpp): getPath: size of waypoints is " << size << endl;
    if (size==0) return false;
    
    pathData.path.resize( size );
    int counter = -1;
    
    // offset time if we have several loops
    const float timeOffset = times_.last() + secondsToCompleteLoop();
    
    for (int k=0; k<wpWidget_->numberOfLoops(); k++)
    {
        for (int i=0; i<waypoints_.size(); i++)
        {
            counter++;
            
            float heading = headings_[i]/16;
            if (heading>180.0) {
                heading = heading - 360.0;
            }
            float headingTolerance = headingTolerances_[i]/16;
    
            pathData.path[counter].target.p.x = waypoints_[i].x();
            pathData.path[counter].target.p.y = waypoints_[i].y();
            pathData.path[counter].target.o = heading/180.0 * M_PI;
            pathData.path[counter].distanceTolerance = distTolerances_[i];
            pathData.path[counter].headingTolerance = (float)headingTolerance/180.0*M_PI;      
            pathData.path[counter].timeTarget = orcaice::toOrcaTime( times_[i] + k*timeOffset );
                
            pathData.path[counter].maxApproachSpeed = maxSpeeds_[i];
            pathData.path[counter].maxApproachTurnrate = (float)maxTurnrates_[i]/180.0*M_PI;
        }
    }
    return true;
}

orca::PathPlanner2dTask
PathPlannerInput::getTask() const
{
    int size = waypoints_.size();
        
    orca::PathPlanner2dTask task;
    task.coarsePath.resize( size );
        
    for (int i=0; i<size; i++)
    {
        
        float heading = headings_[i]/16;
        if (heading>180.0) {
            heading = heading - 360.0;
        }
        float headingTolerance = headingTolerances_[i]/16;
//  AlexB: why normalise tolerance?
//         if (headingTolerance>180.0) {
//             headingTolerance = headingTolerance - 360.0;
//         }
        orca::Waypoint2d wp;
        wp.target.p.x = waypoints_[i].x();
        wp.target.p.y = waypoints_[i].y();
        wp.target.o = heading/180.0 * M_PI;
        wp.distanceTolerance = distTolerances_[i];
        wp.headingTolerance = (float)headingTolerance/180.0*M_PI;      
        wp.timeTarget = orcaice::toOrcaTime( times_[i] );
        wp.maxApproachSpeed = maxSpeeds_[i];
        wp.maxApproachTurnrate = (float)maxTurnrates_[i]/180.0*M_PI;
            
        task.coarsePath[i] = wp;
    }
    return task;
}


//////////////////////////////////////////////////////////////////////

WaypointSettings
readWaypointSettings( const Ice::PropertiesPtr & props, const std::string & tag )
{
    std::string prefix = tag + ".Config.Waypoints.";

    Ice::StringSeq strIn; strIn.push_back("Velocity"); Ice::StringSeq strOut;
    strOut = orcaice::getPropertyAsStringSeqWithDefault( props, prefix+"SpacingProperty", strIn );
    std::string spacingProperty = strOut[0];
    float spacingValue = orcaice::getPropertyAsDoubleWithDefault( props, prefix+"SpacingValue", 1.0 );
    float distanceTolerance = orcaice::getPropertyAsDoubleWithDefault( props, prefix+"DistanceTolerance", 1.0 );
    int headingTolerance = orcaice::getPropertyAsIntWithDefault( props, prefix+"HeadingTolerance", 90 );
    float maxApproachSpeed = orcaice::getPropertyAsDoubleWithDefault( props, prefix+"MaxApproachSpeed", 2e6 );
    int maxApproachTurnrate = orcaice::getPropertyAsIntWithDefault( props, prefix+"MaxApproachTurnRate", 6000000 );
    WaypointSettings wpSettings(spacingProperty, spacingValue, distanceTolerance, headingTolerance, maxApproachSpeed, maxApproachTurnrate);
    return wpSettings;
}

bool 
readActivateImmediately( const Ice::PropertiesPtr & props, const std::string & tag )
{
    std::string prefix = tag + ".Config.PathFollower2d.";
    return orcaice::getPropertyAsIntWithDefault( props, prefix+"ActivatePathImmediately", 1 );
}

QString readDumpPath( const Ice::PropertiesPtr & props, const std::string & tag )
{
    Ice::StringSeq strIn; strIn.push_back("/tmp"); Ice::StringSeq strOut;
    strOut = orcaice::getPropertyAsStringSeqWithDefault( props, tag+".Config.General.DumpPath", strIn );
    return QString(strOut[0].c_str());
}


}


