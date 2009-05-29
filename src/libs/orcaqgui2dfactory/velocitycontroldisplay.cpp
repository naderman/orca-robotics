#include <QTextEdit>
#include <QSplitter>
#include <sstream>
#include <gbxutilacfr/mathdefs.h>
#include "velocitycontroldisplay.h"

using namespace std;

namespace orcaqgui2d {

VelocityControlDisplay::VelocityControlDisplay( const std::string      &platformName, 
                                                QSplitter              *spaceBottomRight ) :
        vx_(0.0), vy_(0.0), w_(0.0)
{    
    // display_ is created in the main GUI thread
    display_ = new QTextEdit();
    
    if (spaceBottomRight!=0)
        spaceBottomRight->addWidget( display_ );
    
    display_->setReadOnly(true);
    display_->setWindowTitle( "Teleop: " + QString(platformName.c_str()) );  
    display_->show();
    
    // a queued connection is setup which is thread safe
    connect(this,SIGNAL(newStringAvailable(const QString&)),display_,SLOT(append(const QString&)),Qt::QueuedConnection);
}

VelocityControlDisplay::~VelocityControlDisplay() 
{
    delete display_;
}

void 
VelocityControlDisplay::sentNewVelocityCommand( double vx, double vy, double w, bool vxLimit, bool vyLimit, bool wLimit )
{
    // save for repeated commands output
    vx_=vx;
    vy_=vy;
    w_=w;
    
    stringstream ss; ss << "New command (vx,vy,w(deg/s)): (" << vx << ", " << vy << ", " << RAD2DEG(w) << ")";  
    emit newStringAvailable( QString(ss.str().c_str()) );
}

void 
VelocityControlDisplay::sentRepeatCommand()
{
    stringstream ss; ss << "Repeated command (vx,vy,w(deg/s)): (" << vx_ << ", " << vy_ << ", " << RAD2DEG(w_) << ")";    
    emit newStringAvailable( QString(ss.str().c_str()) );
}

void 
VelocityControlDisplay::failedToSendCommand( const string &errorMsg )
{   
    emit newStringAvailable( "ERROR: Failed to send command: " + QString( errorMsg.c_str() ) );
}

}



