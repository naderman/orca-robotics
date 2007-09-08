/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAIFACEIMPL_CONSUMER_IMPL_H
#define ORCAIFACEIMPL_CONSUMER_IMPL_H

#include <orcaice/context.h>
#include <orcaice/multiconnectutils.h>
#include <orcaice/thread.h>

namespace orcaifaceimpl
{

// detail class.
// Functionality which ConsumerI needs to know about when talking to ConsumerImpl class.
template<class ObjectType>
class ConsumerDataHandler
{
public:
    virtual ~ConsumerDataHandler() {};
    virtual void internalSetData( const ObjectType &data )=0;
};

// generic consumerI.
// implements any consumer interface whose set method is called "setData".
// this is a "detail" class, not used directly by the end user.
template<class ConsumerType, class ObjectType>
class ConsumerI : public ConsumerType
{
public:
    ConsumerI( ConsumerDataHandler<ObjectType> &handler )
        : handler_(handler) {}

    virtual ~ConsumerI() {};
    
    // implementation of remote call defined in all *Consumer interfaces
    // this implementation redirects to the implementation class
    virtual void setData( const ObjectType& data, const Ice::Current& )
    {
        handler_.internalSetData( data );
    }

private:
    ConsumerDataHandler<ObjectType> &handler_;
};

//! Functionality which end users needs to know about when telling the ConsumerImpl class
//! class to subscribe to data provider. Because this class is not templated it is possible
//! to have a generic pointer to ConsumerImpl regardless of template parameters.
class ConsumerSubscriber
{
public:
    virtual ~ConsumerSubscriber() {};

    //! Tries to connect to remote interface with stringified proxy @c proxyString.
    //! If succesful, tries to subscribe for data using the internal consumer interface.
    //! Does not catch any exceptions.
    virtual void subscribeWithString( const std::string& proxyString )=0;

    //! Tries to connect to remote interface with stringified proxy @c proxyString.
    //! If succesful, tries to unsubscribe the internal consumer interface.
    //! Does not catch any exceptions.
    virtual void unsubscribeWithString( const std::string& proxyString )=0;

    //! Same as subscribeWithString() but the interface is looked up using the config file and tag interfaceTag.
    virtual void subscribeWithTag( const std::string& interfaceTag )=0;

    //! Same as unsubscribeWithString() but the interface is looked up using the config file and tag interfaceTag.
    virtual void unsubscribeWithTag( const std::string& interfaceTag )=0;

    //! Tries to connect to remote interface with stringified proxy @c proxyString.
    //! If succesful, tries to subscribe for data using the internal consumer interface.
    //! Catches appropriate exceptions. DOCUMENT!
    virtual void subscribeWithString( const std::string& proxyString, orcaice::Thread* thread, int retryInterval=2 )=0;

    //! Same as unsubscribeWithString() but the interface is looked up using the config file and tag interfaceTag.
    virtual void subscribeWithTag( const std::string& interfaceTag, orcaice::Thread* thread, int retryInterval=2 )=0;
};

/*!
 A generic consumer: instantiates and looks after a consumerI.
 By 'looks after' I mean: adds it to/removes it from the adapter.

 Derived classes need to implement the handleData() callback.
*/
//  Note: inheriting from IceUtil::Shared allows us to use Ice smart
//  pointers with these things.
template<class ProviderPrxType, class ConsumerType, class ConsumerPrxType, class ObjectType>
class ConsumerImpl : public ConsumerDataHandler<ObjectType>,
                     public ConsumerSubscriber,
                     public IceUtil::Shared
{
friend class ConsumerI<ConsumerType,ObjectType>;

public:
    //! Constructor
    ConsumerImpl( const orcaice::Context &context )
        : context_(context),
          ptr_(new ConsumerI<ConsumerType,ObjectType>(*this)),
          // this function does not throw
          prx_(orcaice::createConsumerInterface<ConsumerPrxType>(context,ptr_))
    {}

    virtual ~ConsumerImpl() 
    {
        try {
            context_.adapter()->remove( prx_->ice_getIdentity() );
        }
        // This can fail if the adapter is shutting down.  We don't care.
        catch ( ... ) {}
    }

    virtual void subscribeWithString( const std::string& proxyString )
    {
        ProviderPrxType providerPrx;
        orcaice::connectToInterfaceWithString<ProviderPrxType>( context_, providerPrx, proxyString );

        providerPrx->subscribe( prx_ );
    }

    virtual void subscribeWithTag( const std::string& interfaceTag )
    {
        // this may throw ConfigFileException, we don't catch it, let the user catch it at the component level
        std::string proxyString = orcaice::getRequiredInterfaceAsString( context_, interfaceTag );
    
        // now that we have the stingified proxy, use the function above.
        subscribeWithString( proxyString );
    }

    virtual void unsubscribeWithString( const std::string& proxyString )
    {
        ProviderPrxType providerPrx;
        orcaice::connectToInterfaceWithString<ProviderPrxType>( context_, providerPrx, proxyString );

        providerPrx->unsubscribe( prx_ );
    }

    virtual void unsubscribeWithTag( const std::string& interfaceTag )
    {
        // this may throw ConfigFileException, we don't catch it, let the user catch it at the component level
        std::string proxyString = orcaice::getRequiredInterfaceAsString( context_, interfaceTag );
    
        // now that we have the stingified proxy, use the function above.
        unsubscribeWithString( proxyString );
    }

    virtual void subscribeWithString( const std::string& proxyString, orcaice::Thread* thread, int retryInterval=2  )
    {
        ProviderPrxType providerPrx;
        // multi-try
        orcaice::connectToInterfaceWithString<ProviderPrxType>( context_, providerPrx, proxyString, thread, retryInterval );

        while ( !thread->isStopping() )
        {
            try {
                providerPrx->subscribe( prx_ );
                break;
            }
            catch ( const Ice::Exception &e )
            {
                std::stringstream ss;
                ss << "Failed to subscribe. "
                    <<"Will retry in "<<retryInterval<<"s."
                    <<e;
                context_.tracer()->warning( ss.str() );
            }
            catch ( const std::exception &e )
            {
                std::stringstream ss;
                ss << "Failed to subscribe. "
                    <<"Will retry in "<<retryInterval<<"s."
                    <<e.what();
                context_.tracer()->warning( ss.str() );
            }
            catch ( ... )
            {
                std::stringstream ss;
                ss << "Failed to subscribe for unknown reason. "
                    <<"Will retry in "<<retryInterval<<"s.";
                context_.tracer()->warning( ss.str() );
            }
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(retryInterval));
        }

    }

    virtual void subscribeWithTag( const std::string& interfaceTag, orcaice::Thread* thread, int retryInterval=2  )
    {
        // this may throw ConfigFileException, we don't catch it, let the user catch it at the component level
        std::string proxyString = orcaice::getRequiredInterfaceAsString( context_, interfaceTag );
    
        // now that we have the stingified proxy, use the function above.
        subscribeWithString( proxyString, thread, retryInterval );
    }

    //! Access to the proxy to the internal consumer interface implementation.
    ConsumerPrxType consumerPrx() const { return prx_; }

protected:
    //! Implement this call back in the derived class. 
    virtual void handleData( const ObjectType &data )=0;

    //! Component context.
    orcaice::Context context_;

private:
    // from ConsumerDataHandler
    virtual void internalSetData( const ObjectType &data )
    {
        // callback implemented in derived classes.
        handleData( data );
    }

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr   ptr_;
    // proxy to the internal consumer interface implementation
    ConsumerPrxType  prx_;
};

} // namespace

#endif
