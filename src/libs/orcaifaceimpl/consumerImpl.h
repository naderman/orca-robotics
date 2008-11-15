/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAIFACEIMPL_CONSUMER_IMPL_H
#define ORCAIFACEIMPL_CONSUMER_IMPL_H

#include <orcaice/context.h>
#include <orcaice/multiconnectutils.h>
#include <orcaice/iceutils.h>
#include <orcaice/icestormutils.h>
#include <orcaice/icegridutils.h>
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <gbxsickacfr/gbxiceutilacfr/thread.h>

namespace orcaifaceimpl
{

//! Functionality which end users needs to know about when telling the ConsumerImpl class
//! class to subscribe to data provider. Because this class is not templated it is possible
//! to have a generic pointer to ConsumerImpl regardless of template parameters.
class ConsumerSubscriber
{
public:
    ConsumerSubscriber( const orcaice::Context& context );

    virtual ~ConsumerSubscriber() {};

    //! Tries to connect to remote interface with stringified proxy @c proxyString.
    //! If succesful, tries to subscribe for data using the internal consumer interface.
    //! Does not catch any exceptions.
    virtual void subscribeWithString( const std::string& proxyString )=0;

    //! Same as subscribeWithString() but the interface is looked up using the config file and tag interfaceTag.
    void subscribeWithTag( const std::string& interfaceTag );

    //! When previously subscribed, tries to connect to the internally-stored topic proxy and unsubscribes.
    //! When not currently subscribed, quietly returns.
    virtual void unsubscribe()=0;

    //! Tries to connect to remote interface with stringified proxy @c proxyString.
    //! Will try to connect @c retryNumber number of times (-1 means infinite), waiting for @c retryInterval [s] after
    //! each attempt.
    //! If succesful, tries to subscribe for data using the internal consumer interface.
    //! Catches appropriate exceptions. DOCUMENT!
    //! 
    //! Returns TRUE if subscribed succesfully, FALSE otherwise.
    virtual bool subscribeWithString( const std::string& proxyString,  
                          gbxiceutilacfr::Thread*  thread, const std::string& subsysName="", 
                          int retryInterval=2, int retryNumber=-1 )=0;

    //! Same as the threaded version of subscribeWithString() but the interface is looked up 
    //! using the config file and tag interfaceTag.
    //! 
    //! Returns TRUE if subscribed succesfully, FALSE otherwise.
    bool subscribeWithTag( const std::string& interfaceTag, 
                          gbxiceutilacfr::Thread*  thread, const std::string& subsysName="", 
                          int retryInterval=2, int retryNumber=-1 );
protected:

    // This could be used for re-subscribing.
//     gbxiceutilacfr::Store<std::string> proxyString_;

    //! EXPERIMENTAL!
    gbxiceutilacfr::Store<IceStorm::TopicPrx> topic_;

    //! Component context.
    orcaice::Context context_;
};

// have to define it as a separate abstract class
// because ConsumerTypeI only needs to know about dataEvent()
// (and does not know about the 2 extra template parameters in
// ConsumerImpl)
template<class ObjectType>
class AbstractConsumer
{
public:
    // Implement this callback in the derived class. 
    virtual void dataEvent( const ObjectType& data )=0;
};

// implements Slice consumer interface
// the end user does not need to know about it.
// redirects incoming data to ConsumerImpl's derivatives.
template<class ConsumerType, class ObjectType>
class ConsumerTypeI : virtual public ConsumerType
{
public:
    ConsumerTypeI( AbstractConsumer<ObjectType> &impl ) :
        impl_(impl) {}
    virtual ~ConsumerTypeI() {};

    // implementation of remote call defined in all *Consumer interfaces
    // this implementation redirects to the Impl class
    virtual void setData( const ObjectType& data, const Ice::Current& )
    {
//         context_.tracer().debug( "Received data from provider", 8 );
        impl_.dataEvent( data );
    }

private:
    AbstractConsumer<ObjectType>& impl_;
};

/*!
A generic consumer: instantiates and looks after a consumerI, i.e. adds it to/removes it from the adapter. 

Derived classes need to implement the dataEvent() callback function which is called when the new data arrives.

This consumer subscribes for data updates in the constructor and unsubscribes in the
destructor. There's still a small chance that IceStorm will report an error when data delivery and
unsubscription are closely spaced, e.g. ('ast' in 'status/ast' means asterisk, replaced to avoid compiler warning):
@verbatim
<<<<<<< .mine
Oct 25 03:26:47 tango /usr/bin/icebox[2474]: Topic: status\*@tango/localnav: subscribeAndGetPublisher: 07394FBF-586C-4128-AA28-1727B9DA2E19 QoS:  subscriptions: []
Oct 25 03:26:49 tango /usr/bin/icebox[2474]: Topic: status\*@tango/localnav: unsubscribe: 07394FBF-586C-4128-AA28-1727B9DA2E19[07394FBF-586C-4128-AA28-1727B9DA2E19]
=======
Oct 25 03:26:47 tango /usr/bin/icebox[2474]: Topic: status/ast@tango/localnav: subscribeAndGetPublisher: 07394FBF-586C-4128-AA28-1727B9DA2E19 QoS:  subscriptions: []
Oct 25 03:26:49 tango /usr/bin/icebox[2474]: Topic: status/ast@tango/localnav: unsubscribe: 07394FBF-586C-4128-AA28-1727B9DA2E19[07394FBF-586C-4128-AA28-1727B9DA2E19]
>>>>>>> .r5129
Oct 25 03:26:49 tango /usr/bin/icebox[2474]: Subscriber: 0x81182e0 07394FBF-586C-4128-AA28-1727B9DA2E19: subscriber errored out: OutgoingAsync.cpp:305: Ice::ObjectNotExistException: object does not exist: identity: `07394FBF-586C-4128-AA28-1727B9DA2E19' facet:  operation: setData retry: 0/0
@endverbatim
*/
template<class ProviderType, class ProviderPrxType, class ConsumerType, class ConsumerPrxType, class ObjectType>
class ConsumerImpl : public ConsumerSubscriber, 
                     public AbstractConsumer<ObjectType>,
                     public IceUtil::Shared
{

protected:
    // these are protected so that it's possible to re-implement initConsumer()

    //! Proxy to the internal consumer interface implementation
    ConsumerPrxType consumerPrx_;

    //! Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr consumerPtr_;

public:
    //! Constructor creates consumer interface object. Does not contain any remote calls.
    ConsumerImpl( const orcaice::Context &context ) :
        ConsumerSubscriber(context)
    {
        consumerPtr_ = new ConsumerTypeI<ConsumerType,ObjectType>( *this );
        // this function does not throw, because it never talks to the Registry
        consumerPrx_ = orcaice::createConsumerInterface<ConsumerPrxType>(context_,consumerPtr_);
    }

    virtual ~ConsumerImpl() 
    {
//         context_.tracer().debug( "ConsumerImpl::~ConsumerImpl()" );
        // unsubscribe from the info provider
        try {
            unsubscribe();
        }
        catch ( const std::exception& e ) {
            std::stringstream ss;
            ss << "failed to unsubscribe in destructor: " << e.what();
            context_.tracer().warning( ss.str() );
        }
        catch ( ... ) {
            context_.tracer().warning( "failed to unsubscribe in destructor." );
        }

        // now destroy our consumer object
        if ( !consumerPrx_ )
            return;

        orcaice::tryRemoveInterfaceWithIdentity( context_, consumerPrx_->ice_getIdentity() );
    }

    //! Access the proxy to the internal consumer interface implementation.
    ConsumerPrxType consumerPrx() const { return consumerPrx_; }

    // This is tricky! Can't leave it pure virtual because we unsubscribe and detsroy
    // in ConsumerImpl destructor. By that time, the derived class (e.g. StoringConsumer)
    // is already destroyed and we'll get "pure virtual method called".
    //! This function is called when new data arrives to the consumer.
    //! Default implementation does nothing. 
    //! Re-implement this callback in the derived class. 
    virtual void dataEvent( const ObjectType& data )
    {
    }

    // no doxytags, these functions are already documented above.

    virtual void subscribeWithString( const std::string& proxyString )
    {
        ProviderPrxType providerPrx;

        std::string staticId = ConsumerType::ice_staticId();
        // Home does not have a consumer
        if ( staticId == "::orca::StatusConsumer" || staticId == "::orca::TracerConsumer" ) {
            orca::FQInterfaceName fqIfaceName = orcaice::toInterfaceName( proxyString );
            orca::FQComponentName fqCompName;
            fqCompName.platform = fqIfaceName.platform;
            fqCompName.component = fqIfaceName.component;
            orcaice::connectToAdminInterface<ProviderType,ProviderPrxType>( context_, providerPrx, fqCompName );
        }
        else {
            orcaice::connectToInterfaceWithString( context_, providerPrx, proxyString );
        }

        IceStorm::TopicPrx topicPrx = providerPrx->subscribe( consumerPrx_ );
        topic_.set( topicPrx );

        std::stringstream ss;
        ss << "Subscribed to " << proxyString;
        context_.tracer().debug( ss.str() );
    }

    virtual void unsubscribe()
    {
        if ( !topic_.isEmpty() )
        {
            IceStorm::TopicPrx topicPrx;
            topic_.get( topicPrx );

            topicPrx->unsubscribe( consumerPrx_ );
            std::stringstream ss;
            ss << "Unsubscribed from " << topicPrx->ice_toString();
            context_.tracer().debug( ss.str() );
        }
    }

    virtual bool subscribeWithString( const std::string& proxyString, 
                          gbxiceutilacfr::Thread*  thread, const std::string& subsysName="", 
                          int retryInterval=2, int retryNumber=-1 )
    {
        ProviderPrxType providerPrx;
        // multi-try
        orcaice::connectToInterfaceWithString( context_,
                                               providerPrx,
                                               proxyString,
                                               thread,
                                               subsysName,
                                               retryInterval,
                                               retryNumber );

        int count = 0;
        while ( !thread->isStopping() && ( retryNumber<0 || count<retryNumber) )
        {
            try {
                IceStorm::TopicPrx topicPrx = providerPrx->subscribe( consumerPrx_ );
                topic_.set( topicPrx );

                std::stringstream ss;
                ss << "Subscribed to " << proxyString;
                context_.tracer().debug( ss.str() );
                return true;
            }
            catch ( const std::exception &e )
            {
                std::stringstream ss;
                ss << "Failed to subscribe: " << e.what() << std::endl
                   <<"Will retry in "<<retryInterval<<"s.";
                context_.tracer().warning( ss.str() );
            }
            catch ( ... )
            {
                std::stringstream ss;
                ss << "Failed to subscribe for unknown reason. "
                   <<"Will retry in "<<retryInterval<<"s.";
                context_.tracer().warning( ss.str() );
            }
            ++count;
            if ( !subsysName.empty() )
                context_.status().heartbeat( subsysName );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(retryInterval));
            if ( !subsysName.empty() )
                context_.status().heartbeat( subsysName );
        }

        return false;
    }
};

} // namespace

#endif
