/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCA_ORCAICEGRID_SESSIONMANAGER_H
#define ORCA_ORCAICEGRID_SESSIONMANAGER_H

#include <IceGrid/Registry.h>
#include <orcaice/context.h>
#include <hydroiceutil/safethread.h>

namespace orcaicegrid
{

//!
//! Derive from this and pass it the SessionManager if you 
//! want to do anything on connection.
//!
class SessionCreationCallback {
public:

    virtual ~SessionCreationCallback() {};

    // Returns: 
    // - true  = everything is OK
    // - false = something didn't work, we should re-establish the session.
    //
    // Any exceptions thrown by this method will be caught by SessionManager.
    //
    virtual bool actionOnSessionCreation( IceGrid::AdminSessionPrx session )=0;
};

//!
//! @brief Creates and maintains an IceGrid admin session.
//!
//! @author Alex Brooks
//!
class SessionManager : public hydroiceutil::SafeThread
{
public:

    SessionManager( SessionCreationCallback &sessionCreationCallback,
                    const orcaice::Context& context );

    virtual void walk();

private:

    bool tryCreateSession();

    // Tries to sleep for the specified number of seconds, but wakes
    // up if !isStopping() fails.
    void checkedSleep( int sec );

    SessionCreationCallback &sessionCreationCallback_;

    IceGrid::AdminSessionPrx session_;

    int timeoutSec_;
    orcaice::Context context_;
};

} // namespace

#endif
