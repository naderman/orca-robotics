/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAICEGRID_ADMIN_SESSIONMANAGER_H
#define ORCA2_ORCAICEGRID_ADMIN_SESSIONMANAGER_H

#include <IceGrid/Registry.h>

namespace orcaicegrid
{

//!
//! @brief Creates and maintains an IceGrid admin session.
//!
//! The implementation must be thread-safe.
//!
//! @author Alex Brooks
//!
class AdminSessionManager
{
public:
    //! Tells IceGrid to remove app
    virtual void removeApplication( const std::string &appName )=0;

    //! Tells IceGrid to get app info
    virtual IceGrid::ApplicationInfo getApplicationInfo( const std::string &appName )=0;

    //! Tells IceGrid to get server state
    virtual IceGrid::ServerState getServerState( const std::string &serverId )=0;

    //! Tells IceGrid to start server
    virtual void startServer( const std::string &serverId )=0;

    //! Tells IceGrid to stop server
    virtual void stopServer( const std::string &serverId )=0;

    //! State of the Session 
    enum State
    {
        //! Disconnected
        Disconnected,
        //! Connecting
        Connecting,
        //! Connected
        Connected,
        //! Disconnecting
        Disconnecting
    };

    //! Get the current state of the session. This a local operation.
    virtual State getState()=0;
};

} // namespace

#endif
