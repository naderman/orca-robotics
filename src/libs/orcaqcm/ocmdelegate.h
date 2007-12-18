/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA_OCM_DELEGATE_H
#define ORCA_OCM_DELEGATE_H

#include <QItemDelegate>

namespace orcaqcm
{

class OcmDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    OcmDelegate(QObject *parent = 0)
        : QItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
};

} //namespace

#endif
