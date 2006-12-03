/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include "ocmview.h"
#include "ocmmodel.h"   // for role definitions

using namespace std;

namespace qorcacm
{

OcmView::OcmView( QWidget *parent )
        : QTreeView( parent ), isHideDisconnected_(false), isHideStandard_(false)
{
}

// NOTE: could be made more efficient by making up our own signal from
// the model which is emitted only when the connection status is changed.
void
OcmView::dataChanged ( const QModelIndex & topLeft,
                               const QModelIndex & bottomRight )
{
    QAbstractItemView::dataChanged( topLeft, bottomRight );
    
    reviewVisibility( topLeft.parent() );
}

void
OcmView::toggleDisconnected()
{
    isHideDisconnected_ = !isHideDisconnected_;

    reviewVisibility( rootIndex() );
}

void
OcmView::toggleStandard()
{
    isHideStandard_ = !isHideStandard_;

    reviewVisibility( rootIndex() );
}

// NOTE: setRowHidden() for some reason operates on the parent.
void
OcmView::reviewVisibility( const QModelIndex & parent )
{
    QModelIndex child;
    
    // for every child of the parent
    for ( int i=0; i<model()->rowCount(parent); ++i )
    {
        child = model()->index(i,0,parent);
        
        setRowHidden ( i, parent, isToBeHidden( child ) );
    
        // recurse down
        reviewVisibility( child );
    }
}


bool
OcmView::isToBeHidden( const QModelIndex & ind )
{
    // first check for special interfaces, regardless of connection status
    if ( isHideStandard_ && model()->data(ind, OcmModel::TypeRole)=="Interface" )
    {
        QString iface = model()->data( ind.sibling(ind.row(),1) ).toString();
        
        if ( iface=="::orca::Home" || iface=="::orca::Status" ) {
            // this is a standard item, hide
            return true;
        }
    }

    // now check if items are connected
    if ( isHideDisconnected_ && !model()->data(ind, OcmModel::ConnectionRole).toInt() )
    {
        // item is disconnected, hide if instructed
        return isHideDisconnected_;
    }

    // otherwise, do not hide
    return false;
}

void
OcmView::showLevelOne()
{
    expand( rootIndex(), 0 );
}

void OcmView::showLevelTwo()
{

    expand( rootIndex(), 1 );
}

void
OcmView::showLevelThree()
{
    expand( rootIndex(), 2 );
}

void
OcmView::showLevelFour()
{
    expand( rootIndex(), 3 );
}

void
OcmView::expand( const QModelIndex & ind, const int depth )
{
//cout<<"("<<depth<<")  expanding "<<model()->data(ind).toString().toStdString()<<endl;
    setExpanded( ind, true );
    
    QModelIndex child;
    for ( int i=0; i<model()->rowCount(ind); ++i )
    {
        child = model()->index(i,0,ind);
        if ( depth==0 ) {
//cout<<"    collapsing "<<model()->data(child).toString().toStdString()<<endl;
            setExpanded( child, false );
        }
        else {
            expand( child, depth-1 );
        }
    }
}

} // namespace
