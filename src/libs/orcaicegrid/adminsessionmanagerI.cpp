/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <sstream>
#include "adminsessionmanagerI.h"
#include <orcaice/orcaice.h>

using namespace std;

namespace orcaicegrid {

namespace {
    const int MAX_TRIES = 3;
} // namespace

AdminSessionManagerI::AdminSessionManagerI( const orcaice::Context &context ) :
    SafeThread(context.tracer()),
    state_(Disconnected),
    timeoutSec_(0),
    context_(context)     
{
}

bool
AdminSessionManagerI::tryCreateSession()
{
//     cout<<"TRACE(icegridadminsession.cpp): tryCreateSession()" << endl;

    // config file parameters
    Ice::PropertiesPtr props = context_.properties();
    std::string prefix = context_.tag() + ".Config.";
   
    std::string instanceName = context_.properties()->getPropertyWithDefault( 
            prefix+"IceGrid.InstanceName", "IceGrid" );
    IceGrid::RegistryPrx registry;
    
    //
    // connect to the Registry
    //
    try {
        string regName = instanceName+"/Registry";
        registry = IceGrid::RegistryPrx::checkedCast( context_.communicator()->stringToProxy(regName) );
        
        if( !registry )
        {
            context_.tracer().error( "AdminSessionManagerI: Could not contact registry" );
            return false;
        }
    }
    catch ( const Ice::CommunicatorDestroyedException & )
    {
        // This is OK, we're shutting down
        return false;
    }
    catch ( const Ice::Exception &e )
    {
        std::stringstream ss;
        ss << "AdminSessionManagerI: Error contacting registry: " << e;
        context_.tracer().error( ss.str() );
        return false;
    }
    catch ( const std::exception &e )
    {
        std::stringstream ss;
        ss << "AdminSessionManagerI: Error contacting registry: " << e.what();
        context_.tracer().error( ss.str() );
        return false;
    }
    catch ( ... )
    {
        std::stringstream ss;
        ss << "AdminSessionManagerI: Unknown error contacting registry.";
        context_.tracer().error( ss.str() );
        return false;
    }
            
    //
    // Create Session
    //
    try
    {
        session_ = registry->createAdminSession( "sessionmanager.cpp-assume-no-access-control",
                                                 "sessionmanager.cpp-assume-no-access-control" );
        timeoutSec_ = registry->getSessionTimeout();
        stringstream ss; ss<<"AdminSessionManagerI: Created session (timeout="<<timeoutSec_<<"s)";
        context_.tracer().info( ss.str() );

        iceGridAdmin_ = session_->getAdmin();
    }
    catch( const std::exception& e)
    {
        stringstream ss;
        ss << "AdminSessionManagerI: Error creating Admin Session: " << e.what();
        context_.tracer().error( ss.str() );
        return false;
    }
    catch( ... )
    {
        stringstream ss;
        ss << "AdminSessionManagerI: Unknown exception when creating Admin Session.";
        context_.tracer().error( ss.str() );
        return false;
    }

//     cout<<"TRACE(icegridadminsession.cpp): tryCreateSession() returning ok" << endl;
    return true;
}

void
AdminSessionManagerI::checkedSleep( int sec )
{
    for ( int i=0; i < sec; i++ )
    {
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
        if ( !!isStopping() )
            break;
    }
}

void
AdminSessionManagerI::walk()
{
    bool sentKeepalive = false;

    // This outer loop is repeated on session failure.
    while( !isStopping() )
    {
        try {
            // Connect to the Registry and create the session
            {
                IceUtil::Mutex::Lock lock(mutex_);
                state_ = Connecting;
            }
            while ( !isStopping() )
            {
                {
                    IceUtil::Mutex::Lock lock(mutex_);
                    if ( tryCreateSession() )
                        break;
                }
                IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
            }
            // Connected!
            {
                IceUtil::Mutex::Lock lock(mutex_);
                state_ = Connected;
            }

            // Keep it alive
            while ( !isStopping() )
            {
                checkedSleep( timeoutSec_/4 );

                try
                {
                    IceUtil::Mutex::Lock lock(mutex_);
//                     double secSinceLastKeepalive = orcaice::timeDiffAsDouble( orcaice::getNow(), lastKeepaliveTime_ );
                    double secSinceLastKeepalive = ( IceUtil::Time::now()-lastKeepaliveTime_ ).toSecondsDouble();
                    if ( sentKeepalive && secSinceLastKeepalive > timeoutSec_ )
                    {
                        stringstream ss;
                        ss << "AdminSessionManagerI: failed to send keepAlives quicker than timeout.  timeoutSec_="
                           <<timeoutSec_<<", secSinceLastKeepalive=" << secSinceLastKeepalive;
                        context_.tracer().warning( ss.str() );
                    }
                    context_.tracer().debug( "AdminSessionManagerI: sending keepAlive()" );
                    session_->keepAlive();
                    lastKeepaliveTime_ = IceUtil::Time::now(); //orcaice::getNow();
                    sentKeepalive = true;
                }
                catch( const Ice::CommunicatorDestroyedException & )
                {
                    // This is OK, we're shutting down.
                    break;
                }
                catch( const Ice::Exception& e )
                {
                    stringstream ss; ss<<"AdminSessionManagerI: Failed to keep session alive: "<<e;
                    context_.tracer().warning( ss.str() );
                    break;
                }
                catch( const std::exception& e )
                {
                    stringstream ss; ss<<"AdminSessionManagerI: Failed to keep session alive: "<<e.what();
                    context_.tracer().warning( ss.str() );
                    break;
                }
            }
        }
        catch( const Ice::CommunicatorDestroyedException & )
        {
            // This is OK, we're shutting down.
        }
        catch ( std::exception &e )
        {
            stringstream ss;
            ss << "AdminSessionManagerI: Caught stray exception: " << e.what();
            context_.tracer().warning( ss.str() );
        }
        catch ( ... )
        {
            context_.tracer().warning( "AdminSessionManagerI: caught unknown stray exception." );
        }
        
        // Lost it!
        {
            IceUtil::Mutex::Lock lock(mutex_);
            state_ = Disconnected;
        }
    } // while

    
    {
        IceUtil::Mutex::Lock lock(mutex_);
        state_ = Disconnecting;
    }

    try {
        // Destroying the session_ will release all allocated objects.
        context_.tracer().info( "AdminSessionManagerI: Destroying session." );
        session_->destroy();
    }
    catch ( ... )
    {
        // Not much we can do about it...
    }

    {
        IceUtil::Mutex::Lock lock(mutex_);
        state_ = Disconnected;
    }
}

AdminSessionManagerI::State 
AdminSessionManagerI::getState()
{
    IceUtil::Mutex::Lock lock(mutex_);
    return state_;
}

class AdminSessionManagerI::Operation {
public:
    virtual ~Operation() {}

    virtual void performOp( IceGrid::AdminPrx &iceGridAdmin )=0;

    virtual std::string toString() const=0;
};

void
AdminSessionManagerI::performOp( Operation &op )
{
    IceUtil::Mutex::Lock lock(mutex_);

    // cout<<"TRACE(icegridadminsession.cpp): "<<op.toString()<<" got lock" << endl;

    stringstream errorLog;

    for ( int i=0; i < MAX_TRIES; i++ )
    {
        try {
            orca::Time startTime = orcaice::getNow();
            context_.tracer().debug( string("AdminSessionManagerI: performing ")+op.toString(),10 );
            op.performOp( iceGridAdmin_ );
            stringstream ss;
            ss << "AdminSessionManagerI: "<<op.toString()<<" done.  Took "<<orcaice::timeDiffAsDouble(orcaice::getNow(),startTime)<<"s";
            context_.tracer().debug( ss.str(),10 );
            return;
        }
        catch ( const Ice::ObjectNotExistException &e )
        {
            stringstream ss;
            ss << "AdminSessionManagerI: "<<op.toString()<<"(): caught exception: "<<e;
            context_.tracer().warning( ss.str() );
            errorLog << "Error " << i << ": " << ss.str();
            tryCreateSession();
        }
        catch ( const Ice::TimeoutException &e )
        {
            stringstream ss;
            ss << "AdminSessionManagerI: "<<op.toString()<<"(): caught exception: "<<e;
            context_.tracer().warning( ss.str() );
            errorLog << "Error " << i << ": " << ss.str();
            tryCreateSession();
        }
    }

    stringstream ss;
    ss << "AdminSessionManagerI::"<<op.toString()<<": giving up after errors:\n"<<errorLog.str();
    context_.tracer().error( ss.str() );
    throw hydroutil::Exception( ERROR_INFO, ss.str() );    
}


IceGrid::ServerState 
AdminSessionManagerI::getServerState( const std::string &serverId )
{
    class GetServerStateOp : public Operation {
    public:
        GetServerStateOp( std::string serverId )
            : serverId_(serverId) {}

        virtual void performOp( IceGrid::AdminPrx &iceGridAdmin )
            {
                serverState_ = iceGridAdmin->getServerState( serverId_ );
            }
        
        virtual std::string toString() const { return string("getServerState(")+serverId_+")"; }

        std::string serverId_;
        IceGrid::ServerState serverState_;
    };

    GetServerStateOp op(serverId);
    performOp( op );
    return op.serverState_;
}

void 
AdminSessionManagerI::startServer( const std::string &serverId )
{
    class StartServerOp : public Operation {
    public:
        StartServerOp( std::string serverId )
            : serverId_(serverId) {}

        virtual void performOp( IceGrid::AdminPrx &iceGridAdmin )
            { iceGridAdmin->startServer( serverId_ ); }
        
        virtual std::string toString() const { return string("startServer(")+serverId_+")"; }

        std::string serverId_;
    };

    StartServerOp op(serverId);
    performOp( op );
}

void 
AdminSessionManagerI::stopServer( const std::string &serverId )
{
    class StopServerOp : public Operation {
    public:
        StopServerOp( std::string serverId )
            : serverId_(serverId) {}

        virtual void performOp( IceGrid::AdminPrx &iceGridAdmin )
            { iceGridAdmin->stopServer( serverId_ ); }
        
        virtual std::string toString() const { return string("stopServer(")+serverId_+")"; }

        std::string serverId_;
    };

    StopServerOp op(serverId);
    performOp( op );
}

void 
AdminSessionManagerI::removeApplication( const std::string &appName )
{
    class RemoveApplicationOp : public Operation {
    public:
        RemoveApplicationOp( std::string appName )
            : appName_(appName) {}

        virtual void performOp( IceGrid::AdminPrx &iceGridAdmin )
            { iceGridAdmin->removeApplication( appName_ ); }
        
        virtual std::string toString() const { return "removeApplication"; }

        std::string appName_;
    };

    RemoveApplicationOp op(appName);
    performOp( op );
}

IceGrid::ApplicationInfo 
AdminSessionManagerI::getApplicationInfo( const std::string &appName )
{
    class GetApplicationInfoOp : public Operation {
    public:
        GetApplicationInfoOp( std::string appName )
            : appName_(appName) {}

        virtual void performOp( IceGrid::AdminPrx &iceGridAdmin )
            { appInfo_ = iceGridAdmin->getApplicationInfo( appName_ ); }
        
        virtual std::string toString() const { return "getApplicationInfo"; }

        IceGrid::ApplicationInfo appInfo_;
        std::string appName_;
    };

    GetApplicationInfoOp op(appName);
    performOp( op );
    return op.appInfo_;
}

} // namespace
