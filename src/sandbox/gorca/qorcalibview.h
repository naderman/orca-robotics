/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
