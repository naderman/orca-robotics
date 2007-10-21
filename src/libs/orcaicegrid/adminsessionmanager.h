/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAICEGRID_ADMIN_SESSIONMANAGER_H
#define ORCA2_ORCAICEGRID_ADMIN_SESSIONMANAGER_H

#include <IceUtil/Time.h>
#include <IceGrid/Registry.h>
#include <orcaice/context.h>
#include <hydroutil/thread.h>

namespace orcaicegrid
{

//!
//! @brief Creates and maintains an IceGrid admin session.
//!
//! This thing is thread-safe
//!
//! @author Alex Brooks
//!
class AdminSessionManager : public hydroutil::Thread
{
public:
    //! Constructor
    AdminSessionManager( const orcaice::Context& context );

    //! Tells IceGrid to remove app
    void removeApplication( const std::string &appName );

    //! Tells IceGrid to get app info
    IceGrid::ApplicationInfo getApplicationInfo( const std::string &appName );

    //! Tells IceGrid to get server state
    IceGrid::ServerState getServerState( const std::string &serverId );

    //! Tells IceGrid to start server
    void startServer( const std::string &serverId );

    //! Tells IceGrid to stop server
    void stopServer( const std::string &serverId );

    // from Thread
    virtual void run();

private:

    bool tryCreateSession();

    // Tries to sleep for the specified number of seconds, but wakes
    // up if !isStopping() fails.
    void checkedSleep( int sec );

    class Operation;
    void performOp( Operation &op );

    IceGrid::AdminSessionPrx session_;
    IceGrid::AdminPrx        iceGridAdmin_;

    int timeoutSec_;
    IceUtil::Time lastKeepaliveTime_;
    orcaice::Context context_;
    IceUtil::Mutex   mutex_;
};

} // namespace

#endif
