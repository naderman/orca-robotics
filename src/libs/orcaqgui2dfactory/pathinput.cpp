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
#include <orcaqgui/ihumanmanager.h>
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

namespace orcaqgui {
    
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
                    QVector<int> *maxTurnrates,
                    QVector<QString> *behaviours)
    : pathInput_(pathInput),
      pathFileSet_(false),
      pathFileName_("/tmp")
{
    setWindowTitle("List of Waypoints");
    wpTable_ = new WpTable( this, pathInput, waypoints, headings, times, waitingTimes, distTolerances, headingTolerances, maxSpeeds, maxTurnrates, behaviours );
    QPushButton *generatePath = new QPushButton(tr("Generate Full Path"), this);
    QPushButton *savePath = new QPushButton(tr("Save Path"), this);
    QPushButton *loadPath = new QPushButton(tr("Load Path"), this);
    
    QObject::connect(generatePath,SIGNAL(clicked()),pathInput,SLOT(generateFullPath()));
    QObject::connect(savePath,SIGNAL(clicked()),this,SLOT(savePath()));
    QObject::connect(loadPath,SIGNAL(clicked()),this,SLOT(loadPath()));
    
    QVBoxLayout *globalLayout = new QVBoxLayout;
    globalLayout->addWidget(wpTable_);
    
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(generatePath);
    hLayout->addWidget(savePath);
    hLayout->addWidget(loadPath);
    globalLayout->addLayout(hLayout);
    
    setLayout(globalLayout);
    this->show();
}

void WpWidget::refreshTable()
{
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
        pathInput_->loadPath( &fileName );
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
                 QVector<int> *maxTurnrates,
                 QVector<QString> *behaviours )
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
      behaviours_(behaviours),
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

void WpTable::test(int i)
{
    
    cout << "Activated: " << i << endl;
}

void WpTable::refreshTable()
{
    isLocked_ = true;
    
    int size = waypoints_->size();
    clearContents();
    setRowCount(size);
    
    QString str;
    QTableWidgetItem *item;
    
    computeVelocities();
    
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
        
        QComboBox *combo = new QComboBox(this);
        QStringList strList;
        strList << "Stationary" << "LeftRight" << "RightLeft" << "Turn360";
        combo->addItems(strList);
        if (waitingTimes_->at(row)==0.0) { 
            combo->setEnabled(false);
        } else {
            int ind = combo->findText( behaviours_->at(row) );
            combo->setCurrentIndex( ind );
        }
        setCellWidget(row, Behaviour, combo);
        
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

QString WpTable::getBehaviour( int row )
{
    QComboBox *combo = dynamic_cast<QComboBox*>(cellWidget(row,Behaviour));
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
            float *data = times_->data();
            data[row] = item->text().toDouble();
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
            data[row] = deltaS/velocity + (data[row-1] + waitingTimes_->at(row-1));
            refreshTable();
            break;
        }
        case Behaviour:
        {
            // This one is never called since the combo boxes only
            QString *data = behaviours_->data();
            QComboBox *combo = dynamic_cast<QComboBox*>(cellWidget(row,Behaviour));
            data[row] = combo->currentText();
            cout << "Behaviour, currentText: " << combo->currentText().toStdString() << endl;    
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
    
PathInput::PathInput( WaypointSettings *wpSettings, IHumanManager *humanManager )
    : wpSettings_(wpSettings),
      humanManager_(humanManager),      
      waypointInFocus_(-99)
{   
    wpWidget_ = new WpWidget( this,
                            &waypoints_,
                            &headings_, 
                            &times_,
                            &waitingTimes_,
                            &distTolerances_,
                            &headingTolerances_,
                            &maxSpeeds_,
                            &maxTurnrates_,
                            &behaviours_ );
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
    behaviours_.resize( index );
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
    behaviours_.append("Stationary");
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
            cout << "Waiting time after: " << waitingTimes_[i] << endl;
            cout << "Difference is: " << (waitingTimes_[i]-waitingTimeBefore) << endl;
            if ( (waitingTimes_[i]-waitingTimeBefore)<1e-5 || (i==waypoints_.size()-1)) return;
            cout << "Didn't return" << endl;
            for (int k=i+1; k<waypoints_.size(); k++)
            {
                times_[k] = times_[k] + waitingTimes_[i];
            }
            
        }
    }

}

void PathInput::expandPath( int index, int numInsert)
{
    // insert the same values numInsert times
    waypoints_.insert( index+1, numInsert, waypoints_[index]);
    distTolerances_.insert( index+1, numInsert, distTolerances_[index]);
    headingTolerances_.insert( index+1, numInsert, headingTolerances_[index]);
    maxSpeeds_.insert( index+1, numInsert, maxSpeeds_[index]);
    maxTurnrates_.insert( index+1, numInsert, maxTurnrates_[index]);
    
    // times
    float epochTime = waitingTimes_[index]/numInsert;
    for (int i=1; i<=numInsert; i++)
    {
        times_.insert( index+i, times_[index] + i*epochTime );
    }
    
    // reset
    waitingTimes_[index] = 0.0;
    waitingTimes_.insert(index+1, numInsert, 0.0);
    behaviours_[index] = "Stationary";
    behaviours_.insert(index+1, numInsert, "Stationary");
}
    
void PathInput::expandPathStationary(int index)
{
    const int numInsert=1;
    expandPath( index, numInsert );
    
    // heading
    headings_.insert(index+1, headings_[index]);
    
//     waypoints_.insert(index+1, waypoints_[index]);
//     headings_.insert(index+1, headings_[index]);
//     times_.insert(index+1, times_[index]+waitingTimes_[index]);
//     distTolerances_.insert(index+1, distTolerances_[index]);
//     headingTolerances_.insert(index+1, headingTolerances_[index]);
//     maxSpeeds_.insert(index+1, maxSpeeds_[index]);
//     maxTurnrates_.insert(index+1, maxTurnrates_[index]);
//     
//     // reset
//     waitingTimes_[index] = 0.0;
//     waitingTimes_.insert(index+1, 0.0);
//     behaviours_[index] = "Stationary";
//     behaviours_.insert(index+1,"Stationary");
}

void PathInput::expandPathLeftRight( int index )
{
    const int numInsert=3;
    expandPath( index, numInsert );
    
    // headings
    int heading = headings_[index]-90*16;
    if (heading < 0) heading = heading + 360*16;
    headings_.insert(index+1, heading);
    heading = headings_[index]+90*16;
    if (heading >360*16) heading = heading - 360*16;
    headings_.insert(index+2, heading);
    headings_.insert(index+3, headings_[index]);
}

void PathInput::expandPathRightLeft( int index )
{
    const int numInsert=3;
    expandPath( index, numInsert );
    
    // headings
    int heading = headings_[index]+90*16;
    if (heading >360*16) heading = heading - 360*16;
    headings_.insert(index+1, heading);
    heading = headings_[index]-90*16;
    if (heading < 0) heading = heading + 360*16;
    headings_.insert(index+2, heading);
    headings_.insert(index+3, headings_[index]);
}

void PathInput::expandPathTurn360( int index )
{
    const int numInsert=4;
    expandPath( index, numInsert );
    
    // headings
    int heading = headings_[index];
    for (int i=1; i<numInsert; i++)
    {
        heading = heading + 90*16;
        if (heading > 360*16) heading = heading - 360*16;
        headings_.insert(index+i, heading);
    }
    headings_.insert(index+4, headings_[index]);
}


void PathInput::generateFullPath()
{    
    bool hasListExpanded = false;
            
    // using the waiting time, we produce additional waypoints
    // basically we're 'flattening' the list
    for (int i=0; i<waypoints_.size(); i++)
    {
        if (waitingTimes_[i] != 0.0)
        {
            hasListExpanded = true;
            
            QString behaviour = wpWidget_->getBehaviour( i );
            
            cout << "generatePath: Behaviour: " << behaviour.toStdString() << endl;
            
            if (behaviour=="Stationary") {
                expandPathStationary(i);
            } 
            else if (behaviour=="LeftRight") {
                expandPathLeftRight(i);
            }
            else if (behaviour=="RightLeft") {
                expandPathRightLeft(i);
            }
            else if (behaviour=="Turn360") {
                expandPathTurn360(i);
            }
            else {
                cout << "Error(pathinput.cpp): Unknown behaviour: " << behaviour.toStdString() << endl;
                break;
            }
        }
    }
    if (hasListExpanded) {
        wpWidget_->refreshTable();
    } else {
        humanManager_->showStatusMsg(Warning, "No waiting times, no list expansion");
    }
}

void 
PathInput::savePath( const QString &fileName ) const
{
    int size=wpSettings_->numberOfLoops * waypoints_.size();
    
    if (size==0)
    {
        humanManager_->showBoxMsg(Warning, "Path has no waypoints. File will be empty!");
    }
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        humanManager_->showBoxMsg(Error, "Cannot create file " + fileName );
        return;
    }
    
    // for loops we need to know the timestamp of the last waypoint
    const float timeOffset = times_[waypoints_.size()-1];

    QTextStream out(&file);
    for (int k=0; k<wpSettings_->numberOfLoops; k++)
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

void PathInput::loadPath( QString* fileName )
{    
    resizeData(0);
    
    QFile file(*fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        humanManager_->showStatusMsg(Error, "Problem opening file " + *fileName );
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
        behaviours_.append("");
    }
    humanManager_->showStatusMsg(Information, "Successfully loaded file " + *fileName );
    wpWidget_->refreshTable();
}

orca::PathFollower2dData
PathFollowerInput::getPath() const
{
    int size = wpSettings_->numberOfLoops * waypoints_.size();
    cout << "DEBUG(pathinput.cpp): getPath: size of waypoints is " << size << endl;
    
    orca::PathFollower2dData pathData;
    pathData.path.resize( size );
    int counter = -1;
    
    const float timeOffset = times_[waypoints_.size()-1];
    
    for (int k=0; k<wpSettings_->numberOfLoops; k++)
    {
        for (int i=0; i<waypoints_.size(); i++)
        {
            counter++;
            
            float heading = headings_[i]/16;
            if (heading>180.0) {
                heading = heading - 360.0;
            }
            float headingTolerance = headingTolerances_[i]/16;
            if (headingTolerance>180.0) {
                headingTolerance = headingTolerance - 360.0;
            }
    
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

    return pathData;
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
        if (headingTolerance>180.0) {
            headingTolerance = headingTolerance - 360.0;
        }
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
    int numberOfLoops = orcaice::getPropertyAsIntWithDefault( props, prefix+"NumberOfLoops", 1 );
    WaypointSettings wpSettings(spacingProperty, spacingValue, distanceTolerance, headingTolerance, maxApproachSpeed, maxApproachTurnrate, numberOfLoops);
    return wpSettings;
}

bool 
readActivateImmediately( const Ice::PropertiesPtr & props, const std::string & tag )
{
    std::string prefix = tag + ".Config.PathFollower2d.";
    return orcaice::getPropertyAsIntWithDefault( props, prefix+"ActivatePathImmediately", 1 );
}


}


