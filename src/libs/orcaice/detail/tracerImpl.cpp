/*
 * Orca-Robotics Project: Components for robotics
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include <orcaice/icegridutils.h>
#include <orcaice/convertutils.h>

#include "tracerImpl.h"

using namespace std;

namespace orcaice {
namespace detail {

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
    virtual ::orca::TracerVerbosityConfig getVerbosity(const ::Ice::Current&)
        { return impl_.internalGetVerbosity(); }
    virtual void setVerbosity(const ::orca::TracerVerbosityConfig& cfg, const ::Ice::Current&)
        { impl_.internalSetVerbosity(cfg); }
    virtual IceStorm::TopicPrx subscribe(const ::orca::TracerConsumerPrx& consumer, const ::Ice::Current& = ::Ice::Current())
        { return impl_.internalSubscribe( consumer ); }
private:
    TracerImpl &impl_;
};

//////////////////////////////////////////////////////////////

TracerImpl::TracerImpl( const orcaice::Context& context ) :
    LocalTracer( hydroutil::Properties(
                    context.properties()->getPropertiesForPrefix("Orca.Tracer."),"Orca.Tracer."),
                 orcaice::toString(context.name()) ),
    interfaceName_("tracer"),
    context_(context)
{
    isInitialized_.set( false );

    // create the object
    ptr_ = new TracerI( *this );
}

TracerImpl::~TracerImpl()
{
//     orcaice::tryRemoveAdminInterface( context_, "Tracer" );
}

void
TracerImpl::initInterface()
{
    // the ONLY place where the topic handler is inited ( InitPolicyManual )
    // decide if we need IceStorm topics
    if ( config_.verbosity[gbxutilacfr::AnyTrace][gbxutilacfr::ToNetwork] ) {
        initTopicHandler();
    }

    // previous method: adding Home to the component adapter
//     orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );

    // EXPERIMENTAL! adding as a facet to the Admin interface.
    orcaice::createAdminInterface( context_, ptr_, context_.name() );
}

void
TracerImpl::initTopicHandler()
{
    // are we required to connect to status topic? (there's always default value for this property)
    bool isTopicRequired = context_.properties()->getPropertyAsInt( "Orca.Tracer.RequireIceStorm" );

    // fqTName is something like "tracer/*@platformName/componentName"
    orca::FQTopicName fqTName = orcaice::toTracerTopic( context_.name() );

    topicHandler_.reset( new TracerTopicHandler( orcaice::toString(fqTName), context_ ) );
    bool localReportingOnly = true;
    if ( !topicHandler_->connectToTopic( localReportingOnly ) )
    {
        const bool localReportingOnly = true;
        if ( isTopicRequired )
        {
            std::string s = prefix_+": Failed to connect to an IceStorm tracer topic '"+
                orcaice::toString(fqTName)+"'\n" +
                "\tYou may allow to proceed by setting Orca.Tracer.RequireIceStorm=0.";
            initTracerError( context_, s, localReportingOnly );
            // this should kill the app
            exit(1);
        }
        else
        {
            std::string s = prefix_+": Failed to connect to an IceStorm tracer topic\n";
            s += "\tAll trace messages will be local.\n";
            s += "\tYou may enforce connection by setting Orca.Tracer.RequireIceStorm=1.";
            initTracerWarning( context_, s, localReportingOnly );

            // turn off all outputs toNetwork
            for ( int i=0; i<gbxutilacfr::NumberOfTraceTypes; ++i ) {
                config_.verbosity[i][gbxutilacfr::ToNetwork] = 0;
            }
            // move on
        }

        // reset the pointer back to zero.
        // this is how we'll know not to use it.
        topicHandler_.reset( 0 );
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

IceStorm::TopicPrx
TracerImpl::internalSubscribe(const ::orca::TracerConsumerPrx& subscriber)
{
    if( !topicHandler_.get() )
    {
        throw orca::SubscriptionFailedException("Component does not have a topic to publish its traces.");
    }

    return topicHandler_->subscribe( subscriber );
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

    if( topicHandler_.get() )
    {
        topicHandler_->publish( data );
    }
}

}
}
