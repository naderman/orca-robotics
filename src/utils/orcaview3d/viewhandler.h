#ifndef ORCAVIEW3D_VIEWHANDLER_H
#define ORCAVIEW3D_VIEWHANDLER_H

#include <QMouseEvent>
#include <cmath>
#include <assert.h>
#include "camera.h"

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
    float x() const { return camera_.getPos().x; }
    float y() const { return camera_.getPos().y; }
    float z() const { return camera_.getPos().z; }
    float roll() const { assert(false); return camera_.getRoll(); }
    float pitch() const { assert(false); return camera_.getPitch(); }
    float yaw() const { assert(false); return camera_.getYaw(); }
        

private: 

    QPointF mouseDownPnt_;
    QPoint prevDragPos_;

    // Current viewpoint
    Camera camera_;

};

}

#endif
