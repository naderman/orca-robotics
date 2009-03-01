/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alex Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <sstream>
#include "icegridmanager.h"
#include <orcaice/orcaice.h>

using namespace std;

namespace orcaicegrid {

IceGridManager::IceGridManager( const orcaice::Context &context,
        const IceGrid::RegistryObserverPrx& reg, 
        const IceGrid::NodeObserverPrx& node, 
        const IceGrid::ApplicationObserverPrx& app, 
        const IceGrid::AdapterObserverPrx& adpt, 
        const IceGrid::ObjectObserverPrx& obj ) :
    IceGridSession( context, reg, node, app, adpt, obj ),
    observer_(0),
    context_(context)     
{
}

bool 
IceGridManager::connectedEvent()
{   
    IceUtil::Mutex::Lock lock(adminMutex_);

    //
    // connect to the Admin interface
    //
    std::stringstream exceptionSS;
    try
    {
        iceGridAdmin_ = session_->getAdmin();
        context_.tracer().info( "IceGridManager: Connected to Admin interface" );

        // just to test it...
//         vector<string> appNames = iceGridAdmin_->getAllApplicationNames();
//         stringstream ss;
//         ss<<"got a list of "<<appNames.size()<<" app names";
//         context_.tracer().info( ss.str() );

        if ( observer_ )
            observer_->stateChangedEvent( ManagerConnected );
        return true;
    }
    catch ( const Ice::CommunicatorDestroyedException & ) {
        // This is OK, we're shutting down
    }
    catch( const std::exception& e) {
        exceptionSS << "IceGridManager: Error creating Admin Session: " << e.what();
    }
    catch( ... ) {
        exceptionSS << "IceGridManager: Unknown exception when creating Admin Session.";
    }

    if ( !exceptionSS.str().empty() ) {
        context_.tracer().error( exceptionSS.str() );
    }
    return false;
}

void 
IceGridManager::disconnectedEvent()
{
    IceUtil::Mutex::Lock lock(adminMutex_);
    iceGridAdmin_ = 0;

    if ( observer_ )
        observer_->stateChangedEvent( ManagerDisconnected );
    context_.tracer().info( "IceGridManager: Disconnected from Admin interface" );
}

class IceGridManager::Operation {
public:
    virtual ~Operation() {}

    virtual void perform( IceGrid::AdminPrx &iceGridAdmin )=0;

    virtual std::string toString() const=0;
};

void
IceGridManager::performOp( Operation &op, int timeoutMs )
{
    // serialize access to admin proxy
//     IceUtil::Mutex::Lock lock(adminMutex_);

    // minimize critical section in order to be able to perform multiple operations
    // simultaneously
    IceGrid::AdminPrx iceGridAdmin;
    gbxutilacfr::Tracer* tracer;
    {
        IceUtil::Mutex::Lock lock(adminMutex_);

        if ( !iceGridAdmin_ ) {
            string warn = "Operation "+op.toString()+" could not be performed because the proxy to IceGrid/Admin is NULL. "
                        + "Session state: "+IceGridSession::toString( getState() );
            throw SessionNotConnectedException( ERROR_INFO, warn );
        }

        // NOTE: cannot currently change timeout because changing timeout invalidates the secure session!
        // use the specified timeout
//         if ( timeoutMs>0 )
//             iceGridAdmin = IceGrid::AdminPrx::uncheckedCast( iceGridAdmin_->ice_timeout( timeoutMs ) );
//         else
            iceGridAdmin = iceGridAdmin_;

        tracer = &context_.tracer();
    }
    // end of critical section

    stringstream exceptionSS;
    gbxiceutilacfr::Timer timer;
    try {
        stringstream debugSS;
        debugSS<<"IceGridManager: performing "<<op.toString()<<" with timeout="<<timeoutMs<<"ms";
        tracer->debug( debugSS.str(),5 );
//         context_.tracer().debug( debugSS.str(),5 );

        // notice the use of the local copy of the Admin proxy
        op.perform( iceGridAdmin );
//         op.perform( iceGridAdmin_ );

        stringstream ss;
        ss << "IceGridManager: "<<op.toString()<<" done.  Took "<<timer.elapsedSec()<<"s";
        tracer->debug( ss.str(),10 );
//         context_.tracer().debug( ss.str(),10 );
        return;
    }
    catch ( const Ice::ObjectNotExistException &e ) {
        exceptionSS << "IceGridManager: "<<op.toString()<<"(): caught exception after "
                <<timer.elapsedSec()<<"s : "<<e;
    }
    catch ( const Ice::TimeoutException &e ) {
        exceptionSS << "IceGridManager: "<<op.toString()<<"(): caught exception after "
                <<timer.elapsedSec()<<"s : "<<e;
    }

    tracer->error( exceptionSS.str() );
//     context_.tracer().error( exceptionSS.str() );
    // destroy old admin proxy, it's no longer valid
//     {
//         IceUtil::Mutex::Lock lock(adminMutex_);
//         iceGridAdmin_ = 0;
//     }
//     tryCreateSession();

    throw gbxutilacfr::Exception( ERROR_INFO, exceptionSS.str() );    
}


IceGrid::ServerState 
IceGridManager::getServerState( const std::string &serverId, int timeoutMs )
{
    class GetServerStateOp : public Operation {
    public:
        GetServerStateOp( std::string serverId ) :
            serverId_(serverId) {};

        virtual void perform( IceGrid::AdminPrx &iceGridAdmin )
            {
                serverState_ = iceGridAdmin->getServerState( serverId_ );
            }
        
        virtual std::string toString() const { return string("getServerState(")+serverId_+")"; }

        std::string serverId_;
        IceGrid::ServerState serverState_;
    };

    GetServerStateOp op(serverId);
    performOp( op, timeoutMs );
    return op.serverState_;
}

void 
IceGridManager::startServer( const std::string &serverId, int timeoutMs )
{
    class StartServerOp : public Operation {
    public:
        StartServerOp( std::string serverId ) :
            serverId_(serverId) {};

        virtual void perform( IceGrid::AdminPrx &iceGridAdmin )
            { iceGridAdmin->startServer( serverId_ ); }
        
        virtual std::string toString() const { return string("startServer(")+serverId_+")"; }

        std::string serverId_;
    };

    StartServerOp op(serverId);
    performOp( op, timeoutMs );
}

void 
IceGridManager::stopServer( const std::string &serverId, int timeoutMs )
{
    class StopServerOp : public Operation {
    public:
        StopServerOp( std::string serverId ) :
            serverId_(serverId) {};

        virtual void perform( IceGrid::AdminPrx &iceGridAdmin )
            { iceGridAdmin->stopServer( serverId_ ); }
        
        virtual std::string toString() const { return string("stopServer(")+serverId_+")"; }

        std::string serverId_;
    };

    StopServerOp op(serverId);
    performOp( op, timeoutMs );
}

void 
IceGridManager::addApplication( IceGrid::ApplicationDescriptor descriptor, int timeoutMs )
{
    class AddApplicationOp : public Operation {
    public:
        AddApplicationOp( IceGrid::ApplicationDescriptor descriptor ) :
            descriptor_(descriptor) {};

        virtual void perform( IceGrid::AdminPrx &iceGridAdmin )
            { iceGridAdmin->addApplication( descriptor_ ); }
        
        virtual std::string toString() const { return "addApplication ("+descriptor_.name+")"; }

        IceGrid::ApplicationDescriptor descriptor_;
    };

    AddApplicationOp op(descriptor);
    performOp( op, timeoutMs );
}

void 
IceGridManager::updateApplication( IceGrid::ApplicationUpdateDescriptor descriptor, int timeoutMs )
{
    class UpdateApplicationOp : public Operation {
    public:
        UpdateApplicationOp( IceGrid::ApplicationUpdateDescriptor descriptor ) :
            descriptor_(descriptor) {};

        virtual void perform( IceGrid::AdminPrx &iceGridAdmin )
            { iceGridAdmin->updateApplication( descriptor_ ); }
        
        virtual std::string toString() const { return "updateApplication ("+descriptor_.name+")"; }

        IceGrid::ApplicationUpdateDescriptor descriptor_;
    };

    UpdateApplicationOp op(descriptor);
    performOp( op, timeoutMs );
}

void 
IceGridManager::patchApplication( const std::string &appName, bool shutdown, int timeoutMs )
{
    class PatchApplicationOp : public Operation {
    public:
        PatchApplicationOp( std::string appName, bool shutdown ) :
            appName_(appName), shutdown_(shutdown) {};

        virtual void perform( IceGrid::AdminPrx &iceGridAdmin )
            { iceGridAdmin->patchApplication( appName_, shutdown_ ); }
        
        virtual std::string toString() const { return "patchApplication( "+appName_+")"; }

        std::string appName_;
        bool shutdown_;
    };

    PatchApplicationOp op(appName, shutdown);
    performOp( op, timeoutMs );
}

void 
IceGridManager::removeApplication( const std::string &appName, int timeoutMs )
{
    class RemoveApplicationOp : public Operation {
    public:
        RemoveApplicationOp( std::string appName ) :
            appName_(appName) {};

        virtual void perform( IceGrid::AdminPrx &iceGridAdmin )
            { iceGridAdmin->removeApplication( appName_ ); }
        
        virtual std::string toString() const { return "removeApplication( "+appName_+")"; }

        std::string appName_;
    };

    RemoveApplicationOp op(appName);
    performOp( op, timeoutMs );
}

IceGrid::ApplicationInfo 
IceGridManager::getApplicationInfo( const std::string &appName, int timeoutMs )
{
    class GetApplicationInfoOp : public Operation {
    public:
        GetApplicationInfoOp( std::string appName ) :
            appName_(appName) {};

        virtual void perform( IceGrid::AdminPrx &iceGridAdmin )
            { appInfo_ = iceGridAdmin->getApplicationInfo( appName_ ); }
        
        virtual std::string toString() const { return "getApplicationInfo"; }

        std::string appName_;
        IceGrid::ApplicationInfo appInfo_;
    };

    GetApplicationInfoOp op(appName);
    performOp( op, timeoutMs );
    return op.appInfo_;
}

IceGridManagerState 
IceGridManager::state()
{
    IceUtil::Mutex::Lock lock(adminMutex_);
    
    if ( iceGridAdmin_ )
        return ManagerConnected;
    else
        return ManagerDisconnected;
}

void 
IceGridManager::setObserver( IceGridManagerObserver* observer )
{
    IceUtil::Mutex::Lock lock(adminMutex_);
    observer_ = observer;
}

} // namespace
