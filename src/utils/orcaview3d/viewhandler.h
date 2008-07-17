#ifndef ORCAVIEW3D_VIEWHANDLER_H
#define ORCAVIEW3D_VIEWHANDLER_H

#include <QMouseEvent>
#include <cmath>
#include <assert.h>
#include "coordinateframe.h"

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

    const CoordinateFrame &pose() const { return cameraPose_; }

private: 

    QPointF mouseDownPnt_;
    QPoint prevDragPos_;

    // Current viewpoint
    CoordinateFrame cameraPose_;

};

}

#endif
