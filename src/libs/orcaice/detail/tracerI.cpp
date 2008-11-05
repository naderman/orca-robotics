/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <IceUtil/Time.h>
#include <orcaice/orcaice.h>

#include "tracerI.h"

using namespace std;
using namespace orcaice::detail;

TracerI::TracerI( const orcaice::Context& context ) :
    LocalTracer( hydroutil::Properties( 
                    context.properties()->getPropertiesForPrefix("Orca.Tracer."),"Orca.Tracer."),
                 orcaice::toString(context.name()) ),
    componentTraceSender_(NULL),
    platformInfoSender_(NULL),
    platformWarningSender_(NULL),
    platformErrorSender_(NULL),
    context_(context)
{
    // do we need IceStorm topics?
    if ( config_.verbosity[AnyTrace][ToNetwork] ) {
        setupAndConnectNetworkSenders();
    }
}

TracerI::~TracerI()
{
    if ( componentTraceSender_ )  delete componentTraceSender_;
    if ( platformInfoSender_ )    delete platformInfoSender_;
    if ( platformWarningSender_ ) delete platformWarningSender_;
    if ( platformErrorSender_ )   delete platformErrorSender_;
}

void
TracerI::trySetupNetworkTraceSender( NetworkTraceSender *&sender,
                                     std::string topic,
                                     bool isTracerTopicRequired )
{
    if ( sender == NULL )
    {
        sender = new NetworkTraceSender( topic,
                                         mutex_,
                                         context_ );
        if ( !sender->connectToIceStorm() )
        {
            icestormConnectFailed( topic, isTracerTopicRequired );
            delete sender; sender = NULL;
        }
    }
}

void
TracerI::setupAndConnectNetworkSenders()
{
    // get properties for our component
    Ice::PropertiesPtr props = context_.properties();

    // are we required to connect to status topic? (there's always default value for this property)
    bool isTracerTopicRequired = props->getPropertyAsInt( "Orca.Tracer.RequireIceStorm" );

    // fqTName is something like "tracer/*@platformName/componentName"
    orca::FQTopicName fqTName = orcaice::toTracerTopic( context_.name() );
    trySetupNetworkTraceSender( componentTraceSender_,
                                orcaice::toString(fqTName),
                                isTracerTopicRequired );

    // alexm: don't try the rest if the first one failed
    bool isIceStormReachable = ( componentTraceSender_ != 0 );
    if ( !isIceStormReachable ) {
        initTracerInfo( "Tracer: failed to connect to the first topic, skipping the rest" );
        return;
    }

    std::string infoTopic = std::string("*/info@")+fqTName.platform+std::string("/*");
    trySetupNetworkTraceSender( platformInfoSender_,
                                infoTopic,
                                isTracerTopicRequired );

    std::string warningTopic = std::string("*/warning@")+fqTName.platform+std::string("/*");
    trySetupNetworkTraceSender( platformWarningSender_,
                                warningTopic,
                                isTracerTopicRequired );

    std::string errorTopic = std::string("*/error@")+fqTName.platform+std::string("/*");
    trySetupNetworkTraceSender( platformErrorSender_,
                                errorTopic,
                                isTracerTopicRequired );
}

void
TracerI::icestormConnectFailed( const std::string &topicName,
                                bool isTracerTopicRequired )
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
        for ( int i=0; i<NumberOfTraceTypes; ++i ) {
            config_.verbosity[i][ToNetwork] = 0;
        }
        // move on
    }
}

::orca::TracerVerbosityConfig 
TracerI::getVerbosity( const ::Ice::Current& )
{
    ::orca::TracerVerbosityConfig config;

    config.info = config_.verbosity[TracerI::InfoTrace][TracerI::ToNetwork];
    config.warning = config_.verbosity[TracerI::WarningTrace][TracerI::ToNetwork];
    config.error = config_.verbosity[TracerI::ErrorTrace][TracerI::ToNetwork];
    config.debug = config_.verbosity[TracerI::DebugTrace][TracerI::ToNetwork];

    return config;
}

void 
TracerI::setVerbosity( const ::orca::TracerVerbosityConfig& config,  const ::Ice::Current& )
{
    if ( config.info > -1 ) {
        config_.verbosity[TracerI::InfoTrace][TracerI::ToNetwork]     = config.info;
    }
    if ( config.warning > -1 ) {
        config_.verbosity[TracerI::WarningTrace][TracerI::ToNetwork]  = config.warning;
    }
    if ( config.error > -1 ) {
        config_.verbosity[TracerI::ErrorTrace][TracerI::ToNetwork]    = config.error;
    }
    if ( config.debug > -1 ) {
        config_.verbosity[TracerI::DebugTrace][TracerI::ToNetwork]    = config.debug;
    }

    stringstream ss;
    ss << "New ToNetwork verbosity levels:"
        << " info="<<config_.verbosity[TracerI::InfoTrace][TracerI::ToNetwork]
        << " warn="<<config_.verbosity[TracerI::WarningTrace][TracerI::ToNetwork]
        << " error="<<config_.verbosity[TracerI::ErrorTrace][TracerI::ToNetwork]
        << " debug="<<config_.verbosity[TracerI::DebugTrace][TracerI::ToNetwork];
    // do not call info because it may go back to the network and lock up
    // do it manually
    if ( config_.verbosity[InfoTrace][ToDisplay] ) {
        toDisplay( "info", ss.str(), 1 );
    }
    if ( config_.verbosity[InfoTrace][ToFile] ) {
        toFile( "info", ss.str(), 1 );
    }   
    if ( config_.verbosity[InfoTrace][ToLog] ) {
        toLog( "info", ss.str(), 1 );
    }  

    // pre-calculate marginals: accross trace types
    recalcMarginals();
}

void
TracerI::subscribe( NetworkTraceSender *&sender, const ::orca::TracerConsumerPrx& subscriber )
{
    if ( !sender ) 
    {
        setupAndConnectNetworkSenders();
        if ( !sender )
        {
            throw orca::SubscriptionFailedException("Component does not have a topic to publish its traces.");
        }
    }
    
    sender->subscribe( subscriber );
}

void
TracerI::unsubscribe( NetworkTraceSender *sender, const ::orca::TracerConsumerPrx& subscriber )
{
    if ( sender )
        sender->unsubscribe( subscriber );
}

void
TracerI::info( const std::string &message, int level )
{
    LocalTracer::info( message, level );

    if ( config_.verbosity[InfoTrace][ToNetwork] >= level ) {
		toNetwork( gbxutilacfr::Tracer::InfoTrace, message, level );
    }
}

void
TracerI::warning( const std::string &message, int level )
{
    LocalTracer::warning( message, level );

    if ( config_.verbosity[WarningTrace][ToNetwork] >= level ) {
        toNetwork( gbxutilacfr::Tracer::WarningTrace, message, level );
    }
}
    
void
TracerI::error( const std::string &message, int level )
{
    LocalTracer::error( message, level );

    if ( config_.verbosity[ErrorTrace][ToNetwork] >= level ) {
        toNetwork( gbxutilacfr::Tracer::ErrorTrace, message, level );
    }
}

void
TracerI::debug( const std::string &message, int level )
{
    LocalTracer::debug( message, level );

    if ( config_.verbosity[DebugTrace][ToNetwork] >= level ) {
        toNetwork( gbxutilacfr::Tracer::DebugTrace, message, level );
    }
}

std::string 
TracerI::categoryToString( gbxutilacfr::Tracer::TraceType category )
{
    if ( category == gbxutilacfr::Tracer::InfoTrace )
        return "info";
    else if ( category == gbxutilacfr::Tracer::WarningTrace )
        return "warning";
    else if ( category == gbxutilacfr::Tracer::ErrorTrace )
        return "error";
    else if ( category == gbxutilacfr::Tracer::DebugTrace )
        return "debug";
    else
        return "other";
}

void
TracerI::toNetwork( gbxutilacfr::Tracer::TraceType traceType,
                    const std::string& message,
                    int level )
{
    orca::TracerData tracerData;
    orcaice::setToNow( tracerData.timeStamp );
    tracerData.name = context_.name();
    tracerData.category = categoryToString( traceType );
    tracerData.verbosity = level;
    tracerData.message = message;

    assert( componentTraceSender_ != NULL );
    componentTraceSender_->sendToNetwork( tracerData );

    if ( traceType == gbxutilacfr::Tracer::InfoTrace )
    {
        assert( platformInfoSender_ != NULL );
        platformInfoSender_->sendToNetwork( tracerData );
    }
    else if ( traceType == gbxutilacfr::Tracer::WarningTrace )
    {
        assert( platformWarningSender_ != NULL );
        platformWarningSender_->sendToNetwork( tracerData );
    }
    else if ( traceType == gbxutilacfr::Tracer::ErrorTrace )
    {
        assert( platformErrorSender_ != NULL );
        platformErrorSender_->sendToNetwork( tracerData );
    }
}
