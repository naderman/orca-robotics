/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef GORCA_LIB_VIEW_H
#define GORCA_LIB_VIEW_H

#include <q3canvas.h>
//Added by qt3to4:
#include <QMouseEvent>

class QorcaLibView : public Q3CanvasView
{
    Q_OBJECT

public:
    QorcaLibView( Q3Canvas*, QWidget* parent=0, const char* name=0, Qt::WFlags f=0);

    void clear();

    QString tip( const QPoint & p );

protected:
    virtual void contentsMousePressEvent(QMouseEvent*);
    virtual void contentsMouseMoveEvent(QMouseEvent*);
    virtual void contentsMouseReleaseEvent(QMouseEvent*);

private:
    Q3CanvasItem* clickedObj_;
    Q3CanvasItem* draggingObj_;
};

#endif
