#include "viewhandler.h"
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <gbxutilacfr/mathdefs.h>

using namespace std;

namespace orcaview3d {

ViewHandler::ViewHandler()
{
    camera_.setLookDir( Vector3(1,0,0) );
    camera_.setV( Vector3(0,0,1) );
    camera_.setU( cross(camera_.getDir(),-camera_.getV()) );

    camera_.setPos( Vector3( -7, 0, 2 ) );
}

void 
ViewHandler::applyViewingTransformation() const
{
//     cout<<"TRACE(viewhandler.cpp): "<<__func__<<": x,y,z:     "<<camera_.getPos().x<<","<<camera_.getPos().y<<","<<camera_.getPos().z << endl;
//     cout<<"TRACE(viewhandler.cpp): "<<__func__<<": roll="<<camera_.getRoll()*180.0/M_PI<<", pitch="<<camera_.getPitch()*180.0/M_PI<<", yaw="<<camera_.getYaw()*180.0/M_PI << endl;

    // cout<<"TRACE(viewhandler.cpp): camera_: " << toString(camera_) << endl;

    Vector3 center = camera_.getPos() + camera_.getDir();
    gluLookAt( camera_.getPos().x,
               camera_.getPos().y,
               camera_.getPos().z,
               center.x, center.y, center.z,
               camera_.getV().x, camera_.getV().y, camera_.getV().z );
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
        camera_.yawPlanet( M_PI/180.0 * sensitivity*dx*10 );
        camera_.pitch( M_PI/180.0 * sensitivity*dy*10 );
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

bool 
ViewHandler::keyPressEvent(QKeyEvent *e)
{
    bool needUpdate=true;

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
        Vector3 fwd = camera_.getDir();
        fwd.z = 0;
        normalise(fwd);
        camera_.move(amtLin*fwd);
        break;
    }
    case Qt::Key_K:
    {
        Vector3 fwd = camera_.getDir();
        fwd.z = 0;
        normalise(fwd);
        camera_.move(-amtLin*fwd);
        break;
    }
    case Qt::Key_U:
    {
        Vector3 left = camera_.getU();
        camera_.move(amtLin*left);
        break;
    }
    case Qt::Key_O:
    {
        Vector3 left = camera_.getU();
        camera_.move(-amtLin*left);
        break;        
    }
    case Qt::Key_J:
    {
        camera_.yawPlanet(amtRot);
        break;
    }
    case Qt::Key_L:
    {
        camera_.yawPlanet(-amtRot);
        break;        
    }
    case Qt::Key_Up:
    {
        camera_.move( Vector3( 0, 0, amtLin ) );
        break;
    }
    case Qt::Key_Down:
    {
        camera_.move( Vector3( 0, 0, -amtLin ) );
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

