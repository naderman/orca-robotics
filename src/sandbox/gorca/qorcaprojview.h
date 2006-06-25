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

#ifndef GORCA_PROJ_VIEW_H
#define GORCA_PROJ_VIEW_H

#include <q3canvas.h>
//Added by qt3to4:
#include <QKeyEvent>
#include <QDropEvent>
#include <QContextMenuEvent>
#include <QDragEnterEvent>
#include <QMouseEvent>

//#include <qorcauml/qorcamodel.h>

// fwd decl
class QorcaInterface;

class QorcaProjView : public Q3CanvasView
{
    Q_OBJECT

public:
    QorcaProjView( Q3Canvas*, 
                   QWidget* parent=0, 
                   const char* name=0, 
                   Qt::WFlags f=0);
    void clear();

    QString tip( const QPoint & p );

    //! WARNING: Doesn't validate the link.
    void linkInterfaces( QorcaInterface *q1, QorcaInterface *q2 );

public slots:
    void toggleLinkTool();
    void deleteSelected();

signals:
    void copyLibToProj( const QPoint& );

private slots:
    void forwardPlacePortsAuto();
    void editComponentParameters();
    void editPortParameters();
    void setComponentPlatform();
    void setComponentHost();
    void deleteComponent();
    void toggleNaming();
    void toggleDeployment();
    void toggleHighlight();
    void toggleComponentColors();
    void toggleLinkSignatures();
    void togglePortTags();
    void togglePortServiceName();
    void togglePortSignatures();
    void toggleHidableClients();

protected:
    virtual void contentsMouseDoubleClickEvent(QMouseEvent*);
    virtual void contentsMousePressEvent(QMouseEvent*);
    virtual void contentsMouseMoveEvent(QMouseEvent*);
    virtual void contentsMouseReleaseEvent(QMouseEvent*);

    virtual void contentsDragEnterEvent( QDragEnterEvent *e );
    virtual void contentsDropEvent( QDropEvent *e );
    virtual void contentsContextMenuEvent( QContextMenuEvent *e );
    virtual void keyPressEvent( QKeyEvent * );

private:
    Q3CanvasItem* clickedObject_;
    Q3CanvasItem *movingObject_;
    QPoint moving_start_;

    void setLinkTool( bool on );
    Q3CanvasLine* tempLink_;
    static QPen _tempLinkPen;
    Q3CanvasItem* linkingFromObject_;
    bool validateLink( Q3CanvasItem *q1, Q3CanvasItem *q2 );
    bool isLinking_;

    Q3CanvasItemList allClients();
    Q3CanvasItemList selectedComponents();
    Q3CanvasItemList selected();
};

#endif
