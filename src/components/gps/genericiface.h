/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_GENERIC_IFACE_H
#define ORCA2_GENERIC_IFACE_H

#include <IceStorm/IceStorm.h>

#include <orcaice/proxy.h>
#include <orcaice/context.h>
#include <orcaifaceimpl/util.h>

namespace gps {

//!
//! Implements the Generic interface: Handles remote calls.
//!
template<typename InterfaceType,
         typename DataType,
         typename ConsumerPrxType,
         typename DescriptionType>
class GenericIface : public IceUtil::Shared
{
public:
    GenericIface( const std::string      &ifaceTag,
                  const DescriptionType  &descr,
                  const orcaice::Context &context );
    ~GenericIface();

    // remote calls:

    DataType getData() const;

    void subscribe(const ConsumerPrxType& );

    void unsubscribe(const ConsumerPrxType& );

    DescriptionType getDescription() const
        { return description_; }

    // Local calls:

    //! may throw orcaice::Exceptions
    void initInterface();

    //! A local call which sets the data reported by the interface
    void localSet( const DataType &data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const DataType &data );

private:

    orcaice::Proxy<DataType>  dataProxy_;

    ConsumerPrxType     consumerPrx_;
    IceStorm::TopicPrx  topicPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string     ifaceTag_;
    const DescriptionType description_;
    orcaice::Context      context_;
};

namespace detail {

    template <class InterfaceType,
              typename DataType,
              typename ConsumerPrxType,
              typename DescriptionType>
    class InterfaceI : public InterfaceType 
    {
        typedef GenericIface<InterfaceType,DataType,ConsumerPrxType,DescriptionType> GenericIfaceType;

    public:
        InterfaceI( GenericIfaceType &iface )
            : iface_(iface) {}

        // remote calls:

        virtual DataType getData(const ::Ice::Current& ) const
            { return iface_.getData(); }

        virtual void subscribe(const ConsumerPrxType& subscriber,
                               const ::Ice::Current& = ::Ice::Current())
            { iface_.subscribe( subscriber ); }

        virtual void unsubscribe(const ConsumerPrxType& subscriber,
                                 const ::Ice::Current& = ::Ice::Current())
            { iface_.unsubscribe( subscriber ); }

        virtual DescriptionType getDescription(const Ice::Current& ) const
            { return iface_.getDescription(); }

    private:
        GenericIfaceType &iface_;
    };
}

//////////////////////////////////////////////////////////////////////

template<typename InterfaceType,
         typename DataType,
         typename ConsumerPrxType,
         typename DescriptionType>
GenericIface<InterfaceType,DataType,ConsumerPrxType,DescriptionType>::GenericIface( const std::string &ifaceTag,
                                                                                    const DescriptionType &descr,
                                                                                    const orcaice::Context &context ) :
    ifaceTag_(ifaceTag),
    description_(descr),
    context_(context)
{
}

template<typename InterfaceType,
         typename DataType,
         typename ConsumerPrxType,
         typename DescriptionType>
GenericIface<InterfaceType,DataType,ConsumerPrxType,DescriptionType>::~GenericIface()
{
    orcaifaceimpl::tryRemovePtr( context_, ptr_ );
}

template<typename InterfaceType,
         typename DataType,
         typename ConsumerPrxType,
         typename DescriptionType>
void
GenericIface<InterfaceType,DataType,ConsumerPrxType,DescriptionType>::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<ConsumerPrxType>
        ( context_, consumerPrx_, ifaceTag_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new detail::InterfaceI<InterfaceType,DataType,ConsumerPrxType,DescriptionType>( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, ifaceTag_ );
}

template<typename InterfaceType,
         typename DataType,
         typename ConsumerPrxType,
         typename DescriptionType>
DataType
GenericIface<InterfaceType,DataType,ConsumerPrxType,DescriptionType>::getData() const
{
    context_.tracer()->debug( "GenericIface::getData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (ifaceTag="<<ifaceTag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    DataType data;
    dataProxy_.get( data );
    return data;
}

template<typename InterfaceType,
         typename DataType,
         typename ConsumerPrxType,
         typename DescriptionType>
void 
GenericIface<InterfaceType,DataType,ConsumerPrxType,DescriptionType>::subscribe(const ConsumerPrxType& subscriber)
{
    context_.tracer()->debug( "GenericIface::subscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    try {
        topicPrx_->subscribeAndGetPublisher( IceStorm::QoS(), subscriber->ice_twoway() );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"GenericIface::subscribe: failed to subscribe: "<< e << std::endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

template<typename InterfaceType,
         typename DataType,
         typename ConsumerPrxType,
         typename DescriptionType>
void 
GenericIface<InterfaceType,DataType,ConsumerPrxType,DescriptionType>::unsubscribe(const ConsumerPrxType& subscriber)
{
    context_.tracer()->debug( "GenericIface::unsubscribe(): unsubscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

template<typename InterfaceType,
         typename DataType,
         typename ConsumerPrxType,
         typename DescriptionType>
void
GenericIface<InterfaceType,DataType,ConsumerPrxType,DescriptionType>::localSet( const DataType &data )
{
    //cout<<"TRACE(localise2dIface.cpp): localSetData: " << orcaice::toString(data) << std::endl;

    dataProxy_.set( data );
}

template<typename InterfaceType,
         typename DataType,
         typename ConsumerPrxType,
         typename DescriptionType>
void
GenericIface<InterfaceType,DataType,ConsumerPrxType,DescriptionType>::localSetAndSend( const DataType &data )
{
    dataProxy_.set( data );
    
    // Try to push to IceStorm.
    orcaifaceimpl::tryPushToIceStormWithReconnect<ConsumerPrxType,DataType>
        ( context_,
          consumerPrx_,
          data,
          topicPrx_,
          ifaceTag_ );
}

}

#endif
