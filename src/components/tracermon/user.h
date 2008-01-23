/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <orca/tracer.h>

namespace tracermon
{

class Network;

class User
{
public:
    virtual ~User() {};

    virtual void enable( Network* network )=0;

    virtual void previousField() = 0;
    virtual void nextField() = 0;
    virtual void valueUp() = 0;
    virtual void valueDown() = 0;
    virtual void action() = 0;

    virtual void newTraceMessage( const orca::TracerData & data ) = 0;

    virtual void newVerbosityLevel( int error, int warn, int info, int debug ) = 0;

    virtual void newLocalTrace( const std::string& msg ) = 0;

};

} // namespace

#endif
