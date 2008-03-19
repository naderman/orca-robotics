/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alex Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICEGRID_ICEGRID_MANAGER_H
#define ORCAICEGRID_ICEGRID_MANAGER_H

#include <orcaicegrid/abstracticegridmanager.h>
#include <orcaicegrid/icegridsession.h>

namespace orcaicegrid
{

//!
//! @brief Wraps up functions remotely controlling an IceGrid Registry.
//!
//! This thing is thread-safe.
//!
//! Only two exceptions are caught: TimeoutException and ObjectNotExistException. Both
//! indicate that the remote call time out (just now or earlier). Both conditions lead
//! to reinitialization of the session.
//! 
//! Implementations of all methods inhereted from AbstractIceGridManager
//! throw gbxsickacfr::gbxutilacfr::Exception when something goes wrong.
//!
//! @author Alex Brooks
//!
class IceGridManager : public AbstractIceGridManager, public IceGridSession
{
public:
    //! Constructor
    IceGridManager( const orcaice::Context& context,
        const IceGrid::RegistryObserverPrx&     reg =IceGrid::RegistryObserverPrx(), 
        const IceGrid::NodeObserverPrx&         node=IceGrid::NodeObserverPrx(), 
        const IceGrid::ApplicationObserverPrx&  app=IceGrid::ApplicationObserverPrx(), 
        const IceGrid::AdapterObserverPrx&      adpt=IceGrid::AdapterObserverPrx(), 
        const IceGrid::ObjectObserverPrx&       obj=IceGrid::ObjectObserverPrx() );

    // from AdminSessionManager
    virtual IceGrid::ApplicationInfo getApplicationInfo( const std::string &appName, int timeoutMs=-1 );
    virtual void addApplication( IceGrid::ApplicationDescriptor descriptor, int timeoutMs=-1 );
    virtual void updateApplication( IceGrid::ApplicationUpdateDescriptor descriptor, int timeoutMs=-1 );
    virtual void patchApplication( const std::string &appName, bool shutdown, int timeoutMs=-1 );
    virtual void removeApplication( const std::string &appName, int timeoutMs=-1 );

    virtual IceGrid::ServerState getServerState( const std::string &serverId, int timeoutMs=-1 );
    virtual void startServer( const std::string &serverId, int timeoutMs=-1 );
    virtual void stopServer( const std::string &serverId, int timeoutMs=-1 );

    // from IceGridSession
    virtual bool connectedEvent();
    virtual void disconnectedEvent();

private:

    class Operation;
    void performOp( Operation &op, int timeoutMs );

    IceGrid::AdminPrx        iceGridAdmin_;
    IceUtil::Mutex   adminMutex_;

//     int timeoutSec_;

    orcaice::Context context_;
};

} // namespace

#endif
