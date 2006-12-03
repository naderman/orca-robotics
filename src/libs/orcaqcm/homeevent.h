/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_HOME_EVENT_H
#define ORCA2_HOME_EVENT_H

#include <QEvent>
#include <orcacm/types.h>

namespace qorcacm
{

class HomeEvent : public QEvent
{
public:
    HomeEvent( const orcacm::ComponentData & data )
        : QEvent( QEvent::Type(QEvent::User+7154) ),
          data_(data) {};

    orcacm::ComponentData data_;
};

} // namespace

#endif
