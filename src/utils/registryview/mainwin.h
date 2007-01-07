/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_REGISTRY_VIEW_MAIN_WINDOW_H
#define ORCA2_REGISTRY_VIEW_MAIN_WINDOW_H

#include <QMainWindow>

#include <orcaqcm/networkhandler.h>
#include <orcaqcm/modelhandler.h>

class QTreeView;
class QItemDelegate;
class QSplitter;
class QTimer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( orcaqcm::NetworkHandler* networkHandler, double refreshInterval,
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

    orcaqcm::NetworkHandler     *networkHandler_;
    orcaqcm::ModelHandler       *modelHandler_;
    
    QTreeView                   *view_;
    QItemDelegate               *delegate_;


    QTimer* refreshTimer_;
};

#endif
