/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_TOPIC_HANDLER_H
#define ORCAICE_TOPIC_HANDLER_H

#include <IceStorm/IceStorm.h>
#include <orca/exceptions.h>
#include <orcaice/context.h>
#include <orcaice/icestormutils.h>
#include <orcaice/multiicestormutils.h>

namespace gbxiceutilacfr { class Thread; }

namespace orcaice {

// this is a utility class which wraps up the functionality
// of interacting with IceStorm Topic.
template<class ConsumerProxyType, class DataType>
class TopicHandler
{

private: 

    ConsumerProxyType publisherPrx_;
    IceStorm::TopicPrx topicPrx_;

    std::string topicName_;

    orcaice::Context context_;

    // utility function: subscribes and returns individual publisher
    // (the pointer may be empty if the subscriber is already subscribed)
    ConsumerProxyType internalSubscribe( const ConsumerProxyType& subscriber, 
                                         const IceStorm::QoS& qos )
    {    
        if ( !topicPrx_ )
            throw orca::SubscriptionFailedException( "Not connected to topic yet" );
    
        context_.tracer().debug( "TopicHandler::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
        // see Ice Manual sec.45.6 "Publishing to a specific subscriber"
        ConsumerProxyType individualPublisher;
        try {
            // this talks to IceStorm
            Ice::ObjectPrx pub = topicPrx_->subscribeAndGetPublisher( qos, subscriber->ice_twoway());
            individualPublisher = ConsumerProxyType::uncheckedCast(pub);
        }
        catch ( const IceStorm::AlreadySubscribed& e ) {
            std::stringstream ss;
            ss <<"Request for subscribe but this proxy has already been subscribed, so I do nothing: "<< e.what();
            context_.tracer().debug( ss.str(), 2 );
            // will return an empty poiinter!
        }
        catch ( const Ice::Exception& e ) {
            std::stringstream ss;
            ss <<"TopicHandler::internalSubscribe: failed to subscribe: "<< e.what();
            context_.tracer().warning( ss.str() );
            // throws exception back to the subscriber
            throw orca::SubscriptionFailedException( ss.str() );
        }
        // this pointer may be empty
        return individualPublisher;
    }

public: 

    TopicHandler( const std::string &topicName, const orcaice::Context &context ) :
        topicName_(topicName),
        context_(context)
    {
    }

    IceStorm::TopicPrx topic()
    {
        return topicPrx_;
    }

    //! Access the proxy to the internal consumer interface implementation.
    ConsumerProxyType publisherPrx() const 
    { 
        return publisherPrx_; 
    }

    // Returns TRUE on success, FALSE otherwise.
    // Catches only the exceptions expected in the case when IceStorm is not unavailable.
    //
    // Set localReportingOnly to true when calling this function from classes which deal with
    // error reporting, such as Tracer and Status. Otherwise, potential connection problems will
    // lead to infinite multiplication of fault messages.
    bool connectToTopic( bool localReportingOnly=false )
    {
        context_.tracer().debug( std::string("TopicHandler: connecting to topic ")+topicName_, 2 );
        // Find IceStorm Topic to which we'll publish
        try
        {
            topicPrx_ = orcaice::connectToTopicWithString<ConsumerProxyType>
                ( context_, publisherPrx_, topicName_, localReportingOnly );
        }
        // we only catch the exception which would be thrown if IceStorm is not there.
        catch ( const orcaice::NetworkException& e )
        {
            return false;
        }

        // fix the order of endpoints
//         topicPrx_ = topicPrx_->ice_endpointSelection( Ice::Ordered );
        return true;
    }

    bool connectToTopic( gbxutilacfr::Stoppable* activity, const std::string& subsysName, int retryInterval,
                         bool localReportingOnly=false )
    {
        context_.tracer().debug( subsysName, std::string("TopicHandler: connecting to topic ")+topicName_, 2 );
        // Find IceStorm Topic to which we'll publish
        try
        {
            topicPrx_ = orcaice::connectToTopicWithString<ConsumerProxyType>
                ( context_, publisherPrx_, topicName_, activity, subsysName, retryInterval, -1, localReportingOnly );
        }
        // we only catch the exception which would be thrown if IceStorm is not there.
        catch ( const orcaice::NetworkException& e )
        {
            return false;
        }

        // fix the order of endpoints
//         topicPrx_ = topicPrx_->ice_endpointSelection( Ice::Ordered );
        return true;
    }

    // sub subscribers to the topic we're publishing to
    IceStorm::TopicPrx subscribe( const ConsumerProxyType& subscriber, 
                                  const IceStorm::QoS& qos=IceStorm::QoS() )
    {
        internalSubscribe( subscriber, qos );
        return topicPrx_;
    }

    IceStorm::TopicPrx subscribe( const ConsumerProxyType& subscriber, 
                                   const DataType& initData, 
                                   const IceStorm::QoS& qos=IceStorm::QoS() )
    {    
        ConsumerProxyType individualPublisher = internalSubscribe( subscriber, qos );
    
        // the individualPublisher may be NULL.
        // this normally happens when the subscriber is already subscribed.
        // one example when this happens routinely: when logplayer is paused the subscriber
        // freaks out and tries to resubscribe but, as far as logplayer is concerned, it is
        // already subscribed.
        if ( individualPublisher ) 
        {
            // send all the information we have to the new subscriber (and to no one else)
            try
            {
                // this talks to IceStorm who will forward this data to this subscriber only.
                individualPublisher->setData( initData );   
            }
            catch ( const Ice::Exception&  e ) {
                std::stringstream ss;
                ss <<"TopicHandler::subscribe: failed to send information to the new subscriber: "<< e.what();
                // show this warning locally
                context_.tracer().warning( ss.str() );
                // throws exception back to the subscriber
                throw orca::SubscriptionPushFailedException( ss.str() );
            }
            context_.tracer().info( std::string("TopicHandler::subscribe(): sent status info to new subscriber: ")+individualPublisher->ice_toString() );
        }

        return topicPrx_;
    }

    void unsubscribe( const ConsumerProxyType& subscriber )
    {
        if ( !topicPrx_ )
            return;
    
        context_.tracer().debug( "TopicHandler::internalUnsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
        topicPrx_->unsubscribe( subscriber );
    }

    void publish( const DataType& data )
    {    
        // Try to push to IceStorm.
        orcaice::tryPushToIceStormWithReconnect<ConsumerProxyType,DataType> ( 
            context_,
            publisherPrx_,
            data,
            topicPrx_,
            topicName_ );
    };
};

}

#endif
