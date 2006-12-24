/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TRACERMON_NETWORK_INTERFACE_H
#define ORCA2_TRACERMON_NETWORK_INTERFACE_H

namespace tracermon
{

class Network
{
public:
    virtual ~Network() {};

    virtual void setVerbosityLevel( int error, int warn=-1, int info=-1, int debug=-1 ) = 0;

};

} // namespace

#endif
