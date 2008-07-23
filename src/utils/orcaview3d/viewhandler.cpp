#include "viewhandler.h"
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <gbxutilacfr/mathdefs.h>
#include <sstream>

using namespace std;

namespace orcaview3d {

ViewHandler::ViewHandler()
{
    reset();
}

void
ViewHandler::reset()
{
    zoomFactor_ = 1.0;
    cameraPose_ = orcaqgui3d::CoordinateFrame();

    // Reasonable starting point: where we can see the origin
    cameraPose_.translate( orcaqgui3d::Vector3(-2,0,2) );
}

void
ViewHandler::mousePressEvent( QMouseEvent* e )
{
    prevDragPos_ = e->pos();
}

bool
ViewHandler::mouseMoveEvent( QMouseEvent* e )
{
    const float sensitivity = 0.02;

    float dx=0, dy=0;
    if ( e->buttons() & Qt::LeftButton ||
         e->buttons() & Qt::RightButton ||
         e->buttons() & Qt::MidButton )
    {
        dx = e->pos().x() - prevDragPos_.x();
        dy = e->pos().y() - prevDragPos_.y();        
        prevDragPos_ = e->pos();        
    }

    // cout<<"TRACE(worldview3d.cpp): dx, dy: " << dx << "," << dy << endl;

    if ( e->buttons() & Qt::LeftButton )
    {
        // Rotate camera
        cameraPose_.increaseYawPlanet( sensitivity*dx*10*M_PI/180.0 );
        cameraPose_.increasePitch( sensitivity*dy*10*M_PI/180.0 );
        return true;
    }
#if 0
    else if ( e->buttons() & Qt::RightButton )
    {
        // Pan camera (relative to the view-point)
        float x = sensitivity*dx*0.1;
        float y = sensitivity*dy*0.1;
        // cout<<"TRACE(viewhandler.cpp): x,y: " << x << ", " << y << endl;

        float yawRad = yaw_*M_PI/180.0;

        xOffset_ += x*cos(yawRad) - y*sin(yawRad);
        yOffset_ -= x*sin(yawRad) + y*cos(yawRad);
        //cout<<"TRACE(worldview3d.cpp): pan" << endl;
        return true;
    }
#endif
    return false;
}

std::string
ViewHandler::keyDescription() const
{
    stringstream ss;
    ss << endl
       << " u i o        : " << endl
       << " j k l        : move camera in x-y plane" << endl
       << "              : " << endl
       << " up/dn arrows : move cam in z-dimension" << endl
       << " r            : reset camera params" << endl
       << " +/-          : zoom" << endl;
    return ss.str();
}

bool 
ViewHandler::keyPressEvent( QKeyEvent *e, bool &needResize )
{
    bool needUpdate=true;
    needResize = false;

    double amtLin = 0.2;
    double amtRot = 5*M_PI/180.0;
    if ( e->modifiers() & Qt::ShiftModifier )
    {
        amtLin *= 8;
        amtRot *= 2;
    }
    else if ( e->modifiers() & Qt::ControlModifier )
    {
        amtLin /= 8;
        amtRot /= 2;
    }

    switch ( e->key() )
    {
    case Qt::Key_I:
    {
        orcaqgui3d::Vector3 fwd = cameraPose_.fwd();
        fwd.z() = 0;
        fwd.normalise();
        cameraPose_.translate(amtLin*fwd);
        break;
    }
    case Qt::Key_K:
    {
        orcaqgui3d::Vector3 fwd = cameraPose_.fwd();
        fwd.z() = 0;
        fwd.normalise();
        cameraPose_.translate(-amtLin*fwd);
        break;
    }
    case Qt::Key_U:
    {
        cameraPose_.translate(amtLin*cameraPose_.left());
        break;
    }
    case Qt::Key_O:
    {
        cameraPose_.translate(-amtLin*cameraPose_.left());
        break;        
    }
    case Qt::Key_J:
    {
        cameraPose_.increaseYawPlanet(amtRot);
        break;
    }
    case Qt::Key_L:
    {
        cameraPose_.increaseYawPlanet(-amtRot);
        break;        
    }
    case Qt::Key_Up:
    {
        cameraPose_.translate( orcaqgui3d::Vector3( 0, 0, amtLin ) );
        break;
    }
    case Qt::Key_Down:
    {
        cameraPose_.translate( orcaqgui3d::Vector3( 0, 0, -amtLin ) );
        break;        
    }
    case Qt::Key_R:
    {
        reset();
        needResize = true;
        break;        
    }
    case Qt::Key_Minus:
    {
        zoomFactor_ /= 1.05;
        needResize = true;
        break;
    }
    case Qt::Key_Plus:
    {
        zoomFactor_ *= 1.05;
        needResize = true;
        break;
    }
    default:
    {
        needUpdate=false;
        break;
    }
    }
    return needUpdate;
}

}

