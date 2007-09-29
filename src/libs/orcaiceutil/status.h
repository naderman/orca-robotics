/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICEUTIL_STATUS_H
#define ORCAICEUTIL_STATUS_H

#include <IceUtil/Time.h>

namespace orcaiceutil
{

//!
//! @brief Local interface to component status.
//!
//! @par Overview
//!
//! Status provides a machine-readable interface such that other components can 
//! monitor this component's status.
//!
//! A single Status object is meant to be shared by all threads in the component so the
//! implementation must be thread-safe.
//!
//!
//! The idea is that Status tracks the state of a number of subsystems
//! (possibly one per thread).
//!
//! Each subsystem should call setMaxHeartbeatInterval(), to make the
//! Status engine aware that it exists.  The 'maxHeartbeatIntervalSec'
//! parameter tells the Status engine how often it expects to hear
//! from each subsystem.  If the subsystem has not been heard from for
//! longer than maxHeartbeatIntervalSec, it is assumed that the 
//! subsystem has stalled (hung).
//!
//! Status will publish the entire status of every subsystem whenever
//! anything changes, or every Orca.Status.PublishPeriod, whichever
//! occurs first.
//!
//! @par Local Calls
//!
//! After registering with setMaxHeartbeatInterval, components set
//! their subsystems' status with the various calls.  Each of the calls
//! is sufficient to let the Status engine know that the subsystem is alive.
//! The special call 'heartbeat' lets Status know that the subsystem is
//!  alive without modifying its status.
//!
//! @par Local Calls
//!
//! - @c  Orca.Status.RequireIceStorm (bool)
//!     - orcaiceutil::Component sets up a status and tries to connect to an
//!       IceStorm server on the same host in order to publish component's
//!       status messages. This parameter determines what happens if no server
//!       is found. If set to 0, the startup continues with status messages not
//!       published remotely. If set to 1, the application exits.
//!     - Default: 0
//!
//! - @c  Orca.Status.PublishPeriod (double)
//!     - The minimum interval, in seconds, between remote publishing of status messages.
//!       The actual interval will be less if status changes.
//!     - Default: 30
//!
//! @sa Tracer
//!
class Status
{
    // alexm: this stopped working after separating ComponentThread and Status
    // into different namespaces. will have to ask AB, making process() public
    // for now.
//     friend class ComponentThread; // needs to call process()

public:

    enum SubsystemStatusType
    {
        Initialising,
        Ok,
        Warning,
        Fault,
        Stalled
    };
    
    virtual ~Status() {};

    //! Registers subsystem if it's not registered: should be called
    //! before a sybsystem's status is updated.
    //!
    //! Also sets/modifies maximum expected interval between heartbeats.
    //! When time since last heartbeat exceeds this, alarm is raised.
    virtual void setMaxHeartbeatInterval( const std::string& subsystem,
                                          double maxHeartbeatIntervalSec ) = 0;

    //! Record heartbeat from a subsystem: let Status know the subsystem is alive without
    //! modifying its status.
    virtual void heartbeat( const std::string& subsystem ) = 0;

    //! Set subsystem status to Initialising
    virtual void initialising( const std::string& subsystem, const std::string& message="" ) = 0 ;

    //! Set subsystem status to Ok
    virtual void ok( const std::string& subsystem, const std::string& message="" ) = 0 ;

    //! Set subsystem status to Warning
    virtual void warning( const std::string& subsystem, const std::string& message ) = 0 ;

    //! Set subsystem status to Fault
    virtual void fault( const std::string& subsystem, const std::string& message ) = 0 ;

// alexm, making proces public
// protected:

    virtual void process()=0;
};


} // namespace

#endif
