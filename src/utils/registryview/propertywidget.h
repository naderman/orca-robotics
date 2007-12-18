/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_PROPERTY_WIDGET_H
#define ORCA_PROPERTY_WIDGET_H

#include <QTreeView>


class PropertyWidget : public QTreeView
{
    Q_OBJECT

public:
    PropertyWidget( const QMap<QString,QString> & props, QWidget *parent = 0, Qt::WFlags flags = 0 );

private:
    QModelIndex populate( QAbstractItemModel* model, const QModelIndex & parent, const QStringList & keys );
};

#endif
