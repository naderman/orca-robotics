/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_STATUS_H
#define ORCAICE_STATUS_H

namespace orcaice
{

/*!
 *  Local interface to component status.
 *
 *  @see Tracer
 */
class Status
{
public:
    
    virtual ~Status() {};

    //! Routing is determined by HeartbeatToXxx parameter.
    virtual void heartbeat( const std::string &message, int level=1 ) = 0;
    
    //! Routing is determined by StatusToXxx parameter.
    virtual void status( const std::string &message, bool force=false ) =0 ;
    
    //! Returns current status.
    virtual std::string status() const = 0;

    //! The time when the component was activated
    virtual IceUtil::Time startTime() const = 0;
};


} // namespace

#endif
