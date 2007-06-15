/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <assert.h>

#include <QtGui>

#include "probeview.h"
#include "probedelegate.h"

using namespace std;
using namespace probe;

IBrowser* ProbeView::_browser = 0;
QLabel* ProbeView::_label = 0;
QStatusBar* ProbeView::_statusBar = 0;

void 
ProbeView::config( IBrowser *browser,
                   QLabel                   *label,
                   QStatusBar               *statusBar )
{
    _browser = browser;
    _label = label;
    _statusBar = statusBar;

    assert(_browser || "null pointer to browser");
    assert(_label || "null pointer to label");
    assert(_statusBar || "null pointer to statusBar");
}

ProbeView::ProbeView(   const QString & name,
                        orcaqcm::OcmModel        *model,
                        QWidget *parent)
    : QListView(parent),
      name_(name),
      model_(model),
      left_(0),
      right_(0),
      isOn_(false)
{
    assert(model_ || "null pointer to model");
    
//     setModel( model_ );
    setFrameStyle( QFrame::Box | QFrame::Plain );
    
    // we can still change focus with arrow keys
    setFocusPolicy( Qt::NoFocus );
    setUpdatesEnabled ( true);

    delegate_ = new ProbeDelegate( this );
    setItemDelegate( delegate_ );
}

void 
ProbeView::setNeighbors( ProbeView *left, ProbeView *right )
{
    left_ = left;
    right_ = right;
}

void 
ProbeView::keyPressEvent( QKeyEvent * event )
{
//     cout<<"ProbeView::keyPressEvent"<<endl;

    // pre-process
    switch (event->key()) 
    {
    case Qt::Key_Enter : 
    case Qt::Key_Return :
    case Qt::Key_Right : 
    {
        if ( right_ && currentIndex().isValid() ) {
            cout<<"selecting item "<<currentIndex().row()<<endl;
            _browser->choosePick( currentIndex().row() );
//             right_->turnOn();
//             right_->setFocus( Qt::OtherFocusReason );
        }
        break; 
    }
    case Qt::Key_Left : 
    {
        if ( left_ ) {
            _browser->chooseUp();
//             left_->setFocus( Qt::OtherFocusReason );
        }
        break; 
    }
//     case Qt::Key_Up : 
//     case Qt::Key_Down : {
//         if ( right_ ) {
//             right_->turnOff();
//         }
//         break; 
    default :
        break;
    } //  switch

    //
    // Standard implementation
    //
    QListView::keyPressEvent(event);
}

void 
ProbeView::mousePressEvent( QMouseEvent * event )
{
    // disable mouse pressing because we know the state of
    // the browser by keeping track of movements with arrow keys
}

void 
ProbeView::focusInEvent ( QFocusEvent * event ) 
{
    setLineWidth( 3 );
    QWidget::focusInEvent( event );

    _statusBar->showMessage( currentIndex().data( orcaqcm::OcmModel::DetailRole).toString() );

    if ( left_ ) {
        _label->setText( left_->currentIndex().data( orcaqcm::OcmModel::SummaryRole).toString() );
    }
    else {
        _label->setText( model_->index(0,0,QModelIndex()).data( orcaqcm::OcmModel::SummaryRole).toString() );
    }
}

void 
ProbeView::focusOutEvent ( QFocusEvent * event ) 
{
    setLineWidth( 1 );
    QWidget::focusInEvent( event );
}

void 
ProbeView::currentChanged( const QModelIndex & current, const QModelIndex & previous )
{
    _statusBar->showMessage( currentIndex().data( orcaqcm::OcmModel::DetailRole).toString() );

    if ( right_ ) {
        right_->turnOff();
    }
}

void
ProbeView::turnOn()
{
    if ( isOn_ ) return;
//     cout<<name_.toStdString()<<": turning ON"<<endl;
    isOn_ = true;

    setModel( model_ );
}

void
ProbeView::turnOff()
{
    if ( !isOn_ ) return;
//     cout<<name_.toStdString()<<": turning OFF"<<endl;
    isOn_ = false;

    setModel( 0 );

    if ( right_ ) {
        right_->turnOff();
    }
}

void
ProbeView::putIntoFocus()
{
    turnOn();
    if ( left_ ) {
        setRootIndex( left_->currentIndex() );
    }
    else {
        setRootIndex( model_->index(0,0,QModelIndex()) );
    }
    setCurrentIndex( rootIndex().child(0,0) );
}
