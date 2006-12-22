/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAICE_AUTO_CONSUMER_I_H
#define ORCAICE_AUTO_CONSUMER_I_H

// debug only
#include <iostream>

#include <orcaice/context.h>
#include <orcaice/timer.h>
#include <orcaice/recursivegaussian.h>
#include <orcaice/connectutils.h>

namespace orcaice
{

/*
 *  @brief A convenience class for consumers with setData() operation.
 *
 *  When the data arrives it does two thing:
 *      1. puts it into internal storage
 *      2. updates internal Gaussian model of update frequency.
 *
 *  Relies on the fact that the Consumer objects have only one operation
 *  to implement and it's called setData().
 *
 */
template<class InterfacePrxType, class ConsumerPrxType, class ConsumerType, class ObjectPtrType>
class AutoConsumerI : public ConsumerType
{
public:
    AutoConsumerI( const orcaice::Context & context, const std::string & tag );
    
    virtual ~AutoConsumerI() {};

    // this an implementation of the Ice remote call
    virtual void setData( const ObjectPtrType& data, const Ice::Current& )
        { addData( data ); }

    //! this is called by setData, or you can call it yourself
    void addData( const ObjectPtrType& data );

    //! Returns TRUE if the last update happened more than @p std standard deviations
    //! ago (according to the internal model).
    bool isStale( double std=3.0 ) const;

    //! Call this function periodically to check whether we are still receiving data.
    //! If the time since last update is longer than expected (according to the internal
    //! model), a sinalge attempt is made to re-subscribe.
    void checkStatus();

protected:

    virtual void storeData( const ObjectPtrType & obj )=0;

    //! You can derive from this class and do something when an object is received.
    //! (besides sticking it into the proxy). This adds the functionality of orcaice::PtrNotify.
    //! The default implementation does nothing.
    virtual void handleData( const ObjectPtrType & obj ) {};
    
    void subscribe();

private:
    orcaice::Context context_;
    std::string tag_;
    
    ConsumerPrxType callbackPrx_;

    orcaice::Timer timer_;
    RecursiveGaussian model_;
};

template<class InterfacePrxType, class ConsumerPrxType, class ConsumerType, class ObjectPtrType>
AutoConsumerI<InterfacePrxType,ConsumerPrxType,ConsumerType,ObjectPtrType>::AutoConsumerI
                        ( const orcaice::Context & context, const std::string & tag )
    : context_(context),
      tag_(tag)
{
    model_.reset( 0.0, 0.1, 100 );

    // create a callback object to recieve data
    Ice::ObjectPtr consumer = this;
    callbackPrx_ = orcaice::createConsumerInterface<ConsumerPrxType>( context_, consumer );
}

template<class InterfacePrxType, class ConsumerPrxType, class ConsumerType, class ObjectPtrType>
void
AutoConsumerI<InterfacePrxType,ConsumerPrxType,ConsumerType,ObjectPtrType>::addData
                        ( const ObjectPtrType& data )
{
    // debug
    std::cout<<"sample          : "<<timer_.elapsedMs()<<std::endl;
    
    // estimate time interval
    model_.addSample( timer_.elapsedMs() );
    timer_.restart();

    // stick data into the internal storage. Implementation depends on what storage we have.
    storeData( data );

    // perform optional data handling implemented in derived classes
    handleData( data );
}

template<class InterfacePrxType, class ConsumerPrxType, class ConsumerType, class ObjectPtrType>
void
AutoConsumerI<InterfacePrxType,ConsumerPrxType,ConsumerType,ObjectPtrType>::subscribe()
{
    InterfacePrxType interfacePrx;
    try{
        orcaice::connectToInterfaceWithTag<InterfacePrxType>( context_, interfacePrx, tag_ );
        context_.tracer()->debug("connected to a '"+tag_+"' interface",5);
    }
    catch( const orcaice::NetworkException& e )
    {
        std::stringstream ss;
        ss << "failed to connect to remote interface for tag "<<tag_<<": "<<e;
        context_.tracer()->error( ss.str() );
        return;
    }

    //
    // Subscribe for data
    //
    // will try forever until the user quits with ctrl-c
    try
    {
        interfacePrx->subscribe( callbackPrx_ );
    }
    catch ( const orca::SubscriptionFailedException & e )
    {
        context_.tracer()->error( "failed to subscribe for data updates." );
    }
}

template<class InterfacePrxType, class ConsumerPrxType, class ConsumerType, class ObjectPtrType>
bool
AutoConsumerI<InterfacePrxType,ConsumerPrxType,ConsumerType,ObjectPtrType>::isStale( double std ) const
{
    return ( timer_.elapsedMs() > (model_.mean()+std*model_.std()) );
}

template<class InterfacePrxType, class ConsumerPrxType, class ConsumerType, class ObjectPtrType>
void
AutoConsumerI<InterfacePrxType,ConsumerPrxType,ConsumerType,ObjectPtrType>::checkStatus()
{
    // debug
    std::cout<<"time elapsed    : "<<timer_.elapsedMs()<<std::endl;
    std::cout<<"mean, cov       : "<<model_.mean()<<", "<<model_.std()<<std::endl;
    std::cout<<"threshold       : "<<model_.mean()+3.0*model_.std()<<std::endl;
    
    if ( isStale() ) {
        context_.tracer()->debug("subscribing",9);
        subscribe();
        model_.reset();
        timer_.restart();
    }
    else {
        context_.tracer()->debug("not subscribing",9);
    }
    std::cout<<std::endl;
}

} // namespace

#endif
