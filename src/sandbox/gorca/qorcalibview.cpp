/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <qmatrix.h>
#include <qmessagebox.h>
#include <qtooltip.h>
//#include <q3dragobject.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QDrag>

#include "qorcalibview.h"

#include <qorcauml/qorcacanvas.h>
#include <qorcauml/qorcacomponent.h>
#include <qorcauml/qorcainterface.h>

#include <iostream>
using namespace std;

QorcaLibView::QorcaLibView( Q3Canvas* c, QWidget* parent, const char* name, Qt::WFlags f)
    : Q3CanvasView(c,parent,name,f), clickedObj_(0), draggingObj_(0)
{
    //new QorcaLibTip();
}

void QorcaLibView::contentsMousePressEvent( QMouseEvent* e )
{
    // where and who did we click?
    QPoint p = inverseWorldMatrix().map(e->pos());

    // now see who to select and maybe move
    Q3CanvasItemList cl=canvas()->collisions(p);
    for (Q3CanvasItemList::Iterator it=cl.begin(); it!=cl.end(); ++it)
    {
        // select/deselect for components only
        if ( (*it)->rtti() == 5555)
        {
            if ( e->state() & Qt::ControlModifier ) {
                // Ctrl click => toggle
                QorcaComponent* q = (QorcaComponent*)(*it);
                q->toggleSelected();
            } else {
                // pure click => always select, deselect others if not selected
                QorcaComponent* q = (QorcaComponent*)(*it);
                if ( q->isSelected() ) {}
                else {
                    QorcaCanvas* c = (QorcaCanvas*)canvas();
                    c->deselectAllComponents();
                    q->setSelected( true );
                }
                // remember this, if unclick right here => deselect all others
                clickedObj_ = *it;
            }
            // remember who we clicked for dragging
            draggingObj_ = *it;
            canvas()->update();
            return;
        }
    }

    // if no component found, deselect all
    Q3CanvasItemList al = canvas()->allItems();
    for ( Q3CanvasItemList::Iterator it=al.begin(); it!=al.end(); ++it) {
        if ( (*it)->rtti() == 5555) {
            QorcaComponent* q = (QorcaComponent*)(*it);
            q->setSelected( false );
        } else {
            (*it)->setSelected( false );
        }
    }
    canvas()->update();
}

void QorcaLibView::contentsMouseMoveEvent( QMouseEvent* e )
{
    if (!(e->buttons() & Qt::LeftButton)) {
        return;
    }
    
    if ( draggingObj_ ) {
        // where is the mouse now?
        QPoint p = inverseWorldMatrix().map(e->pos());
        Q3CanvasRectangle* q = (Q3CanvasRectangle*)draggingObj_;
        
        // start dragging if we moved the mouse outside the dragging item
        if ( !q->rect().contains( p ) ) {
                //cout<<"dragging..."<<endl;
                // special component drag
                // not really dragging any information, the receiver will signal back
                // to the mainwindow who has access to the library
            QDrag *drag = new QDrag(this);
            QMimeData *mimeData = new QMimeData;
    
            mimeData->setText("drag king!");
            drag->setMimeData(mimeData);
            drag->start(Qt::CopyAction | Qt::MoveAction);
            
            // reset the pointer, so we don't check this again
            draggingObj_ = 0;
        }
    }
}

void QorcaLibView::contentsMouseReleaseEvent( QMouseEvent* e )
{
    // regardless of whether dropped or not, stop dragging
    if ( draggingObj_ ) {
        draggingObj_ = 0;
    }
    // if there was no pure click on a component, nothing else to check
    if ( !clickedObj_ ) {
        //cout<<"released: no clicker"<<endl;
        return;
    }
    //cout<<"released: clicker"<<endl;
    // where and who did we unclick?
    QPoint p = inverseWorldMatrix().map(e->pos());
    Q3CanvasItemList cl=canvas()->collisions(p);
    Q3CanvasItem* unclicker = 0;
    for (Q3CanvasItemList::Iterator it=cl.begin(); it!=cl.end(); ++it) {
        // find uncliker component
        if ( (*it)->rtti() == 5555) {
            // remember who we unclicked
            unclicker = *it;
            //cout<<"released: found unclicker"<<endl;
        }
    }

    // if unclick on the same guy who was (pure) clicked, deselect everybody
    // note: can't check if Cntl is still on when release
    if ( unclicker==clickedObj_ ) {
        QorcaCanvas* c = (QorcaCanvas*)canvas();
        c->deselectAllComponents();
        // now select the clicker
        QorcaComponent* q = (QorcaComponent*)clickedObj_;
        q->setSelected( true );
    }
    // regardless, reset clickedObj_
    clickedObj_ = 0;
    canvas()->update();
}

void QorcaLibView::clear()
{
    // invalidate pointers
    clickedObj_=0;
    draggingObj_=0;
    
    Q3CanvasItemList list = canvas()->allItems();
    Q3CanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it) {
        delete *it;
    }
    canvas()->update();
}

QString QorcaLibView::tip( const QPoint & pos )
{
/*
    QPoint p = inverseWorldMatrix().map(pos);
    Q3CanvasItemList l=canvas()->collisions(p);
    for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
        if ( (*it)->rtti() == 5555) {
            QorcaComponent* q = (QorcaComponent*)(*it);
            return( q->tip() );
        } else if ( (*it)->rtti()==6666 || (*it)->rtti()==6667 ) {
            QorcaInterface* q = (QorcaInterface*)(*it);
            return( q->signature() );
        }
    }
*/
    return QString();
}
