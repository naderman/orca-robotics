/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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
namespace detail
{

class LocalStatus : public orcaice::Status
{
public:
    LocalStatus();
    virtual ~LocalStatus() {};
    
    // from orcaice::Status

    virtual void heartbeat( const std::string &message, int level=1 ) {};
    
    virtual void status( const std::string &message, bool force=false ) {};
    
    virtual std::string status() const { return "LocalStatus is not implemented"; };

    virtual IceUtil::Time startTime() const { return startTime_; };

private:

    // Not implemented; prevents accidental use.
    LocalStatus( const LocalStatus & );
    LocalStatus& operator= ( const LocalStatus & );

    IceUtil::Time startTime_;
};

} // namespace
} // namespace

#endif
