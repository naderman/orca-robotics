/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
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
        menu.addAction("Add to Project", this, SLOT(addElement()) );
    }
    
    // react to platforms
    if ( currentIndex().data( orcaqcm::OcmModel::TypeRole) == "Platform" )
    {
        menu.addAction("Add all supported interfaces", this, SLOT(addAllPlatformInterfaces()) );
    }
    
    // react to component
    if ( currentIndex().data( orcaqcm::OcmModel::TypeRole) == "Component" )
    {
        menu.addAction("Add all supported interfaces", this, SLOT(addAllComponentInterfaces()) );
    }
    
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

void RegSelectView::addAllPlatformInterfaces()
{
    for (int i=0; i<selectionModel()->selectedRows().size(); i++)
    {
        QString platform = selectionModel()->selectedRows()[i].data().toString();
        cout<<"INFO(regselectview.cpp): addAllInterfaces of platform: " << platform.toStdString() << endl;
        
        orcaqcm::OcmModel* ocmModel = (orcaqcm::OcmModel*)model();
        QStringList registries;
        QStringList platforms;
        QStringList components;
        QStringList interfaces;
        QStringList ids;
        // lookup interface information in the model
        ocmModel->interfacesOnPlatform( selectionModel()->selectedRows()[i], registries, platforms, components, interfaces, ids );
        
        for (int i=0; i<interfaces.size(); i++)
        {
            cout << "Interface: " << interfaces[i].toStdString() << endl;
            QList<QStringList> interfacesInfo;
            QStringList interfaceInfo;
            interfaceInfo << registries[i] << platforms[i] << components[i] << interfaces[i] << ids[i];
            interfacesInfo << interfaceInfo;
            emit newSelection( interfacesInfo );
        }
    }
}

void RegSelectView::addAllComponentInterfaces()
{   
    for (int i=0; i<selectionModel()->selectedRows().size(); i++)
    {
        QString component = selectionModel()->selectedRows()[i].data().toString();
        cout<<"INFO(regselectview.cpp): addAllInterfaces of component: " << component.toStdString() << endl;
        
        orcaqcm::OcmModel* ocmModel = (orcaqcm::OcmModel*)model();
        QStringList registries;
        QStringList platforms;
        QStringList components;
        QStringList interfaces;
        QStringList ids;
        // lookup interface information in the model
        ocmModel->interfacesOnComponent( selectionModel()->selectedRows()[i], registries, platforms, components, interfaces, ids );
        
        for (int i=0; i<interfaces.size(); i++)
        {
            cout << "Interface: " << interfaces[i].toStdString() << endl;
            QList<QStringList> interfacesInfo;
            QStringList interfaceInfo;
            interfaceInfo << registries[i] << platforms[i] << components[i] << interfaces[i] << ids[i];
            interfacesInfo << interfaceInfo;
            emit newSelection( interfacesInfo );
        }
    }
    
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
