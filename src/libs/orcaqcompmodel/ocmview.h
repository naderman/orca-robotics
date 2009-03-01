/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA_OCM_VIEW_H
#define ORCA_OCM_VIEW_H

#include <QTreeView>
#include <QModelIndex>

namespace orcaqcm
{

//! A view (as in model-view-controller) displaying information about the Orca
//! component model (i.e. a hierarchy of platforms, components, interfaces).
//!
//! @see OcmModel, OcmDelegate
class OcmView : public QTreeView
{
    Q_OBJECT

public:
    OcmView( QWidget *parent = 0 );

public slots:
    void toggleDisconnected();
    void toggleStandard();
    
    void reviewVisibility( const QModelIndex & ind );
    
    void showLevelOne();
    void showLevelTwo();
    void showLevelThree();
    void showLevelFour();

protected slots:
    virtual void dataChanged ( const QModelIndex & topLeft,
                               const QModelIndex & bottomRight );

protected:

    virtual bool isToBeHidden( const QModelIndex & ind );

private:
    void expand( const QModelIndex & ind, const int depth );

    bool isHideDisconnected_;
    bool isHideStandard_;
};

} // namespace

#endif
