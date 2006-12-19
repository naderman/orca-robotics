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
#include <QMouseEvent>
#include <QMenu>
#include <QInputDialog>
#include <QHeaderView>

#include <orcaqcm/ocmmodel.h>   // need to access custom API

#include "regselectview.h"

using namespace std;

namespace orcaqgui {
    
RegSelectView::RegSelectView( QWidget *parent )
        : OcmView(parent)
{
    // make sure the 'name' column is fully visible
//     resizeColumnToContents( 0 );
//     expandAll();
//     header()->resizeSection( 0, 200 );
}

void RegSelectView::contextMenuEvent( QContextMenuEvent* e )
{
    QMenu menu(this);
    
    // only react interfaces
    if ( currentIndex().data( orcaqcm::OcmModel::TypeRole) == "Interface" )
    {
        if ( currentIndex().data() == "::orca::Platform2d" ) {
            menu.addAction("Add to Project as OdometryPainter", this, SLOT(addElement()) );
        }
        else {
            menu.addAction("Add to Project", this, SLOT(addElement()) );
        }
        menu.addSeparator();
    }
    
//     menu.addAction("&Properties" );
    menu.exec( e->globalPos() );
}

void RegSelectView::mouseDoubleClickEvent( QMouseEvent* e )
{
    // only react interfaces
    if ( currentIndex().data( orcaqcm::OcmModel::TypeRole) == "Interface" ) {
        addElement();
    }
}

void RegSelectView::addElement()
{    
    QList<QStringList> interfacesInfo;
            
    for (int i=0; i<selectionModel()->selectedRows().size(); i++)
    {
        cout<<"INFO(regselectview.cpp): addElement: " <<selectionModel()->selectedRows()[i].data().toString().toStdString()<<endl;
        
        orcaqcm::OcmModel* ocmModel = (orcaqcm::OcmModel*)model();
        QString registry;
        QString platform;
        QString component;
        QString interface;
        QString id;
        // lookup interface information in the model
        ocmModel->interfaceData( selectionModel()->selectedRows()[i], registry, platform, component, interface, id );
        QStringList interfaceInfo;
        interfaceInfo << registry<<platform<<component<<interface<<id;
        interfacesInfo << interfaceInfo;
    }
    emit newSelection( interfacesInfo );
    
}

void RegSelectView::addRegistry()
{
    bool ok;
    QString text = QInputDialog::getText(
            this, "Add Registry", "Enter registry proxy to add:", QLineEdit::Normal, "", &ok );

    if ( ok && !text.isEmpty() ) {        
        // user entered something and pressed OK
        emit registryRequested( text );
    } else {
        // user entered nothing or pressed Cancel
    }
}

}
