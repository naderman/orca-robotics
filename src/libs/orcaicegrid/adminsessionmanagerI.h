/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAICEGRID_ADMIN_SESSIONMANAGER_I_H
#define ORCA2_ORCAICEGRID_ADMIN_SESSIONMANAGER_I_H

#include <orcaicegrid/adminsessionmanager.h>
#include <hydroutil/thread.h>
#include <orcaice/context.h>
#include <IceUtil/Time.h>

namespace orcaicegrid
{

//!
//! @brief Creates and maintains an IceGrid admin session.
//!
//! This thing is thread-safe
//!
//! @author Alex Brooks
//!
class AdminSessionManagerI : public AdminSessionManager, public hydroutil::Thread
{
public:
    //! Constructor
    AdminSessionManagerI( const orcaice::Context& context );

    // from AdminSessionManager
    virtual void removeApplication( const std::string &appName );
    virtual IceGrid::ApplicationInfo getApplicationInfo( const std::string &appName );
    virtual IceGrid::ServerState getServerState( const std::string &serverId );
    virtual void startServer( const std::string &serverId );
    virtual void stopServer( const std::string &serverId );
    virtual State getState();

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

    State state_;
    int timeoutSec_;
    IceUtil::Time lastKeepaliveTime_;
    orcaice::Context context_;
    IceUtil::Mutex   mutex_;
};

} // namespace

#endif
