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
#include <qcursor.h>
#include <qapplication.h>
#include <qinputdialog.h>
#include <q3popupmenu.h>
//Added by qt3to4:
#include <QKeyEvent>
#include <QDropEvent>
#include <QContextMenuEvent>
#include <QDragEnterEvent>
#include <QMouseEvent>
#include <QDesktopWidget>

#include "qorcaprojview.h"
#include "qorcaxmledit.h"


#include <qorcauml/qorcacanvas.h>
#include <qorcauml/qorcacomponent.h>
#include <qorcauml/qorcarequired.h>
#include <qorcauml/qorcaprovided.h>
#include <qorcauml/qorcalink.h>

#include <iostream>
using namespace std;

QPen QorcaProjView::_tempLinkPen( Qt::black, 1, Qt::DashLine );

QorcaProjView::QorcaProjView( Q3Canvas* c, QWidget* parent, const char* name, Qt::WFlags f)
    : Q3CanvasView(c,parent,name,f),
      clickedObject_(0), movingObject_(0), linkingFromObject_(0),
      isLinking_(0)
{
    // set up to get keyboard focus
    setFocusPolicy( Qt::StrongFocus );

    tempLink_ = new Q3CanvasLine( canvas() );
    tempLink_->setPoints( 10,10, 300,300 );
    tempLink_->setZ( 128.0 );
    tempLink_->setPen( _tempLinkPen );
    tempLink_->hide();

    // drag and drop, baby!
    setAcceptDrops(true);
}

void QorcaProjView::contentsMouseDoubleClickEvent(QMouseEvent* e)
{
    setFocus();

    QPoint p = inverseWorldMatrix().map(e->pos());
    Q3CanvasItemList l=canvas()->collisions(p);
    for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) 
    {
        // ports (servers and clients) are found before components
        if ( (*it)->rtti()==RTTI_PROVIDED_INTERFACE || (*it)->rtti()==RTTI_REQUIRED_INTERFACE ) 
        {
            // remember who was clicked
            clickedObject_ = (*it);
            editPortParameters();
            return;
        } 
        else if ( (*it)->rtti() == 5555) 
        {
            // remember who was clicked
            clickedObject_ = (*it);
            editComponentParameters();
            return;
        }
    }
}

void QorcaProjView::contentsMousePressEvent(QMouseEvent* e)
{
    setFocus();
   
    // if it's a right click, don't do anything, we'll handle it in contextMenuEvent()
    // if the object is not seleced, this should probably select it, but it's not worth the trouble
    if ( e->button() == Qt::RightButton ) 
    {
        //cout<<"right click detected"<<endl;
        return;
    }
    
    // where and who did we click?
    QPoint p = inverseWorldMatrix().map(e->pos());
    Q3CanvasItemList l=canvas()->collisions(p);

    cout<<"TRACE(qorcaprojview.cpp): onClick: " << endl;

    // if the linking tool is ON, don't move or select, just link
    if ( isLinking_ ) 
    {
        cout<<"  TRACE(qorcaprojview.cpp): isLinking_" << endl;
        // nothing is moving
        movingObject_ = 0;
        // check where we clicked
        for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
            // draw links from ports only
            if ( (*it)->rtti()==RTTI_PROVIDED_INTERFACE || (*it)->rtti()==RTTI_REQUIRED_INTERFACE ) {
                linkingFromObject_ = *it;
                QorcaInterface* q = (QorcaInterface*)(*it);
                tempLink_->setPoints( q->bubbleX(),q->bubbleY(),q->bubbleX(),q->bubbleY() );
                tempLink_->show();
                return;
            }
        }
        // if no interfaces were clicked, return anyway, do nothing else with SHIFT
        cout<<"  TRACE(qorcaprojview.cpp): no interfaces clicked :(" << endl;
        return;
    }

    // if Ctrl is NOT on, deselect all items
    if ( !(e->state() & Qt::ControlModifier) ) {
        Q3CanvasItemList itemlist = canvas()->allItems();
        for ( Q3CanvasItemList::Iterator it=itemlist.begin(); it!=itemlist.end(); ++it) {
            (*it)->setSelected( false );
        }
    }
    // now see who to select and maybe move
    for ( Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it ) {
        // select/deselect for components and links only
        if ( (*it)->rtti()==5555 ) {
            if ( e->state() & Qt::ControlModifier ) {
                // Ctrl click => toggle
                QorcaComponent* q = (QorcaComponent*)(*it);
                q->toggleSelected();
            } else {
                // pure click => always select
                QorcaComponent* q = (QorcaComponent*)(*it);
                q->setSelected( true );
            }
            // links
        } else if ( (*it)->rtti()==7777 ) {
            // always select
            (*it)->setSelected( true );
        }
        // now check that it's supposed to move at all
        if ( (*it)->!isStopping() ) {
            movingObject_ = *it;
            moving_start_ = p;
            canvas()->update();
            return;
        }
    }

    canvas()->update();
    movingObject_ = 0;
}

void QorcaProjView::contentsMouseMoveEvent(QMouseEvent* e)
{
    setFocus();

    QPoint p = inverseWorldMatrix().map(e->pos());
    // check if something is moving
    if ( movingObject_ ) {
        // depend on who we are dragging
        if ( movingObject_->rtti() == 5555) {
            // debug            
            QRect br = movingObject_->boundingRect();
            QorcaComponent* c = (QorcaComponent*)movingObject_;
            cout<<"QorcaProjView::contentsMouseMoveEvent at ("<<p.x()<<","<<p.y()<<") for "<<c->name().toStdString()
                <<"["<<br.x()<<":"<<br.y()<<","<<br.width()<<":"<<br.height()<<"]"<<endl;
            // pix( QRect(
            //if ( movingObject_->collidesWith( pix ) )              
        
            Q3CanvasItemList movingComps = selectedComponents();
            for (Q3CanvasItemList::Iterator it=movingComps.begin(); it!=movingComps.end(); ++it) {
                (*it)->moveBy(p.x() - moving_start_.x(),
                              p.y() - moving_start_.y());
            }
        } 
        else 
        {
            movingObject_->moveBy(p.x() - moving_start_.x(),
                                  p.y() - moving_start_.y());
        }
        // reset moving start point
        moving_start_ = p;
        canvas()->update();
    } else if ( tempLink_->isVisible() ) {
        // we must be in linking mode, so drag the link line around
        tempLink_->setPoints( tempLink_->startPoint().x(),tempLink_->startPoint().y(), p.x(),p.y() );
        canvas()->update();
    }
}

/*!
    Used to connect components. Must check that mouse was released over a port.
*/
void QorcaProjView::contentsMouseReleaseEvent(QMouseEvent* e)
{
    // stop link tool regardless
    setLinkTool( false );

    // if the link is not displayed, means we are not linking, get out
    if ( !tempLink_->isVisible() ) 
    {
        return;
    }

    // where and who did we un-click?
    QPoint p = inverseWorldMatrix().map(e->pos());
    Q3CanvasItemList l=canvas()->collisions(p);

    for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) 
    {
        // draw links to ports only (server or client doesn't matter)
        if ( validateLink( linkingFromObject_, (*it) ) )
        {
            QorcaInterface* q1 = (QorcaInterface*)linkingFromObject_;
            QorcaInterface* q2 = (QorcaInterface*)(*it);
            
            linkInterfaces( q1, q2 );
            
            break;
        }
    }

    // hide connector, stop moving and repaint regardless
    tempLink_->hide();
    movingObject_ = 0;
    canvas()->update();
}

/*!
  
WARNING: Doesn't validate the link.

*/
void QorcaProjView::linkInterfaces( QorcaInterface *q1, QorcaInterface *q2 )
{
    // create link (xml is not set)
    QorcaLink* q = new QorcaLink( canvas(), q1, q2 );
    q->show();
            
    // tell the ports that they are linked
    q1->linkEvent( *q2 );
    q2->linkEvent( *q1 );
}

bool QorcaProjView::validateLink( Q3CanvasItem *c1, Q3CanvasItem *c2 )
{
    QorcaProvided* p;
    QorcaRequired* r;
    if ( c1->rtti()==RTTI_PROVIDED_INTERFACE && c2->rtti()==RTTI_REQUIRED_INTERFACE ) {
        p = (QorcaProvided*)c1;
        r = (QorcaRequired*)c2;
    } else if ( c1->rtti()==RTTI_REQUIRED_INTERFACE && c2->rtti()==RTTI_PROVIDED_INTERFACE ) {
        p = (QorcaProvided*)c2;
        r = (QorcaRequired*)c1;
    } else {
        cout<<"Couldn't link: not a required/provided pair"<<endl;
        if ( c1->rtti()==RTTI_PROVIDED_INTERFACE && c2->rtti()==RTTI_PROVIDED_INTERFACE )
        {
            cout<<"  - Both are provided."<<endl;
//             cout<<"    endpoint1: " << ((QorcaDebuggable*)c1)->description() << endl;
//             cout<<"    endpoint2: " << ((QorcaDebuggable*)c2)->description() << endl;
        }
        else if ( c1->rtti()==RTTI_REQUIRED_INTERFACE && c2->rtti()==RTTI_REQUIRED_INTERFACE )
        {
            cout<<"  - Both are required."<<endl;
//             cout<<"    endpoint1: " << ((QorcaDebuggable*)c1)->description() << endl;
//             cout<<"    endpoint2: " << ((QorcaDebuggable*)c2)->description() << endl;
        }
        else
        {
            cout<<"  - One is not an interface" << endl;
            cout<<"  - rttis are "<<c1->rtti()<<" and "<<c2->rtti()<<endl;
        }
        return false;
    }

    // Can be sure now that they're both interfaces
    cout<<"TRACE(qorcaprojview.cpp): Validating link:" << endl;
    cout<<"  From: "<<((QorcaInterface*)c1)->description()<<endl;
    cout<<"  To:   "<<((QorcaInterface*)c2)->description()<<endl;

    if ( p->interface() != r->interface() ) {
        cout<<"couldn't link: interface mismatch: "<<p->interface().toStdString()<<" vs. "<<r->interface().toStdString()<<endl;
        return false;
    }

    if ( r->isLinked() ) {
        cout<<"couldn't link: required interface is already linked"<<endl;
        return false;
    }
/*
    if ( p->linkCount() >= p->maxConnections() ) {
        cout<<"couldn't link: provided connections max'd out"<<endl;
        return false;
    }
*/
    return true;
}

void QorcaProjView::contentsDragEnterEvent( QDragEnterEvent *e )
{
    cout<<"entering..."<<endl;
    // Check if you want the drag...
    if (e->mimeData()->hasFormat("text/plain")) {
        e->acceptProposedAction();
    }
}

void QorcaProjView::contentsDropEvent( QDropEvent *e )
{
    cout<<"dropping..."<<endl;
    setFocus();

    // where was it dropped?
    QPoint p = inverseWorldMatrix().map(e->pos());

    if (e->mimeData()->hasFormat("text/plain")) {
        cout<<"QorcaProjView::dropEvent: "<<e->mimeData()->text().toStdString()<<endl;
        emit copyLibToProj( p );
    }

}

void QorcaProjView::contentsContextMenuEvent( QContextMenuEvent *e )
{
    // where and who did we right-click?
    QPoint p = inverseWorldMatrix().map(e->pos());
    Q3CanvasItemList l=canvas()->collisions(p);

    // right-clicked background
    if ( l.isEmpty() ) {
    
        cout<<"QorcaProjView::contentsMouseMoveEvent at ("<<p.x()<<","<<p.y()<<") ["<<e->pos().x()<<","<<e->pos().y()<<"]"<<endl;
            
        e->accept();
        // create context menu
        Q3PopupMenu* pop = new Q3PopupMenu( this );
        pop->insertItem("Toggle Component &Colors", this,SLOT(toggleComponentColors()), Qt::CTRL|Qt::Key_Backslash);
        pop->insertItem("Toggle Component &Naming", this, SLOT(toggleNaming()));
        pop->insertItem("Toggle Component &Deployment Info", this, SLOT(toggleDeployment()));
        pop->insertSeparator();
        //pop->insertItem("Toggle &Link Signatures", this,SLOT(toggleLinkSignatures()));
        //pop->insertSeparator();
        pop->insertItem("Toggle Interface &Types", this,SLOT(togglePortSignatures()));
        pop->insertItem("Toggle Interface Ta&gs", this,SLOT(togglePortTags()));
        pop->insertItem("Toggle Interface Na&mes", this,SLOT(togglePortServiceName()));
        pop->insertSeparator();
        pop->insertItem("Toggle &Hidable Requires", this,SLOT(toggleHidableClients()));
        //        pop->insertSeparator();
        //        pop->insertItem("&Close Project", parent(),SLOT(closeProject()));
        pop->popup( e->globalPos() );
        return;
    }
    // right-clicked something
    for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
        // accept the first one
        switch ( (*it)->rtti() ) {
        case 5555: {
                //cout<<"QorcaProjView::contentsMouseMoveEvent at ("<<p.x()<<","<<p.y()<<") ["<<e->pos().x()<<","<<e->pos().y()<<"]"<<endl;         
                QorcaComponent* c = (QorcaComponent*)(*it);
                QRect br = (*it)->boundingRect();
                cout<<"QorcaProjView::contentsMouseMoveEvent at ("<<p.x()<<","<<p.y()<<") for "<<c->name().toStdString()
                    <<" at "<<c->x()<<","<<c->y()<<endl;
                    //<<"["<<br.x()<<":"<<br.y()<<","<<br.width()<<":"<<br.height()<<"]"<<endl;
                
                e->accept();
                // remember who was clicked
                clickedObject_ = (*it);
                // create context menu
                Q3PopupMenu* pop = new Q3PopupMenu( this );
                pop->insertItem("&Edit Parameters...", this, SLOT(editComponentParameters()));
                pop->insertItem("Set &Platform Name...", this, SLOT(setComponentPlatform()));
                pop->insertItem("Set &Host Name...", this, SLOT(setComponentHost()));
                pop->insertSeparator();
                pop->insertItem("Toggle H&ighlight", this, SLOT(toggleHighlight()));
                pop->insertSeparator();
                pop->insertItem("&Auto-Place Interfaces", this, SLOT(forwardPlacePortsAuto()));
                pop->insertSeparator();
                pop->insertItem("Delete", this, SLOT(deleteComponent()));
                pop->popup( e->globalPos() );
                break;
            }
        case RTTI_PROVIDED_INTERFACE:
        case RTTI_REQUIRED_INTERFACE: {
                e->accept();
                // remember who was clicked
                clickedObject_ = (*it);
                // create context menu
                Q3PopupMenu* pop = new Q3PopupMenu( this );
                pop->insertItem("&Edit Parameters...", this,SLOT(editPortParameters()));
                pop->popup( e->globalPos() );
                break;
            }
        }
    }
}

void QorcaProjView::editComponentParameters()
{
    if ( !clickedObject_ )
        return;
/*
    QorcaComponent* q = (QorcaComponent*)clickedObject_;
    QorcaXmlEdit* xe = new QorcaXmlEdit( q->parameters() );
    xe->move( QApplication::desktop()->width()/2, QApplication::desktop()->height()/2 );
    xe->show();
*/
    clickedObject_ = 0;
}

void QorcaProjView::editPortParameters()
{
    if ( !clickedObject_ )
        return;
//    QorcaInterface* q = (QorcaInterface*)clickedObject_;
    // todo: most of this should not be edited
    /* o2
    QorcaXmlEdit* xe = new QorcaXmlEdit( q->xml() );
    xe->move( QApplication::desktop()->width()/2, QApplication::desktop()->height()/2 );
    xe->show();
    */
    clickedObject_ = 0;
}

void QorcaProjView::setComponentHost() 
{
    // use the host name from the clicked guy as the initially displayed name
    if ( !clickedObject_ )
    {
        return;
    }
    QorcaComponent* q = (QorcaComponent*)clickedObject_;
    clickedObject_ = 0;
    
    // change host name on all currently selected components, 
    Q3CanvasItemList selectedComps = selectedComponents();
    if ( selectedComps.isEmpty() )
    {
        return;
    }
    
    bool ok;
    QString text = QInputDialog::getText(
                       "G-Orca", "Enter host name:", QLineEdit::Normal,
                       q->hostName(), &ok, this );
    if ( ok && !text.isEmpty() ) 
    {
        // user entered something and pressed OK       
        for (Q3CanvasItemList::Iterator it=selectedComps.begin(); it!=selectedComps.end(); ++it) 
        {
            q = (QorcaComponent*)(*it);
            q->setHostName( text );
        }
        canvas()->update();
    } 
    else 
    {
        // user entered nothing or pressed Cancel
    }
}

void QorcaProjView::setComponentPlatform() 
{
    // use the platform name from the clicked guy as the initially displayed name
    if ( !clickedObject_ )
    {
        return;
    }
    QorcaComponent* q = (QorcaComponent*)clickedObject_;
    clickedObject_ = 0;

    // change platform name on all currently selected components, 
    Q3CanvasItemList selectedComps = selectedComponents();
    if ( selectedComps.isEmpty() )
    {
        return;
    }
    
    bool ok;
    QString text = QInputDialog::getText(
                       "G-Orca", "Enter platform name:", QLineEdit::Normal,
                       q->platformName(), &ok, this );
    if ( ok && !text.isEmpty() ) {
        // user entered something and pressed OK   
        for (Q3CanvasItemList::Iterator it=selectedComps.begin(); it!=selectedComps.end(); ++it) 
        {
            q = (QorcaComponent*)(*it);
            q->setPlatformName( text );
        }
        canvas()->update();
    } 
    else 
    {
        // user entered nothing or pressed Cancel
    }
}

void QorcaProjView::forwardPlacePortsAuto()
{
    if ( !clickedObject_ ) return;
    QorcaComponent* q = (QorcaComponent*)clickedObject_;
    q->placePortsAuto();
    canvas()->update();
    clickedObject_ = 0;
}

void QorcaProjView::toggleHighlight()
{
    if ( !clickedObject_ ) return;
    QorcaComponent* q = (QorcaComponent*)clickedObject_;

    q->toggleHighlighted();
    canvas()->update();
    clickedObject_ = 0;
}

void QorcaProjView::deleteComponent() {
    if ( !clickedObject_ ) return;
    QorcaComponent* q = (QorcaComponent*)clickedObject_;
    // kills all the ports first
    q->prepareToDie();
    // will call the destructor, and remove itself from canvas
    delete q;
    canvas()->update();
    clickedObject_ = 0;
}

void QorcaProjView::keyPressEvent( QKeyEvent *e ) {
    if ( e->key() == Qt::Key_F3 ) {
        e->accept();
        // toggle the linking tool state
        toggleLinkTool();
    } else if( e->key() == Qt::Key_Delete ) {
        e->accept();
        // delete selected items
        deleteSelected();
    } else {
        e->ignore();
    }
}

void QorcaProjView::toggleLinkTool() {
    isLinking_ = !isLinking_;
    // change the pointer type
    if ( isLinking_ ) {
        setCursor( QCursor( Qt::PointingHandCursor ) );
    } else {
        setCursor( QCursor( Qt::ArrowCursor ) );
    }
}

void QorcaProjView::setLinkTool( bool on) {
    if ( on ) {
        isLinking_ = true;
        setCursor( QCursor( Qt::PointingHandCursor ) );
    } else {
        isLinking_ = false;
        setCursor( QCursor( Qt::ArrowCursor ) );
    }
}

void QorcaProjView::deleteSelected() {
    //debug
    Q3CanvasItemList al = canvas()->allItems();
    int before = al.size()-1;

    // find all selected
    Q3CanvasItemList l = selected();
    if ( l.size() == 0 )
        return;

    QorcaComponent* citem;
    QorcaInterface* pitem;
    QorcaLink* litem;
    // delete every selected item
    for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
        if ( (*it)->rtti() == 5555 ) {
            citem = (QorcaComponent*)(*it);
            // kills all the ports first
            citem->prepareToDie();
        } else if ( (*it)->rtti() == RTTI_PROVIDED_INTERFACE || (*it)->rtti() == RTTI_REQUIRED_INTERFACE ) {
            pitem = (QorcaInterface*)(*it);
            // kills all the links first
            // NOTE: will likely crash if links and ports are selected at the same time
            pitem->prepareToDie();
        } else if ( (*it)->rtti() == 7777 ) {
            litem = (QorcaLink*)(*it);
            litem->prepareToDie();
        }
        // will call the destructor, and remove itself from canvas
        delete (*it);
    }
    canvas()->update();

    // debug
    al = canvas()->allItems();
    cout<<"before: "<<before<<" after: "<<al.size()-1<<endl;
}

void QorcaProjView::toggleComponentColors() {
    QorcaComponent::toggleColorScheme();

    QorcaCanvas* c = (QorcaCanvas*)canvas();
    Q3CanvasItemList l = c->allComponents();
    QorcaComponent* item;
    for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
        item = (QorcaComponent*)(*it);
        item->setColor();
    } 
    canvas()->update();
}
void QorcaProjView::toggleNaming()
{
    QorcaComponent::toggleNaming();
    /*
    QorcaCanvas* c = (QorcaCanvas*)canvas();
    Q3CanvasItemList l = c->allComponents();
    QorcaComponent* item;
    for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
        item = (QorcaComponent*)(*it);
        item->setLabel();
    } */
    canvas()->update();
}
void QorcaProjView::toggleDeployment()
{
    QorcaComponent::toggleDeployment();
    /*
    QorcaCanvas* c = (QorcaCanvas*)canvas();
    Q3CanvasItemList l = c->allComponents();
    QorcaComponent* item;
    for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
        item = (QorcaComponent*)(*it);
        item->setLabel();
    } */
    canvas()->update();
}

void QorcaProjView::toggleLinkSignatures() {
    QorcaLink::toggleSignature();

    QorcaCanvas* c = (QorcaCanvas*)canvas();
    Q3CanvasItemList l = c->allLinks();
    QorcaLink* item;
    for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
        item = (QorcaLink*)(*it);
        item->setLabel();
    }
    canvas()->update();
}

void QorcaProjView::togglePortTags() {
    QorcaInterface::toggleTag();

    QorcaCanvas* c = (QorcaCanvas*)canvas();
    Q3CanvasItemList l = c->allPorts();
    QorcaInterface* item;
    for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
        item = (QorcaInterface*)(*it);
        item->setLabel();
    }
    canvas()->update();
}

void QorcaProjView::togglePortServiceName() {
    QorcaInterface::toggleServiceName();

    QorcaCanvas* c = (QorcaCanvas*)canvas();
    Q3CanvasItemList l = c->allPorts();
    QorcaInterface* item;
    for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
        item = (QorcaInterface*)(*it);
        item->setLabel();
    }
    canvas()->update();
}

void QorcaProjView::togglePortSignatures() {
    QorcaInterface::toggleSignature();

    QorcaCanvas* c = (QorcaCanvas*)canvas();
    Q3CanvasItemList l = c->allPorts();
    QorcaInterface* item;
    for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
        item = (QorcaInterface*)(*it);
        item->setLabel();
    }
    canvas()->update();
}

void QorcaProjView::toggleHidableClients() {
    Q3CanvasItemList l = allClients();
    QorcaRequired* item;
    for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
        item = (QorcaRequired*)(*it);
        if ( item->isHidable() ) {
            if ( item->isVisible() ) {
                item->reallyHide();
            } else {
                item->reallyShow();
            }
        }
    }
    canvas()->update();
}

Q3CanvasItemList QorcaProjView::allClients() {
    Q3CanvasItemList al = canvas()->allItems();
    Q3CanvasItemList cl;

    for (Q3CanvasItemList::Iterator it=al.begin(); it!=al.end(); ++it) {
        if ( (*it)->rtti() == RTTI_REQUIRED_INTERFACE ) {
            cl.push_back( *it );
        }
    }
    return cl;
}

Q3CanvasItemList QorcaProjView::selectedComponents() {
    Q3CanvasItemList al = canvas()->allItems();
    Q3CanvasItemList cl;

    for (Q3CanvasItemList::Iterator it=al.begin(); it!=al.end(); ++it) {
        // ignore non-components
        if ( (*it)->rtti() == 5555 && (*it)->isSelected() ) {
            cl.push_back( *it );
        }
    }
    return cl;
}

Q3CanvasItemList QorcaProjView::selected()
{
    Q3CanvasItemList al = canvas()->allItems();
    Q3CanvasItemList l;

    for (Q3CanvasItemList::Iterator it=al.begin(); it!=al.end(); ++it) {
        if ( (*it)->isSelected() ) {
            l.push_back( *it );
        }
    }
    return l;
}

void QorcaProjView::clear()
{
    // invalidate pointers
    clickedObject_ = 0;
    movingObject_ = 0;
    linkingFromObject_ = 0;
    
    Q3CanvasItemList list = canvas()->allItems();
    Q3CanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it) {
        // hack: don't delete the temp line
        if ( *it && (*it)->rtti()!=Q3CanvasItem::Rtti_Line )
            delete *it;
    }
    canvas()->update();
}

QString QorcaProjView::tip( const QPoint & pos )
{
/*
    QPoint p = inverseWorldMatrix().map(pos);
    Q3CanvasItemList l=canvas()->collisions(p);
    for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
        if ( (*it)->rtti() == 5555) {
            QorcaComponent* q = (QorcaComponent*)(*it);
            return( q->tip() );
        } else if ( (*it)->rtti()==RTTI_PROVIDED_INTERFACE || (*it)->rtti()==RTTI_REQUIRED_INTERFACE ) {
            QorcaInterface* q = (QorcaInterface*)(*it);
            return( q->signature() );
        }
    }
*/
    return QString();
}
