/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef NETWORK_INTERFACE_H
#define NETWORK_INTERFACE_H

namespace tracermon
{

class Network
{
public:
    virtual ~Network() {};

    // Set level=-1 to keep it unchanged.
    virtual void setVerbosityLevel( int error, int warn, int info, int debug ) = 0;
};

} // namespace

#endif
