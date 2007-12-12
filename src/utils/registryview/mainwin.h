/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_REGISTRY_VIEW_MAIN_WINDOW_H
#define ORCA2_REGISTRY_VIEW_MAIN_WINDOW_H

#include <QMainWindow>

#include <orcaqcm/networkthread.h>
#include <orcaqcm/modelthread.h>

class QTreeView;
class QItemDelegate;
class QSplitter;
class QTimer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( orcaqcm::NetworkThread* networkThread, double refreshInterval,
                QWidget *parent = 0, Qt::WFlags flags = 0 );

private slots:
    
    void updateRegistryView();
    void reloadRegistryView();
    void getProperties( const QString & proxy );
    
    void quit();
    void aboutOrca();
    void aboutApp();

private:
    void setupMenuBar();

    orcaqcm::NetworkThread     *networkThread_;
    orcaqcm::ModelThread       *modelThread_;
    
    QTreeView                   *view_;
    QItemDelegate               *delegate_;


    QTimer* refreshTimer_;
};

#endif
