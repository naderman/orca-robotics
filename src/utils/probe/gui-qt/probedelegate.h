/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_PROBE_PROBE_DELEGATE_H
#define ORCA2_PROBE_PROBE_DELEGATE_H

#include <QItemDelegate>

namespace probe
{

class ProbeDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    ProbeDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    bool isOn() { return isOn_; };
    void turnOn() { isOn_=true; };
    void turnOff() { isOn_=false; };

private:
    bool isOn_;
};

} //namespace

#endif
