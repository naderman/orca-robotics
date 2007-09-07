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
#include <orcaice/connectutils.h>
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

/*!
 A generic consumer: instantiates and looks after a consumerI.
 By 'looks after' I mean: adds it to/removes it from the adapter.

 Derived classes need to implement the handleData() callback.
*/
template<class ProviderPrxType, class ConsumerType, class ConsumerPrxType, class ObjectType>
class ConsumerImpl : public ConsumerDataHandler<ObjectType>
{
friend class ConsumerI<ConsumerType,ObjectType>;

public:
    //! Constructor
    ConsumerImpl( const orcaice::Context &context )
        : ptr_(new ConsumerI<ConsumerType,ObjectType>(*this)),
          // this function does not throw
          prx_(orcaice::createConsumerInterface<ConsumerPrxType>(context,ptr_)),
          context_(context)
    {}

    virtual ~ConsumerImpl() 
    {
        try {
            context_.adapter()->remove( prx_->ice_getIdentity() );
        }
        // This can fail if the adapter is shutting down.  We don't care.
        catch ( ... ) {}
    }

    //! Tries to connect to remote interface using interface tag and component config file.
    //! If succesful, tries to subscribe for data using the internal consumer interface.
    //! Does not catch any exceptions.
    void subscribe( const std::string& interfaceTag )
    {
        ProviderPrxType providerPrx;
        orcaice::connectToInterfaceWithTag<ProviderPrxType>( context_, providerPrx, interfaceTag );

        providerPrx->subscribe( prx_ );
    }

    //! Tries to connect to remote interface using interface tag and component config file.
    //! If succesful, tries to unsubscribe the internal consumer interface.
    //! Does not catch any exceptions.
    void unsubscribe( const std::string& interfaceTag )
    {
        ProviderPrxType providerPrx;
        orcaice::connectToInterfaceWithTag<ProviderPrxType>( context_, providerPrx, interfaceTag );

        providerPrx->unsubscribe( prx_ );
    }

    //! Tries to connect to remote interface using interface tag and component config file.
    //! If succesful, tries to subscribe for data using the internal consumer interface.
    //! Does not catch any exceptions.
    void subscribe( const std::string& interfaceTag, orcaice::Thread* thread, int retryInterval=2  )
    {
        ProviderPrxType providerPrx;
        // multi-try
        orcaice::connectToInterfaceWithTag<ProviderPrxType>( context_, providerPrx, interfaceTag, thread, retryInterval );

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
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(retryInterval));
        }

    }

    //! Access to the proxy to the internal consumer interface implementation.
    ConsumerPrxType consumerPrx() const { return prx_; }

protected:
    //! Implement this call back in the derived class. 
    virtual void handleData( const ObjectType &data )=0;

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
    orcaice::Context context_;
};

} // namespace

#endif
