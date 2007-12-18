/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef QTDRIVER_PROBE_VIEW_H
#define QTDRIVER_PROBE_VIEW_H

#include <QListView>

#include <orcaqcm/ocmmodel.h>

#include <orcaprobe/ibrowser.h>

class QLabel;
class QStatusBar;

namespace probe
{

class ProbeView;
class ProbeDelegate;

class ProbeView : public QListView
{
    Q_OBJECT

public:
    ProbeView(  const QString & name,
                orcaqcm::OcmModel       *model,
                QWidget                 *parent=0 );

    void setNeighbors( ProbeView *left,
                       ProbeView *right );

    // hooks up to the model using parent's current index
    void putIntoFocus();

    static void config( orcaprobe::IBrowser *browser,
                        QLabel                   *label,
                        QStatusBar               *statusBar );

private:

    virtual void keyPressEvent( QKeyEvent * event );
    virtual void mousePressEvent( QMouseEvent * event );
    virtual void focusInEvent( QFocusEvent * event );
    virtual void focusOutEvent( QFocusEvent * event );

    virtual void currentChanged( const QModelIndex & current, const QModelIndex & previous );

    QString name_;
    orcaqcm::OcmModel        *model_;
    ProbeDelegate            *delegate_;

    // need to keep track of left/right for several reasons:
    // - blank out right columns on vertical move
    // - display info about parent in label above
    // - hook us up to the model based on parent's current index
    ProbeView               *left_;
    ProbeView               *right_;

    // utilities to blank-out the display
    // turnOff is recursive to the right
    void turnOn();
    void turnOff();
    bool isOn_;

    static orcaprobe::IBrowser *_browser;
    static QLabel                   *_label;
    static QStatusBar               *_statusBar;
};

} // namespace

#endif
