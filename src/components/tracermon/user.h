/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TRACERMON_USER_INTERFACE_H
#define ORCA2_TRACERMON_USER_INTERFACE_H

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
