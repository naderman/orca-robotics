/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_REGISTRY_SELECT_VIEW_H
#define ORCA2_REGISTRY_SELECT_VIEW_H

#include <orcaqcm/ocmview.h>

namespace orcaqgui {

class RegSelectView : public orcaqcm::OcmView
{
    Q_OBJECT

public:
    RegSelectView( QWidget *parent = 0 );

signals:
    void newSelection( const QList<QStringList> &interfacesInfo );
    void registryRequested( const QString & proxy );

protected:
    // Reimplement function from QTreeView
    virtual void contextMenuEvent( QContextMenuEvent* e );
    virtual void mouseDoubleClickEvent ( QMouseEvent* e );

private slots:
    void addElement();
    void addRegistry();
    void addAllPlatformInterfaces();
    void addAllComponentInterfaces();
    
};

}
#endif
