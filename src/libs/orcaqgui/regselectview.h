/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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
    void interfaceToBeAdded( const QStringList & iterfaceInfo );
    void registryRequested( const QString & proxy );

protected:
    // Reimplement function from QTreeView
    virtual void contextMenuEvent( QContextMenuEvent* e );
    virtual void mouseDoubleClickEvent ( QMouseEvent* e );

private slots:
    void addSelectionIndividually();
    void addSelectionCombined();
    void addRegistry();
    
};

}
#endif
