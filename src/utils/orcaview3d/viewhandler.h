#ifndef ORCAVIEW3D_VIEWHANDLER_H
#define ORCAVIEW3D_VIEWHANDLER_H

#include <QMouseEvent>
#include <cmath>
#include <assert.h>
#include <orcaqgui3d/coordinateframe.h>

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

    // event handlers: these aren't actually hooked up to Qt,
    // they have to be called explicitly.
    void mousePressEvent( QMouseEvent* );
    // Returns true if the viewpoint has been modified
    bool mouseMoveEvent( QMouseEvent* );
    // Returns true if the viewpoint has been modified.
    // Sets 'needResize' if the eindow needs a resizeEvent (eg due to a zoom operation)
    bool keyPressEvent(QKeyEvent *e, bool &needResize);

    const orcaqgui3d::CoordinateFrame &pose() const { return cameraPose_; }
    double zoomFactor() const { return zoomFactor_; }

    std::string keyDescription() const;

private: 

    void reset();

    QPointF mouseDownPnt_;
    QPoint prevDragPos_;

    // Current viewpoint
    orcaqgui3d::CoordinateFrame cameraPose_;

    double zoomFactor_;
};

}

#endif
