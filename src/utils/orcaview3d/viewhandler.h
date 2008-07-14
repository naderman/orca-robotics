#ifndef ORCAVIEW3D_VIEWHANDLER_H
#define ORCAVIEW3D_VIEWHANDLER_H

#include <QMouseEvent>
#include <cmath>
#include <assert.h>

namespace orcaview3d {

//!
//! @brief Handles the camera's view position via keypresses and mouse dragging
//!
//! @author Alex Brooks
//!
class ViewHandler
{

public: 

    ViewHandler();

    // 'Puts the camera in position'
    // Calls OpenGL commands to modify the matrix
    void applyViewingTransformation() const;

    // event handlers: these aren't actually hooked up to Qt,
    // they have to be called explicitly.
    void mousePressEvent( QMouseEvent* );
    // Returns true if the viewpoint has been modified
    bool mouseMoveEvent( QMouseEvent* );
    // Returns true if the viewpoint has been modified
    bool keyPressEvent(QKeyEvent *e);

    // Get the pose of the camera in world-coordinates, in S.I. units.
    // (might be stored weirdly internally for OpenGL-reasons)
    float x() const { return xOffset_; }
    float y() const { return yOffset_; }
    float z() const { return zOffset_; }
    float roll() const { return M_PI*180.0*(0.0); }
    float pitch() const 
        {
            assert( false && "Not sure how this should be defined...." );
            return M_PI/180.0*pitch_; 
        }
    float yaw() const { return M_PI/180.0*(yaw_-90); }
        

private: 

    QPointF mouseDownPnt_;
    QPoint prevDragPos_;

    // Current viewpoint
    float xOffset_, yOffset_, zOffset_;
    float yaw_, pitch_;

};

}

#endif
