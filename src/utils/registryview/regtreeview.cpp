/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <QMouseEvent>
#include <QMenu>
#include <QInputDialog>

#include <orcaqcompmodel/ocmmodel.h>

#include "regtreeview.h"

using namespace std;


void
RegTreeView::rowsInserted ( const QModelIndex & parent, int start, int end )
{
    //cout<<"OcmView::rowsInserted "<<start<<" "<<end<<endl;
    //cout<<"OcmView::rowsInserted ("<<start<<","<<end<<") "<<model()->data( model()->index(start,0,parent) ).toString().toStdString()<<endl;
    
    // do what the parent normally does
    QTreeView::rowsInserted( parent, start, end );

    if ( start==0 ) {
        setExpanded( parent, true );
    }
    
    //for ( int i=start; i<=end; ++i ) {
    //    setExpanded( parent.child( i, 0 ), true );
    //}

    // make sure the 'name' column is fully visible
    resizeColumnToContents( 0 );

    // make sure rows are hidden/unhidden properly
    reviewVisibility( parent );
}

void
RegTreeView::contextMenuEvent( QContextMenuEvent* e )
{
    QMenu menu(this);
        
    // only react interfaces
    QString nodeType = currentIndex().data( orcaqcm::OcmModel::TypeRole ).toString();
//     QString nodeType = model()->data( currentIndex(), orcaqcm::OcmModel::TypeRole ).toString();
    if ( nodeType != "Interface" ) {
        return;
    }
    
    // only react the interface we know how to display
    QString ifaceId = currentIndex().sibling(currentIndex().row(),1).data( Qt::DisplayRole ).toString();
//     QString ifaceId = model()->data( currentIndex().sibling(currentIndex().row(),1),  Qt::DisplayRole ).toString();

    // this method is no longer in the interface
//     if ( ifaceId == "::orca::Home" ) {
//         menu.addAction("getProperties()", this, SLOT(home_getProperties()) );
//     }

    menu.exec( e->globalPos() );
}

void
RegTreeView::mouseDoubleClickEvent( QMouseEvent* e )
{
    // only react interfaces
    QString nodeType = currentIndex().data( orcaqcm::OcmModel::TypeRole ).toString();
//     QString nodeType = model()->data( currentIndex(), orcaqcm::OcmModel::TypeRole ).toString();
    if ( nodeType != "Interface" ) {
        return;
    }
    
    // only react the interface we know how to display
    QString ifaceId = currentIndex().sibling(currentIndex().row(),1).data( Qt::DisplayRole ).toString();
//     QString ifaceId = model()->data( currentIndex().sibling(currentIndex().row(),1), Qt::DisplayRole ).toString();

    // this method is no longer in the interface
//     if ( ifaceId == "::orca::Home" ) {
//         home_getProperties();
//     }
}

// void
// RegTreeView::home_getProperties()
// {
//     // only react interfaces
//     QString nodeType = currentIndex().data( orcaqcm::OcmModel::TypeRole ).toString();
// //     QString nodeType = model()->data( currentIndex(), orcaqcm::OcmModel::TypeRole ).toString();
//     if ( nodeType != "Interface" ) {
//         return;
//     }
// 
//     QModelIndex iface = currentIndex().sibling(currentIndex().row(),0);
//     QModelIndex comp = iface.parent();
//     QModelIndex platf = comp.parent();
//     
//     QString proxy = model()->data(iface).toString() + "@"
//                     + model()->data(platf).toString() + "/"
//                     + model()->data(comp).toString();
//     emit propertiesRequested( proxy );
// }
