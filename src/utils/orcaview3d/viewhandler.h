#ifndef ORCAVIEW3D_VIEWHANDLER_H
#define ORCAVIEW3D_VIEWHANDLER_H

#include <QMouseEvent>

namespace orcaview3d {

//!
//! @brief Handles pan/zoom via mouse dragging
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

private: 

    QPointF mouseDownPnt_;
    QPoint prevDragPos_;

    // Current viewpoint
    float zoomFactor_;
    float xOffset_, yOffset_, zOffset_;
    float yaw_, pitch_;

};

}

#endif
