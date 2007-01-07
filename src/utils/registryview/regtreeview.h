/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_REGVIEW_TREE_VIEW_H
#define ORCA2_REGVIEW_TREE_VIEW_H

#include <orcaqcm/ocmview.h>

class RegTreeView : public orcaqcm::OcmView
{
    Q_OBJECT

public:
    RegTreeView( QWidget *parent = 0 )
            : OcmView(parent) {};

signals:
    void propertiesRequested( const QString & proxy );

protected:

    void mouseDoubleClickEvent( QMouseEvent* e );

    // Reimplement function from QTreeView
    virtual void contextMenuEvent( QContextMenuEvent* e );

protected slots:
    // Reimplement function from QTreeView
    virtual void rowsInserted ( const QModelIndex & parent, int start, int end );
    
private slots:
    void home_getProperties();
    
};

#endif
