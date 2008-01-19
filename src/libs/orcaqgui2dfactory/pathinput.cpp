/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <cmath>
 
#include <orcaice/orcaice.h>
#include <orcalogfactory/logstringutils.h>
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
#include <QShortcut>

#include "pathinput.h"

using namespace std;

namespace orcaqgui2d {
    
const int QT_ANGLE_MULTIPLIER=16;
    
// normalises heading to 0<angle<360*16 for Gui painting
int guiNormalise( int heading )
{
    if (heading < 0) return (heading + 360*QT_ANGLE_MULTIPLIER);
    if (heading > 360*QT_ANGLE_MULTIPLIER) return (heading - 360*QT_ANGLE_MULTIPLIER);
    
    return heading;
}
    
void guiPathToOrcaPath( const GuiPath &in, orca::Path2d &out, int numLoops, float timeOffset )
{
    out.resize( in.size()*numLoops );
    
    int counter = 0;
    
    for (int k=0; k<numLoops; k++)
    {
        for (int i=0; i<in.size(); i++)
        {
            double heading = DEG2RAD((double)in[i].heading/QT_ANGLE_MULTIPLIER);
            NORMALISE_ANGLE( heading );
    
            out[counter].target.p.x = in[i].position.x();
            out[counter].target.p.y = in[i].position.y();
            out[counter].target.o = heading;
            out[counter].distanceTolerance = in[i].distanceTolerance;
            out[counter].headingTolerance = DEG2RAD((float)in[i].headingTolerance/QT_ANGLE_MULTIPLIER);      
            out[counter].timeTarget = orcaice::toOrcaTime( in[i].timeTarget + k*timeOffset );
            out[counter].maxApproachSpeed = in[i].maxSpeed;
            out[counter].maxApproachTurnrate = DEG2RAD((float)in[i].maxTurnrate);
            
            counter++;
        }
    }
}

void orcaPathToGuiPath( const orca::Path2d &in, GuiPath &out )
{
    out.resize( in.size() );
    
    for (unsigned int i=0; i<in.size(); i++)
    {
        out[i].position.setX( in[i].target.p.x );
        out[i].position.setY( in[i].target.p.y );
        out[i].heading = (int)floor(RAD2DEG(in[i].target.o))*QT_ANGLE_MULTIPLIER;
        out[i].distanceTolerance = in[i].distanceTolerance;
        out[i].headingTolerance = (int)floor(RAD2DEG(in[i].headingTolerance))*QT_ANGLE_MULTIPLIER;   
        out[i].timeTarget = orcaice::timeAsDouble(in[i].timeTarget);
        out[i].maxSpeed = in[i].maxApproachSpeed;
        out[i].maxTurnrate = (int)floor(RAD2DEG(in[i].maxApproachTurnrate));
    }
}
    
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
                    GuiPath *guiPath,
                    QVector<float> *waitingTimes )
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
    wpTable_ = new WpTable( this, pathInput, guiPath, waitingTimes );
    
    QPushButton *generatePath = new QPushButton(tr("Generate Full Path"), this);
    QPushButton *savePath = new QPushButton(savePathIcon, tr("Save Path"), this);
    QPushButton *loadPath = new QPushButton(openIcon, tr("Load Path"), this);
    QPushButton *loadPreviousPath = new QPushButton(openIcon, tr("Load Previous Path"), this);
    QObject::connect(generatePath,SIGNAL(clicked()),pathInput,SLOT(generateFullPath()));
    QObject::connect(savePath,SIGNAL(clicked()),this,SLOT(savePath()));
    QObject::connect(loadPath,SIGNAL(clicked()),this,SLOT(loadPath()));
    QObject::connect(loadPreviousPath,SIGNAL(clicked()),pathInput,SLOT(loadPreviousPath()));
    
    QPushButton *sendPath = new QPushButton(sendIcon, tr("Send Path"), this);
    QShortcut *sendShortcut = new QShortcut(QKeySequence(tr("F11")), this );
    QPushButton *cancelPath = new QPushButton(cancelIcon, tr("Cancel Path"), this);
    QObject::connect(sendShortcut,SIGNAL(activated()),pathInput,SIGNAL(sendPathClicked()));
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
                 GuiPath *guiPath,
                 QVector<float> *waitingTimes
                )
    : QTableWidget( parent ),
      pathInput_(pathInput),
      guiPath_(guiPath),
      waitingTimes_(waitingTimes),
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
    int size = guiPath_->size();
    setRowCount(size);
    
    QString str;
    QTableWidgetItem *item;
    
    for (int row=0; row<size; row++)
    {
        str.setNum(guiPath_->at(row).timeTarget,'g',4);
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
        
        str.setNum(guiPath_->at(row).position.x(),'g',4);
        item = new QTableWidgetItem(str);
        setItem(row, WaypointX, item);
        
        str.setNum(guiPath_->at(row).position.y(),'g',4);
        item = new QTableWidgetItem(str);
        setItem(row, WaypointY, item);
        
        str.setNum(guiPath_->at(row).heading/QT_ANGLE_MULTIPLIER);
        item = new QTableWidgetItem(str);
        setItem(row, Heading, item);
        
        str.setNum(guiPath_->at(row).distanceTolerance,'g',4);
        item = new QTableWidgetItem(str);
        setItem(row, DistanceTolerance, item);
        
        str.setNum(guiPath_->at(row).headingTolerance/QT_ANGLE_MULTIPLIER);
        item = new QTableWidgetItem(str);
        setItem(row, HeadingTolerance, item);
        
        str.setNum(guiPath_->at(row).maxSpeed,'g',4);
        item = new QTableWidgetItem(str);
        setItem(row, MaxApproachSpeed, item);
        
        str.setNum(guiPath_->at(row).maxTurnrate);
        item = new QTableWidgetItem(str);
        setItem(row, MaxApproachTurnRate, item);
    }
    
    isLocked_ = false;
}

void WpTable::computeVelocities()
{
    velocities_.resize(1);
    velocities_[0] = 0.0;
    for (int i=1; i<guiPath_->size(); i++)
    {
        float deltaS = straightLineDist( guiPath_->at(i).position - guiPath_->at(i-1).position);
        float deltaT = guiPath_->at(i).timeTarget - (guiPath_->at(i-1).timeTarget + waitingTimes_->at(i-1));
        velocities_.push_back(deltaS/deltaT);
    }
}

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
    
    // get a pointer to first element
    GuiWaypoint *data = guiPath_->data();
    
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
        case WaitingTime: 
        {       
            float waitingTime =  item->text().toDouble();
            if (waitingTime>0.0) {
                cellWidget(row,Behaviour)->setEnabled(true);
            } else {
                cellWidget(row,Behaviour)->setEnabled(false);
            }
            float *waitingData = waitingTimes_->data();
            waitingData[row] = waitingTime;
            // update all subsequent times
            for (int i=row+1; i<rowCount(); i++)
            {
                data[i].timeTarget = data[i].timeTarget + waitingTime;    
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
            float deltaS = straightLineDist( guiPath_->at(row).position - guiPath_->at(row-1).position );
            float oldTime = data[row].timeTarget;
            float newTime = deltaS/velocity + (data[row-1].timeTarget + waitingTimes_->at(row-1));
            // set new absolute time in our row
            data[row].timeTarget = newTime;
            //all subsequent times will be updated too
            for (int i=row+1; i<rowCount(); i++)
            {
                data[i].timeTarget = data[i].timeTarget + (newTime-oldTime);    
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
            cout << "WpTable:: This column is not supported!" << endl;
        }
    }
        
    
}
    
PathInput::PathInput( QObject                  *parent,
                      WaypointSettings         *wpSettings,
                      hydroqgui::IHumanManager &humanManager,
                      QString                   lastSavedPathFile )
    : wpSettings_(wpSettings),
      humanManager_(humanManager),      
      waypointInFocus_(-1),
      lastSavedPathFile_(lastSavedPathFile)
{   
    wpWidget_ = new WpWidget( this,
                              &guiPath_,
                              &waitingTimes_);
    
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
    guiPath_.resize( index );
    waitingTimes_.resize( index );
}


void PathInput::paint( QPainter *p )
{
    QMatrix wm = p->matrix(); 
    wmInv_ = wm.inverted();
    
    if ( guiPath_.isEmpty() ) return;
    
    const float PATH_WIDTH = 0.05;
    
    QColor fillColor;
    QColor drawColor;
    
    if (useTransparency_) {
        fillColor = getTransparentVersion(Qt::green);
    } else {
        fillColor=Qt::green;
    }
    
    for ( int i=0; i<guiPath_.size(); i++)
    {
        p->save();
        p->translate( guiPath_[i].position.x(), guiPath_[i].position.y() );    // move to point

        if (i==waypointInFocus_) {
            drawColor = Qt::black;
        } else {
            drawColor = fillColor;
        }
        
        paintWaypoint( p, 
                       fillColor,
                       drawColor, 
                       guiPath_[i].heading,
                       guiPath_[i].distanceTolerance,
                       guiPath_[i].headingTolerance );

        p->restore();
    }
    
    // ===== draw the waypoint in focus again, to be able to see the edge =======
    if ( (waypointInFocus_!=-1) && (guiPath_.size()>0) )
    {
        p->save();

        p->translate( guiPath_[waypointInFocus_].position.x(), guiPath_[waypointInFocus_].position.y() );    // move to point
        drawColor = Qt::black;
        paintWaypoint( p, 
                        fillColor,
                        drawColor, 
                        guiPath_[waypointInFocus_].heading,
                        guiPath_[waypointInFocus_].distanceTolerance,
                        guiPath_[waypointInFocus_].headingTolerance );
        p->restore();
    }
    
    // draw connections between them
    if ( guiPath_.size()>1 )
    {
        p->setPen( QPen( fillColor, PATH_WIDTH ) );
        p->setBrush ( Qt::NoBrush );

        for ( int i=1; i<guiPath_.size(); ++i)
        {
            p->drawLine(guiPath_[i].position, guiPath_[i-1].position);
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
    
    int numWaypoints = guiPath_.size();
    
    GuiWaypoint gwp;
    gwp.position = wp;
    guiPath_.push_back( gwp );
     
    if (numWaypoints==0)
    {   // first waypoint is special
        guiPath_[0].heading = 0;
        guiPath_[0].headingTolerance = 180*QT_ANGLE_MULTIPLIER;
        guiPath_[0].timeTarget = 0.0;
    }
    else 
    {
        // times: dependent on spacing mode
        float timeDelta;
        if( wpSettings_->spacingProperty=="Time" ) {
            timeDelta = wpSettings_->spacingValue;
        } else {
            timeDelta = straightLineDist( guiPath_[numWaypoints].position-guiPath_[numWaypoints-1].position ) / (wpSettings_->spacingValue);
        }
        
        
        guiPath_[numWaypoints].timeTarget = timeDelta + guiPath_[numWaypoints-1].timeTarget + waitingTimes_[numWaypoints-1];
        
        // heading: aligned with the line between the two last waypoints
        QPointF diff = guiPath_[numWaypoints].position - guiPath_[numWaypoints-1].position;
        int tmpHeading =(int)floor( RAD2DEG(atan2(diff.y(),diff.x())) ) * QT_ANGLE_MULTIPLIER;
        guiPath_[numWaypoints].heading = guiNormalise(tmpHeading);
        
        guiPath_[numWaypoints].headingTolerance = wpSettings_->headingTolerance*QT_ANGLE_MULTIPLIER;
        // correct previous headings to the direction we're going
        if (numWaypoints>1)
        {
            guiPath_[numWaypoints-1].heading = tmpHeading;
        }
    }
    
    guiPath_[numWaypoints].distanceTolerance = wpSettings_->distanceTolerance;
    guiPath_[numWaypoints].maxSpeed = wpSettings_->maxApproachSpeed;
    guiPath_[numWaypoints].maxTurnrate = wpSettings_->maxApproachTurnrate;
    waitingTimes_.append( 0.0 );
}

void PathInput::removeWaypoint( QPointF p1 )
{
    if (guiPath_.isEmpty() ) return;
    const double VICINITY = 1.0; // in meters
    
    // check if click is near the latest waypoint
    int lastIndex = guiPath_.size() - 1;
    QPointF p2;
    p2 = guiPath_[lastIndex].position;
    double dist = sqrt ( (p1.x()-p2.x()) * (p1.x()-p2.x()) + (p1.y()-p2.y()) * (p1.y()-p2.y()) );
    if (dist<VICINITY)
    {
        // delete the last element
        resizeData( lastIndex );
    }
    
}

void PathInput::changeWpParameters( QPointF p1 )
{

    if ( guiPath_.isEmpty() ) return;    
    const double VICINITY = 1.0; // in meters
    
    // check if click is near a waypoint
    for (int i=0; i<guiPath_.size(); i++)
    {
        QPointF p2;
        p2 = guiPath_[i].position;
        double dist = sqrt ( (p1.x()-p2.x()) * (p1.x()-p2.x()) + (p1.y()-p2.y()) * (p1.y()-p2.y()) );
        
        if (dist<VICINITY)
        {
            QDialog *myDialog = new QDialog;
            Ui::WaypointDialog ui;
            ui.setupUi(myDialog);
            
            // save waiting time
            float waitingTimeBefore = waitingTimes_[i];
            //cout << "Waiting time before: " << waitingTimeBefore << endl;
            
            ui.xSpin->setValue( guiPath_[i].position.x() );
            ui.ySpin->setValue( guiPath_[i].position.y() );
            ui.headingSpin->setValue( guiPath_[i].heading/QT_ANGLE_MULTIPLIER );
            ui.timeSpin->setValue( guiPath_[i].timeTarget );
            ui.waitingTimeSpin->setValue( waitingTimes_[i] );
            ui.distanceTolSpin->setValue( guiPath_[i].distanceTolerance );
            ui.headingTolSpin->setValue( guiPath_[i].headingTolerance/QT_ANGLE_MULTIPLIER );
            ui.maxSpeedSpin->setValue( guiPath_[i].maxSpeed );
            ui.maxTurnrateSpin->setValue( guiPath_[i].maxTurnrate );
            
            int ret = myDialog->exec();
            if (ret==QDialog::Rejected) return;
            
            guiPath_[i].position.setX( ui.xSpin->value() );
            guiPath_[i].position.setY( ui.ySpin->value() );
            guiPath_[i].heading = ui.headingSpin->value()*QT_ANGLE_MULTIPLIER;
            guiPath_[i].timeTarget = ui.timeSpin->value();
            waitingTimes_[i] = ui.waitingTimeSpin->value();
            guiPath_[i].distanceTolerance = ui.distanceTolSpin->value();
            guiPath_[i].headingTolerance = ui.headingTolSpin->value()*QT_ANGLE_MULTIPLIER;
            guiPath_[i].maxSpeed = ui.maxSpeedSpin->value();
            guiPath_[i].maxTurnrate = ui.maxTurnrateSpin->value();
            
            // if waiting time has changed we need to update all subsequent times
            if ( (waitingTimes_[i]-waitingTimeBefore)<1e-5 || (i==guiPath_.size()-1)) return;
            for (int k=i+1; k<guiPath_.size(); k++)
            {
                guiPath_[k].timeTarget = guiPath_[k].timeTarget + waitingTimes_[i];
            }
            
        }
    }

}

void PathInput::expandPath( int index, int numInsert, int headingTolerance)
{
    // insert the same values numInsert times
    GuiWaypoint gwp;
    gwp.position = guiPath_[index].position;
    gwp.distanceTolerance = guiPath_[index].distanceTolerance;
    gwp.headingTolerance = headingTolerance;
    gwp.maxSpeed = guiPath_[index].maxSpeed;
    gwp.maxTurnrate = guiPath_[index].maxTurnrate;
    
    // times
    float epochTime = waitingTimes_[index]/numInsert;
    float time = 0.0;
    for (int i=1; i<=numInsert; i++)
    {
        time = guiPath_[index].timeTarget + i*epochTime;
    }
    gwp.timeTarget = time;
    guiPath_.insert( index+1, numInsert, gwp );
    
    // reset waiting times
    waitingTimes_[index] = 0.0;
    waitingTimes_.insert(index+1, numInsert, 0.0);
}
    
int PathInput::expandPathStationary(int index)
{
    const int numInsert = 1;
    const int headingTolerance = guiPath_[index].headingTolerance;
    expandPath( index, numInsert, headingTolerance );
    
    // heading
    guiPath_[index+1].heading = guiPath_[index].heading;
    
    return numInsert;
}

int PathInput::expandPathRightLeft( int index )
{
    const int numInsert=4;
    const int headingTolerance=15*QT_ANGLE_MULTIPLIER;
    expandPath( index, numInsert, headingTolerance );
    
    // headings
    // right
    guiPath_[index+1].heading = guiNormalise(guiPath_[index].heading-80*QT_ANGLE_MULTIPLIER);
    // back to middle
    guiPath_[index+2].heading = guiPath_[index].heading;
    // left
    guiPath_[index+3].heading =  guiNormalise(guiPath_[index].heading+80*QT_ANGLE_MULTIPLIER);
    // back to middle
    guiPath_[index+4].heading = guiPath_[index].heading;
    
    return numInsert;
}

int PathInput::expandPathLeftRight( int index )
{
    const int numInsert=4;
    const int headingTolerance=15*QT_ANGLE_MULTIPLIER;
    expandPath( index, numInsert, headingTolerance );
    
    // headings
    // left
    guiPath_[index+1].heading = guiNormalise(guiPath_[index].heading+80*QT_ANGLE_MULTIPLIER);
    // back to middle
    guiPath_[index+2].heading = guiPath_[index].heading;
    // right
    guiPath_[index+3].heading = guiNormalise(guiPath_[index].heading-80*QT_ANGLE_MULTIPLIER);
    // back to middle
    guiPath_[index+4].heading = guiPath_[index].heading;
    
    return numInsert;
}

int PathInput::expandPathTurn360( int index )
{
    const int numInsert=4;
    const int headingTolerance=15*QT_ANGLE_MULTIPLIER;
    expandPath( index, numInsert, headingTolerance );
    
    // headings
    int heading = guiPath_[index].heading;
    for (int i=1; i<numInsert; i++)
    {
        guiPath_[index+i].heading = guiNormalise(heading + 90*QT_ANGLE_MULTIPLIER);
    }
    guiPath_[index+4].heading = guiPath_[index].heading;
    
    return numInsert;
}


void PathInput::generateFullPath()
{                
    // First: find all indices where we need to insert new waypoints   
    vector<int> indices;
    for (int i=0; i<guiPath_.size(); i++)
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
        humanManager_.showStatusMsg(hydroqgui::IHumanManager::Warning, "No waiting times, no list expansion");
    }
}

void 
PathInput::savePath( const QString &fileName )
{
    int size=wpWidget_->numberOfLoops() * guiPath_.size();
    
    if (size==0)
    {
        humanManager_.showDialogMsg(hydroqgui::IHumanManager::Warning, "Path has no waypoints!");
        return;
    }
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        humanManager_.showDialogMsg(hydroqgui::IHumanManager::Error,
                                  "Cannot create file " + fileName );
        return;
    }
    
    lastSavedPathFile_ = fileName;
    
    // offset time if we have several loops
    const float timeOffset = guiPath_.last().timeTarget + secondsToCompleteLoop();

    // convert gui path to an orca path
    orca::Path2d orcaPath;
    guiPathToOrcaPath( guiPath_, orcaPath, wpWidget_->numberOfLoops(), timeOffset );
    
    // write to a file
    QTextStream out(&file);
    out << QString(orcalogfactory::toLogString( orcaPath ).c_str());
    file.close();
    humanManager_.showStatusMsg(hydroqgui::IHumanManager::Information, "Path successfully saved to " + fileName );
}

float
PathInput::secondsToCompleteLoop() const
{
    // time from last waypoint to first waypoint to complete the loop
    float timeToCompleteLoop;
    if( wpSettings_->spacingProperty=="Time" ) {
        timeToCompleteLoop = wpSettings_->spacingValue;
    } else {
        timeToCompleteLoop = straightLineDist( guiPath_.last().position - guiPath_.first().position ) / (wpSettings_->spacingValue);
    }
    return timeToCompleteLoop;
}
    
void PathInput::loadPreviousPath()
{
    if (lastSavedPathFile_!="") {
        loadPath( lastSavedPathFile_ );    
    } else {
        humanManager_.showStatusMsg(hydroqgui::IHumanManager::Warning, "No path saved yet!" ); 
    }
    
}

void PathInput::loadPath( const QString& fileName )
{    
    resizeData(0);
    
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        humanManager_.showStatusMsg(hydroqgui::IHumanManager::Error, "Problem opening file " + fileName );
        return;
    }
    
    orca::Path2d orcaPath;
    orca::Waypoint2d wp;
    
    QTextStream in(&file);
    while (!in.atEnd()) 
    {
        stringstream ss( in.readLine().toStdString() );
        orcalogfactory::fromLogString( ss, wp );
        orcaPath.push_back(wp);
    }
    file.close();
    
    orcaPathToGuiPath( orcaPath, guiPath_ );
    waitingTimes_.fill(0.0, orcaPath.size());
    
    humanManager_.showStatusMsg(hydroqgui::IHumanManager::Information, "Successfully loaded file " + fileName );
    wpWidget_->refreshTable();
}

bool
PathFollowerInput::getPath( orca::PathFollower2dData &pathData ) const
{    
    int size = wpWidget_->numberOfLoops() * guiPath_.size();
    //cout << "DEBUG(pathinput.cpp): getPath: size of waypoints is " << size << endl;
    if (size==0) return false;
    
    const float timeOffset = guiPath_.last().timeTarget + secondsToCompleteLoop();
    guiPathToOrcaPath( guiPath_, pathData.path, wpWidget_->numberOfLoops(), timeOffset );
    
    return true;
}

orca::PathPlanner2dTask
PathPlannerInput::getTask() const
{       
    orca::PathPlanner2dTask task;
    guiPathToOrcaPath( guiPath_, task.coarsePath );
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


