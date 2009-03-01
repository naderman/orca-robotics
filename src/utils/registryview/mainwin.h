/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

#include <hydroiceutil/jobqueue.h>
#include <orcaice/context.h>

namespace orcaqcm {
    class OcmModel;
};

class QTreeView;
class QItemDelegate;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( hydroiceutil::JobQueue* jobQueue, double refreshInterval, const orcaice::Context& context );

private slots:
    
    void updateRegistryView();
    void reloadRegistryView();
    void getProperties( const QString & proxy );
    
    void quit();
    void aboutOrca();
    void aboutApp();

private:
    void setupMenuBar();

    orcaqcm::OcmModel*      model_;
    QTreeView*              view_;
    QItemDelegate*          delegate_;

    hydroiceutil::JobQueue*    jobQueue_;
    orcaice::Context        context_;

    QTimer*                 refreshTimer_;
};

#endif
