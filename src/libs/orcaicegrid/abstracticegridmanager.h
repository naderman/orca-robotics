/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alex Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICEGRID_ABSTRACT_ICEGRID_MANAGER_H
#define ORCAICEGRID_ABSTRACT_ICEGRID_MANAGER_H

#include <IceGrid/Registry.h>

namespace orcaicegrid
{

//!
//! @brief Creates and maintains an IceGrid admin session.
//!
//! Defines convenient wrappers for a subset of operations provided by IceGrid::Admin interface.
//! 
//! All operations can set a timeout in [ms]. Default: no timeout. Note that because this timeout
//! will be set explicitly, the standard Ice.Override.Timeout will not apply.
//!
//! The implementation must be thread-safe.
//!
//! @author Alex Brooks
//!
class AbstractIceGridManager
{
public:
    //! Tells IceGrid to get app info.
    virtual IceGrid::ApplicationInfo getApplicationInfo( const std::string &appName, int timeoutMs=-1 )=0;

    //! Tells IceGrid to add app from file
    virtual void addApplication( IceGrid::ApplicationDescriptor descriptor, int timeoutMs=-1 )=0;

    //! Tells IceGrid to upadate app from file
    virtual void updateApplication( IceGrid::ApplicationUpdateDescriptor descriptor, int timeoutMs=-1 )=0;

    //! Tells IceGrid to patch app.
    virtual void patchApplication( const std::string &appName, bool shutdown, int timeoutMs=-1 )=0;

    //! Tells IceGrid to remove app
    virtual void removeApplication( const std::string &appName, int timeoutMs=-1 )=0;

    //! Tells IceGrid to get server state
    virtual IceGrid::ServerState getServerState( const std::string &serverId, int timeoutMs=-1 )=0;

    //! Tells IceGrid to start server
    virtual void startServer( const std::string &serverId, int timeoutMs=-1 )=0;

    //! Tells IceGrid to stop server
    virtual void stopServer( const std::string &serverId, int timeoutMs=-1 )=0;
};

} // namespace

#endif
