/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
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
