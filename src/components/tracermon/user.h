/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TRACERMON_USER_INTERFACE_H
#define ORCA2_TRACERMON_USER_INTERFACE_H

#include "events.h"

namespace tracermon
{

class Network;

class User
{
public:
    virtual ~User() {};

    virtual void enable( Network* network )=0;

    virtual void newTraceMessage( const orca::TracerData & data ) = 0;

};

} // namespace

#endif
