/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PROBE_USER_DRIVER_INTERFACE_H
#define ORCA2_PROBE_USER_DRIVER_INTERFACE_H

namespace probe
{

class UserDriver
{

public:

    virtual ~UserDriver() {};

    // need a separate enable function because the Qt driver does not return
    virtual void enable()=0;

};

} // namespace

#endif
