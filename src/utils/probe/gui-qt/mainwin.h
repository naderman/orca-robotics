/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PROBE_QTDRIVER_MAIN_WINDOW_H
#define ORCA2_PROBE_QTDRIVER_MAIN_WINDOW_H

#include <QMainWindow>

#include <orcaprobe/browserdriver.h>

class QListView;
class QTableView;
class QTreeView;
class QLabel;
class QMovie;
class QTimer;

namespace orcaqcm
{
class OcmModel;
}

namespace probe
{

class ProbeView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( orcaprobe::BrowserDriver *browser,
                orcaqcm::OcmModel        *model,
                const std::vector<std::string> & supportedInterfaces,
                QWidget *parent = 0, Qt::WFlags flags = 0 );

private slots:
    void reload();
    void top();
   
    void quit();
    void aboutOrca();
    void aboutApp();

private:

    virtual void keyPressEvent ( QKeyEvent * event );
    virtual void customEvent( QEvent* e );

    void setupMenuBar();

    orcaprobe::BrowserDriver *browser_;
    orcaqcm::OcmModel        *model_;
    std::vector<std::string>    supportedInterfaces_;
    
    ProbeView                *regView_;
    ProbeView                *platfView_;
    ProbeView                *compView_;
    ProbeView                *ifaceView_;
    ProbeView                *operView_;
    
    QLabel *pathLabel_;
    QMovie *movie_;
};

} // namespace

#endif
