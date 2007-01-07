/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include "propertywidget.h"

#include <QStandardItemModel>
#include <QHeaderView>


using namespace std;

PropertyWidget::PropertyWidget( const QMap<QString,QString> & props, QWidget *par, Qt::WFlags flags)
        : QTreeView( par )
{
    //
    // Widget
    //
    setMinimumWidth( 400 );
    setMinimumWidth( 100 );
    setWindowTitle("Orca2: Properties");

    //
    // Model
    //
    QStandardItemModel* model = new QStandardItemModel;
    
    //
    // View
    //
    setModel( model );
    setRootIsDecorated(true);
    header()->setMovable(true);
    setUniformRowHeights(true);

    //
    // Populate the model
    //
    model->setHeaderData( 0, Qt::Horizontal, "Key" );
    model->setHeaderData( 1, Qt::Horizontal, "Value" );
    
    QModelIndex root = model->index(0, 0, QModelIndex() );
    model->insertColumns(0, 2, root);
    
    QMap<QString,QString>::const_iterator it = props.constBegin();
    
    QModelIndex parent;
    QStringList keys;
    
    while ( it != props.constEnd() )
    {
        keys = it.key().split( "." );
        parent = root;

        while ( keys.size() )
        {
            parent = populate( model, parent, keys );
            keys.removeFirst();
        }
        model->setData( parent.sibling(parent.row(),1), it.value() );
        ++it;
    }

    // make sure the 'name' column is fully visible
    resizeColumnToContents( 0 );
}

QModelIndex PropertyWidget::populate( QAbstractItemModel* model, const QModelIndex & parent, const QStringList & keys )
{
    int childCount = model->rowCount(parent);
    cout<<keys[0].toStdString()<<" "<<childCount<<" children"<<endl;
    
    for ( int i=0; i<childCount; ++i )
    {
        //cout<< model->data( model->index(i,0,parent)).toString().toStdString() << endl;
        
        if ( keys[0] == model->data( model->index(i,0,parent)) ) {
            // exists
            return model->index(i,0,parent);
        }
    }

    // does not exist
    cout<<"inserting"<<endl;
    model->insertRows( childCount, 1, parent);

    QModelIndex child = model->index( childCount, 0, parent );
    model->setData( child, keys[0] );
    model->insertColumns(0, 2, child);

    // expand non-boring items
    if ( keys[0]=="Ice" || keys[0]=="IceStorm" || keys[0]=="Orca" ) {
        this->setExpanded( child, false );
    }
    else {
        this->setExpanded( child, true );
    }
    
    return child;
}
