/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAIFACEIMPL_CONSUMER_H
#define ORCAIFACEIMPL_CONSUMER_H

#include <orcaice/context.h>
#include <orcaice/connectutils.h>

namespace orcaifaceimpl
{

// fwd decl
template<class ConsumerType, class ConsumerPrxType, class ObjectType>
class Consumer;

// generic consumerI: implements a consumer interface whose set method
// is "setData"
template<class ConsumerType, class ConsumerPrxType, class ObjectType>
class ConsumerI : public ConsumerType
{
public:

    ConsumerI( Consumer<ConsumerType,ConsumerPrxType,ObjectType> &consumer )
        : consumer_(consumer) {}

    virtual ~ConsumerI() {};
    
    // remote call
    virtual void setData( const ObjectType& data, const Ice::Current& );

private:
    Consumer<ConsumerType,ConsumerPrxType,ObjectType> &consumer_;
};

//
// A generic consumer: instantiates and looks after a consumerI.
// By 'looks after' I mean: adds it to/removes it from the adapter.
//
// Derived classes need to implement the 'setData' method.
//
template<class ConsumerType, class ConsumerPrxType, class ObjectType>
class Consumer 
{
public:
    Consumer( const orcaice::Context &context )
        : consumerI_(new ConsumerI<ConsumerType,ConsumerPrxType,ObjectType>(*this)),
          consumerPrx_(orcaice::createConsumerInterface<ConsumerPrxType>(context,consumerI_)),
          context_(context)
        {}
    virtual ~Consumer() 
        {
            try {
                context_.adapter()->remove( consumerPrx_->ice_getIdentity() );
            }
            // This can fail if the adapter is shutting down.  We don't care.
            catch ( ... ) {}
        }

    virtual void setData( const ObjectType &data )=0;

    ConsumerPrxType consumerPrx() const { return consumerPrx_; }

private:
    Ice::ObjectPtr   consumerI_;
    ConsumerPrxType  consumerPrx_;
    orcaice::Context context_;
};


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

template<class ConsumerType, class ConsumerPrxType, class ObjectType>
void 
ConsumerI<ConsumerType,ConsumerPrxType,ObjectType>::setData( const ObjectType& data, const Ice::Current& )
{
    consumer_.setData( data );
}

} // namespace

#endif
