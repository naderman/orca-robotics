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
#include <gbxsickacfr/gbxiceutilacfr/safethread.h>
#include <iostream>

namespace orcaifaceimpl
{

//! Functionality which end users needs to know about when telling the ConsumerImpl class
//! class to subscribe to data provider. Because this class is not templated it is possible
//! to have a generic pointer to ConsumerImpl regardless of template parameters.
class ConsumerSubscriber
{
public:
    ConsumerSubscriber( const orcaice::Context& context );

    virtual ~ConsumerSubscriber();

    //! Tries to connect to remote interface with stringified proxy @c proxyString.
    //! If succesful, tries to subscribe for data using the internal consumer interface.
    //! Does not catch any exceptions.
    virtual void subscribeWithString( const std::string& proxyString )=0;

    //! Tries to connect to remote interface with stringified proxy @c proxyString.
    //! If succesful, tries to unsubscribe the internal consumer interface.
    //! Does not catch any exceptions.
    virtual void unsubscribeWithString( const std::string& proxyString )=0;


    //! Tries to connect to remote interface with stringified proxy @c proxyString.
    //! Will try to connect @c retryNumber number of times (-1 means infinite), waiting for @c retryInterval [s] after
    //! each attempt.
    //! If succesful, tries to subscribe for data using the internal consumer interface.
    //! Catches appropriate exceptions. DOCUMENT!
    virtual void subscribeWithString( const std::string& proxyString, 
                          gbxsickacfr::gbxiceutilacfr::Thread*  thread, const std::string& subsysName="", 
                          int retryInterval=2, int retryNumber=-1 )=0;

    //! Same as subscribeWithString() but the interface is looked up using the config file and tag interfaceTag.
    void subscribeWithTag( const std::string& interfaceTag );

    //! Same as unsubscribeWithString() but the interface is looked up using the config file and tag interfaceTag.
    void unsubscribeWithTag( const std::string& interfaceTag );

    //! Same as the threaded version of subscribeWithString() but the interface is looked up 
    //! using the config file and tag interfaceTag.
    void subscribeWithTag( const std::string& interfaceTag, 
                          gbxsickacfr::gbxiceutilacfr::Thread*  thread, const std::string& subsysName="", 
                          int retryInterval=2, int retryNumber=-1 );

protected:

    //! Component context.
    orcaice::Context context_;
};

/*!
 A generic consumer: instantiates and looks after a consumerI, i.e. adds it to/removes it from the adapter.

 Derived classes need to implement the handleData() callback.
*/
//  Note: inheriting from IceUtil::Shared allows us to use Ice smart
//  pointers with these things.
template<class ProviderPrxType, class ConsumerType, class ConsumerPrxType, class ObjectType>
class ConsumerImpl : virtual public ConsumerType,
                           public ConsumerSubscriber
//                            public IceUtil::Shared
{
public:
    //! Constructor
    ConsumerImpl( const orcaice::Context &context ) :
        ConsumerSubscriber(context),
        isEnabled_(true)
    {
        Ice::ObjectPtr ptr = this;
//         this function does not throw
        consumerPrx_ = orcaice::createConsumerInterface<ConsumerPrxType>(context,ptr);

    }

    // no doxytags, these functions are already documented above.
    virtual ~ConsumerImpl() {};

    // implementation of remote call defined in all *Consumer interfaces
    // this implementation redirects to the implementation class
    virtual void setData( const ObjectType& data, const Ice::Current& )
    {
// std::cout<<"ConsumerImpl::setData() start"<<std::endl;
        IceUtil::Mutex::Lock lock(enableMutex_);

//         if ( isEnabled_ ) {
            dataEvent( data );
//             std::cout<<"ConsumerImpl::setData() end"<<std::endl;
//             return;
//         }
// std::cout<<"ConsumerImpl::setData() end -- DISABLED"<<std::endl;
    }

    //! Implement this callback in the derived class. 
    virtual void dataEvent( const ObjectType& data )=0;

    virtual void subscribeWithString( const std::string& proxyString )
    {
        ProviderPrxType providerPrx;
        orcaice::connectToInterfaceWithString<ProviderPrxType>( context_, providerPrx, proxyString );

        providerPrx->subscribe( consumerPrx_ );
        std::stringstream ss;
        ss << "Subscribed to " << proxyString;
        context_.tracer().debug( ss.str() );
    }

    virtual void unsubscribeWithString( const std::string& proxyString )
    {
        ProviderPrxType providerPrx;
        orcaice::connectToInterfaceWithString<ProviderPrxType>( context_, providerPrx, proxyString );

        providerPrx->unsubscribe( consumerPrx_ );
        std::stringstream ss;
        ss << "unsubscribed to " << proxyString;
        context_.tracer().debug( ss.str() );
    }

    virtual void subscribeWithString( const std::string& proxyString, 
                          gbxsickacfr::gbxiceutilacfr::Thread*  thread, const std::string& subsysName="", 
                          int retryInterval=2, int retryNumber=-1 )
    {
        ProviderPrxType providerPrx;
        // multi-try
        orcaice::connectToInterfaceWithString<ProviderPrxType>( 
                context_, providerPrx, proxyString, thread, subsysName, retryInterval, retryNumber );

        int count = 0;
        while ( !thread->isStopping() && ( retryNumber<0 || count<retryNumber) )
        {
            try {
                providerPrx->subscribe( consumerPrx_ );
                std::stringstream ss;
                ss << "Subscribed to " << proxyString;
                context_.tracer().debug( ss.str() );
                break;
            }
            catch ( const Ice::Exception &e )
            {
                std::stringstream ss;
                ss << "Failed to subscribe: " << e << std::endl
                   <<"Will retry in "<<retryInterval<<"s.";
                context_.tracer().warning( ss.str() );
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
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(retryInterval));
        }

    }

    //! Access the proxy to the internal consumer interface implementation.
    ConsumerPrxType consumerPrx() const { return consumerPrx_; }

    //! Disable data forwarding
    void destroy()
    {
        IceUtil::Mutex::Lock lock(enableMutex_);
        isEnabled_ = false;

        if ( !consumerPrx_ )
            return;

        try {
            context_.adapter()->remove( consumerPrx_->ice_getIdentity() );
std::cout<<"ConsumerImpl::destroy() id="<<consumerPrx_->ice_toString()<<std::endl;
            
        }
        // This can fail if the adapter is shutting down.  We don't care.
        catch ( ... ) {
// std::cout<<"ConsumerImpl::disable() failed to unsubscribe."<<std::endl;
        }
    }

private:
    // proxy to the internal consumer interface implementation
    ConsumerPrxType  consumerPrx_;

    IceUtil::Mutex enableMutex_;
    bool isEnabled_;
};

} // namespace

#endif
