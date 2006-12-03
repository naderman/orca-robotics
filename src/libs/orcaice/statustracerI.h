/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_STATUS_TRACER_I_H
#define ORCAICE_STATUS_TRACER_I_H

#include <orca/status.h>
#include <orcaice/tracer.h>

#include <orcaice/ptrproxy.h>
#include <IceStorm/IceStorm.h>
#include <IceUtil/Mutex.h>

namespace orcaice
{

namespace detail {
    class SysLogger;
}

// An implementation of the (remote) Status interface and local Tracer interfaces.
// Contains no local public API.
class StatusTracerI : public virtual orca::Status, public virtual orcaice::Tracer
{
public:
    
    StatusTracerI( const orcaice::Context & context );

    ~StatusTracerI();

    // orca::Status interface

    virtual ::orca::StatusDataPtr getData(const ::Ice::Current& ) const;
    
    virtual void subscribe(const ::orca::StatusConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::StatusConsumerPrx&, const ::Ice::Current& = ::Ice::Current());


    // orcaice::Tracer interface
    
    // Prints out verbatim to stdout.
    // @see info
    virtual void print( const std::string &message );

    // Routing is determined by OtherToXXX parameter.
    virtual void trace( const std::string &category, const std::string &message, int level=1 );

    // Routing is determined by InfoToXxx parameter.
    virtual void info( const std::string &message, int level=1 );

    // Routing is determined by HeartbeatToXxx parameter.
    void heartbeat( const std::string &message, int level=1 );
    
    // Routing is determined by WarningToXxx parameter.
    virtual void warning( const std::string &message, int level );
    
    // Routing is determined by ErrorToXxx parameter.
    virtual void error( const std::string &message, int level );

    // Routing is determined by DebugToXxx parameter.
    virtual void debug( const std::string &message, int level=1 );
    
    // Routing is determined by StatusToXxx parameter.
    virtual void status( const std::string &message, bool force=false );
    
    // Returns current status.
    virtual std::string status() const;

private:

    void parseConfigFile();
    bool connectToIceStorm();
    void icestormConnectFailed( const orca::FQTopicName &fqTName,
                                orca::StatusConsumerPrx &statusPublisher,
                                bool isStatusTopicRequired );

    Tracer::Config   config_;
    orcaice::Context context_;
    
    orca::FQComponentName compName_;

    std::string prefix_;

    // custom outgoing commands
    void toDisplay( const std::string& category, const std::string& message, int level );
    void toTopic( const std::string& category, const std::string& message, int level );
    void toFile( const std::string& category, const std::string& message, int level );
    void assembleMessage( const std::string& category, const std::string& message, int level, std::string& s );

    std::ofstream *file_;
    detail::SysLogger *sysLogger_;

    std::string prevWarning_;
    std::string prevError_;

    // We only have one communicator but may have multiple threads.
    IceUtil::Mutex mutex_;

    // Status stuff
    PtrProxy<orca::StatusDataPtr> statusPipe_;
    
    IceStorm::TopicPrx statusTopic_;
    orca::StatusConsumerPrx statusPublisher_;
};


} // namespace

#endif
