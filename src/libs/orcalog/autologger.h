/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_LOGGER_AUTO_LOGGER_H
#define ORCA_LOGGER_AUTO_LOGGER_H

#include <orcalog/logwriterinfo.h>
#include <orcalog/masterfilewriter.h>
#include <orcaifaceimpl/consumerImpl.h>
#include <orcaice/timeutils.h>

namespace orcalog
{

//! An AutoLogger simply logs continuously once it's started.    
class AutoLogger {
public:
    virtual ~AutoLogger() {}
    
    virtual void init( const LogWriterInfo &logWriterInfo, MasterFileWriter &masterFileWriter )=0;
    virtual void startLogging()=0;
};

//!
//! The generic version which will fit most use-cases.
//! has a hook to allow something special to happen on initialisation (eg 'getDescription')
//!
template<class ProviderType, 
         class ConsumerType,  
         class ObjectType,
         class LogWriterType>
class GenericAutoLogger : 
        public orcaifaceimpl::ConsumerImpl<ProviderType,ConsumerType,ObjectType>, 
        public orcalog::AutoLogger
{
    using orcaifaceimpl::ConsumerSubscriber::context_;
    typedef typename ProviderType::ProxyType ProviderPrxType;
    typedef typename ConsumerType::ProxyType ConsumerPrxType;

public:
    GenericAutoLogger( const orcaice::Context &context ) :
        orcaifaceimpl::ConsumerImpl<ProviderType,ConsumerType,ObjectType>( context )
    {
    }

    virtual void dataEvent( const ObjectType& data ) 
    {
        logWriter_->write( data, orcaice::getNow() );
    }

    // Inherited from orcalog::AutoLogger
    virtual void init( const orcalog::LogWriterInfo &logWriterInfo,
                       orcalog::MasterFileWriter    &masterFileWriter )
    {
        logWriter_.reset( new LogWriterType );
        logWriter_->checkFormat( logWriterInfo.format );
        logWriter_->init( logWriterInfo, masterFileWriter );
    }

    virtual void startLogging()
    {
        // may throw
        ProviderPrxType providerPrx;
        orcaice::connectToInterfaceWithTag<ProviderPrxType>( 
                context_, 
                providerPrx,
                logWriter_->logWriterInfo().interfaceTag );

        // Allow derived classes to do something special (like get description)
        setupEvent( providerPrx, *logWriter_ );

        subscribeWithTag( logWriter_->logWriterInfo().interfaceTag );
    }

protected:

    // Customizable event, called during initialisation
    virtual void setupEvent( ProviderPrxType &providerPrx, LogWriterType &logWriter ) {}

private:

    std::auto_ptr<LogWriterType> logWriter_;
};

}

#endif
