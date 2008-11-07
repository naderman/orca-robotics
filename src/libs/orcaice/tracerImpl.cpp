/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>

#include "tracerImpl.h"

using namespace std;

namespace orcaice {

//////////////////////////////////////////////////////////////

namespace
{

void 
convert( const gbxutilacfr::TraceType& internal, orca::TraceType& network )
{
    switch ( internal ) 
    {
    case gbxutilacfr::InfoTrace :
        network = orca::InfoTrace;
        break;
    case gbxutilacfr::WarningTrace :
        network = orca::WarningTrace;
        break;
    case gbxutilacfr::ErrorTrace :
        network = orca::ErrorTrace;
        break;
    case gbxutilacfr::DebugTrace :
        network = orca::DebugTrace;
        break;
    // should these be moved somewhere?
    case gbxutilacfr::AnyTrace :
        assert( !"AnyTrace cannot be converted to an orca type" );
        break;
    case gbxutilacfr::NumberOfTraceTypes :
        assert( !"NumberOfTraceTypes cannot be converted to an orca type" );
    }
}

}

//////////////////////////////////////////////////////////////

class TracerI : public orca::Tracer
{
public:

    TracerI( TracerImpl &impl )
        : impl_(impl) {}
    virtual ~TracerI() {}

    // remote interface
    virtual ::orca::TracerVerbosityConfig getVerbosity(const ::Ice::Current&)
        { return impl_.internalGetVerbosity(); };

    virtual void setVerbosity(const ::orca::TracerVerbosityConfig& cfg, const ::Ice::Current&)
        { impl_.internalSetVerbosity(cfg); };

    // subscribe/unsubscribe functions.
    virtual void subscribeForComponentMessages(const ::orca::TracerConsumerPrx &subscriber, const ::Ice::Current&)
        { impl_.internalSubscribe( impl_.componentTopicHandler_, subscriber ); }
    virtual void unsubscribeForComponentMessages(const ::orca::TracerConsumerPrx &subscriber, const ::Ice::Current&)
        { impl_.internalUnsubscribe( impl_.componentTopicHandler_, subscriber ); }
    virtual void subscribeForPlatformInfoMessages(const ::orca::TracerConsumerPrx &subscriber, const ::Ice::Current&)
        { impl_.internalSubscribe( impl_.platformInfoTopicHandler_, subscriber ); }
    virtual void unsubscribeForPlatformInfoMessages(const ::orca::TracerConsumerPrx &subscriber, const ::Ice::Current&)
        { impl_.internalUnsubscribe( impl_.platformInfoTopicHandler_, subscriber ); }
    virtual void subscribeForPlatformWarningMessages(const ::orca::TracerConsumerPrx &subscriber, const ::Ice::Current&)
        { impl_.internalSubscribe( impl_.platformWarningTopicHandler_, subscriber ); }
    virtual void unsubscribeForPlatformWarningMessages(const ::orca::TracerConsumerPrx &subscriber, const ::Ice::Current&)
        { impl_.internalUnsubscribe( impl_.platformWarningTopicHandler_, subscriber ); }
    virtual void subscribeForPlatformErrorMessages(const ::orca::TracerConsumerPrx &subscriber, const ::Ice::Current&)
        { impl_.internalSubscribe( impl_.platformErrorTopicHandler_, subscriber ); }
    virtual void unsubscribeForPlatformErrorMessages(const ::orca::TracerConsumerPrx &subscriber, const ::Ice::Current&)
        { impl_.internalUnsubscribe( impl_.platformErrorTopicHandler_, subscriber ); }

private:
    TracerImpl &impl_;
};

//////////////////////////////////////////////////////////////

TracerImpl::TracerImpl( const orcaice::Context& context ) :
    LocalTracer( hydroutil::Properties( 
                    context.properties()->getPropertiesForPrefix("Orca.Tracer."),"Orca.Tracer."),
                 orcaice::toString(context.name()) ),
    componentTopicHandler_(0),
    platformInfoTopicHandler_(0),
    platformWarningTopicHandler_(0),
    platformErrorTopicHandler_(0),
    interfaceName_("tracer"),
    context_(context)
{
    isInitialized_.set( false );

    initTracerInfo( "Tracer created" );
}

TracerImpl::~TracerImpl()
{
    if ( componentTopicHandler_ )  delete componentTopicHandler_;
    if ( platformInfoTopicHandler_ )    delete platformInfoTopicHandler_;
    if ( platformWarningTopicHandler_ ) delete platformWarningTopicHandler_;
    if ( platformErrorTopicHandler_ )   delete platformErrorTopicHandler_;

    tryRemoveInterface( context_, interfaceName_ );
}

void
TracerImpl::initInterface()
{
    // decide if we need IceStorm topics
    if ( config_.verbosity[gbxutilacfr::AnyTrace][gbxutilacfr::ToNetwork] ) {
        initTopicHandlers();
    }

    ptr_ = new TracerI( *this );

    // previous method: adding Home to the component adapter
//     orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );

    // EXPERIMENTAL! adding as a facet to the Admin interface.
    try
    {
        context_.communicator()->addAdminFacet( ptr_, "Tracer" );
    }
    catch ( const std::exception& e )
    {
        stringstream ss;
        ss << "(while installng Tracer object) : "<<e.what();
        context_.tracer().error( ss.str() );
        context_.shutdown();
    }

    // manually to home registry
    orca::ProvidedInterface iface;
    iface.name = interfaceName_;
    iface.id   = "::orca::Tracer";
    context_.home().addProvidedInterface( iface );
}

void
TracerImpl::tryInitTopicHandler( TracerTopicHandler *&topicHandler, std::string topic, bool isTracerTopicRequired )
{
    if ( topicHandler == 0 )
    {
        topicHandler = new TracerTopicHandler( topic, context_ );
        if ( !topicHandler->connectToTopic() )
        {
            icestormConnectFailed( topic, isTracerTopicRequired );
            delete topicHandler; 
            topicHandler = 0;
        }
    }
}

void
TracerImpl::initTopicHandlers()
{
    // get properties for our component
    Ice::PropertiesPtr props = context_.properties();

    // are we required to connect to status topic? (there's always default value for this property)
    bool isTracerTopicRequired = props->getPropertyAsInt( "Orca.Tracer.RequireIceStorm" );

    // fqTName is something like "tracer/*@platformName/componentName"
    orca::FQTopicName fqTName = orcaice::toTracerTopic( context_.name() );
    tryInitTopicHandler( componentTopicHandler_, orcaice::toString(fqTName), isTracerTopicRequired );

    // alexm: don't try the rest if the first one failed
    bool isIceStormReachable = ( componentTopicHandler_ != 0 );

    if ( isIceStormReachable ) {
        std::string infoTopic = std::string("*/info@")+fqTName.platform+std::string("/*");
        tryInitTopicHandler( platformInfoTopicHandler_, infoTopic, isTracerTopicRequired );
    
        std::string warningTopic = std::string("*/warning@")+fqTName.platform+std::string("/*");
        tryInitTopicHandler( platformWarningTopicHandler_, warningTopic, isTracerTopicRequired );
    
        std::string errorTopic = std::string("*/error@")+fqTName.platform+std::string("/*");
        tryInitTopicHandler( platformErrorTopicHandler_, errorTopic, isTracerTopicRequired );
    }
    else {
        initTracerInfo( "Tracer: failed to connect to the first topic, skipping the rest" );
    }
    
    isInitialized_.set( true );
}

::orca::TracerVerbosityConfig 
TracerImpl::internalGetVerbosity()
{
    ::orca::TracerVerbosityConfig config;

    {
        IceUtil::Mutex::Lock lock(mutex_);
    
        config.info = config_.verbosity[gbxutilacfr::InfoTrace][gbxutilacfr::ToNetwork];
        config.warning = config_.verbosity[gbxutilacfr::WarningTrace][gbxutilacfr::ToNetwork];
        config.error = config_.verbosity[gbxutilacfr::ErrorTrace][gbxutilacfr::ToNetwork];
        config.debug = config_.verbosity[gbxutilacfr::DebugTrace][gbxutilacfr::ToNetwork];
    }

    return config;
}

void 
TracerImpl::internalSetVerbosity( const ::orca::TracerVerbosityConfig& config )
{
    stringstream ss;
    {
        IceUtil::Mutex::Lock lock(mutex_);
    
        if ( config.info > -1 ) {
            config_.verbosity[gbxutilacfr::InfoTrace][gbxutilacfr::ToNetwork]     = config.info;
        }
        if ( config.warning > -1 ) {
            config_.verbosity[gbxutilacfr::WarningTrace][gbxutilacfr::ToNetwork]  = config.warning;
        }
        if ( config.error > -1 ) {
            config_.verbosity[gbxutilacfr::ErrorTrace][gbxutilacfr::ToNetwork]    = config.error;
        }
        if ( config.debug > -1 ) {
            config_.verbosity[gbxutilacfr::DebugTrace][gbxutilacfr::ToNetwork]    = config.debug;
        }

        // pre-calculate marginals: accross trace types
        recalcMarginals();

        ss << "New ToNetwork verbosity levels:"
            << " info="<<config_.verbosity[gbxutilacfr::InfoTrace][gbxutilacfr::ToNetwork]
            << " warn="<<config_.verbosity[gbxutilacfr::WarningTrace][gbxutilacfr::ToNetwork]
            << " error="<<config_.verbosity[gbxutilacfr::ErrorTrace][gbxutilacfr::ToNetwork]
            << " debug="<<config_.verbosity[gbxutilacfr::DebugTrace][gbxutilacfr::ToNetwork];
    }

    // do not call info because it may go back to the network and lock up
    // do it manually
    if ( config_.verbosity[gbxutilacfr::InfoTrace][gbxutilacfr::ToDisplay] ) {
        toDisplay( gbxutilacfr::InfoTrace, ss.str(), 1 );
    }
    if ( config_.verbosity[gbxutilacfr::InfoTrace][gbxutilacfr::ToFile] ) {
        toFile( gbxutilacfr::InfoTrace, ss.str(), 1 );
    }   
    if ( config_.verbosity[gbxutilacfr::InfoTrace][gbxutilacfr::ToLog] ) {
        toLog( gbxutilacfr::InfoTrace, ss.str(), 1 );
    }  
    // NOT calling toNetwork() !!!
}

void
TracerImpl::internalSubscribe( TracerTopicHandler *&topicHandler, const ::orca::TracerConsumerPrx& subscriber )
{
    if ( !topicHandler ) 
    {
        initTopicHandlers();
        if ( !topicHandler )
        {
            throw orca::SubscriptionFailedException("Component does not have a topic to publish its traces.");
        }
    }
    
    topicHandler->subscribe( subscriber );
}

void
TracerImpl::internalUnsubscribe( TracerTopicHandler *topicHandler, const ::orca::TracerConsumerPrx& subscriber )
{
    if ( topicHandler )
        topicHandler->unsubscribe( subscriber );
}

void
TracerImpl::toNetwork( gbxutilacfr::TraceType traceType, const std::string& message, int level )
{
    // do not publish during initialization 
    // (would lead to more traces which will need to be published which will lead to more traces ...)
    bool isInitialized;
    isInitialized_.get( isInitialized );
    if ( !isInitialized )
        return;

    orca::TracerData data;
    orcaice::setToNow( data.timeStamp );
    data.name = context_.name();
    convert( traceType, data.type );
    data.verbosity = level;
    data.message = message;

    if( componentTopicHandler_ != 0 );
    {
        componentTopicHandler_->publish( data );
    }

    if ( platformInfoTopicHandler_ != 0 && traceType == gbxutilacfr::InfoTrace )
    {
        platformInfoTopicHandler_->publish( data );
    }
    else if ( platformWarningTopicHandler_ != 0 && traceType == gbxutilacfr::WarningTrace )
    {
        platformWarningTopicHandler_->publish( data );
    }
    else if ( platformErrorTopicHandler_ != 0 && traceType == gbxutilacfr::ErrorTrace )
    {
        platformErrorTopicHandler_->publish( data );
    }
}

void
TracerImpl::icestormConnectFailed( const std::string &topicName, bool isTracerTopicRequired )
{
    if ( isTracerTopicRequired ) 
    {
        std::string s = prefix_+": Failed to connect to an IceStorm tracer topic '"+
            topicName+"'\n" +
            "\tYou may allow to proceed by setting Orca.Tracer.RequireIceStorm=0.";
        initTracerError( s );
        // this should kill the app
        exit(1);
    }
    else 
    {
        std::string s = prefix_+": Failed to connect to an IceStorm status topic\n";
        s += "\tAll trace messages will be local.\n";
        s += "\tYou may enforce connection by setting Orca.Tracer.RequireIceStorm=1.";
        initTracerWarning( s );

        // turn off all outputs toNetwork
        for ( int i=0; i<gbxutilacfr::NumberOfTraceTypes; ++i ) {
            config_.verbosity[i][gbxutilacfr::ToNetwork] = 0;
        }
        // move on
    }
}

}
