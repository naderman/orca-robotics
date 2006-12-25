/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_LOCAL_STATUS_H
#define ORCAICE_LOCAL_STATUS_H

#include "../status.h"

namespace orcaice
{

class LocalStatus : public Status
{
public:
    
    virtual ~LocalStatus() {};

    // Routing is determined by HeartbeatToXxx parameter.
    virtual void heartbeat( const std::string &message, int level=1 ) {};
    
    // Routing is determined by StatusToXxx parameter.
    virtual void status( const std::string &message, bool force=false ) {};
    
    // Returns current status.
    virtual std::string status() const { return "LocalStatus is not implemented"; };
};

} // namespace

#endif
