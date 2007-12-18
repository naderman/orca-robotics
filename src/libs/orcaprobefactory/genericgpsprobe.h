/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAPROBEFACTORY_GENERIC_GPS_PROBE_H
#define ORCA_ORCAPROBEFACTORY_GENERIC_GPS_PROBE_H

#include <orcaprobe/interfaceprobe.h>
#include <orcaice/connectutils.h>
#include <orcaprobe/orcaprobe.h>

namespace orcaprobefactory
{

template<typename ConsumerType,
         typename ConsumerPrxType,
         typename InterfacePrxType,
         typename DataType,
         typename DescriptionType>
class GenericGpsProbe : public ConsumerType, public orcaprobe::InterfaceProbe
{

public:

    GenericGpsProbe( const orca::FQInterfaceName& name,
                     orcaprobe::IDisplay& display,                     
                     const std::string &id,
                     const orcaice::Context& context );

    virtual int loadOperationEvent( const int index, orcacm::OperationData& data );
    
    virtual void setData(const DataType& data, const Ice::Current&);

private:

    int loadGetDescription( orcacm::OperationData& data );
    int loadGetData( orcacm::OperationData& data );
    int loadSubscribe( orcacm::OperationData& data );
    int loadUnsubscribe( orcacm::OperationData& data );

    ConsumerPrxType callbackPrx_;
    orcacm::OperationData subscribeOperationData_;
};

template<typename ConsumerType,
         typename ConsumerPrxType,
         typename InterfacePrxType,
         typename DataType,
         typename DescriptionType>
GenericGpsProbe<ConsumerType,
                ConsumerPrxType,
                InterfacePrxType,
                DataType,
                DescriptionType>::GenericGpsProbe( const orca::FQInterfaceName&  name,
                                                   orcaprobe::IDisplay&     display,
                                                   const std::string&            id,
                                                   const orcaice::Context&       context )
    : InterfaceProbe(name,display,context)
{
    id_ = id;
    
    addOperation( "getDescription" );
    addOperation( "getData" );
    addOperation( "subscribe" );
    addOperation( "unsubscribe" );

    Ice::ObjectPtr consumer = this;
    callbackPrx_ = orcaice::createConsumerInterface<ConsumerPrxType>( context_, consumer );
}

template<typename ConsumerType,
         typename ConsumerPrxType,
         typename InterfacePrxType,
         typename DataType,
         typename DescriptionType>
int 
GenericGpsProbe<ConsumerType,
                ConsumerPrxType,
                InterfacePrxType,
                DataType,
                DescriptionType>::loadOperationEvent( const int index,
                                                      orcacm::OperationData& data )
{
    switch ( index )
    {
    case orcaprobe::UserIndex :
        return loadGetDescription( data );
    case orcaprobe::UserIndex+1 :
        return loadGetData( data );
    case orcaprobe::UserIndex+2 :
        return loadSubscribe( data );
    case orcaprobe::UserIndex+3 :
        return loadUnsubscribe( data );
    }
    return 1;
}

template<typename ConsumerType,
         typename ConsumerPrxType,
         typename InterfacePrxType,
         typename DataType,
         typename DescriptionType>
int 
GenericGpsProbe<ConsumerType,
                ConsumerPrxType,
                InterfacePrxType,
                DataType,
                DescriptionType>::loadGetData( orcacm::OperationData& data )
{
    DataType result;
    try
    {
        InterfacePrxType derivedPrx = InterfacePrxType::checkedCast(prx_);
        result = derivedPrx->getData();
        orcaprobe::reportResult( data, "data", orcaice::toString(result) );
    }
    catch( const Ice::Exception& e )
    {
        std::stringstream ss;
        ss<<e<<std::endl;
        orcaprobe::reportException( data, ss.str() );
    }
    return 0;
}

template<typename ConsumerType,
         typename ConsumerPrxType,
         typename InterfacePrxType,
         typename DataType,
         typename DescriptionType>
int 
GenericGpsProbe<ConsumerType,
                ConsumerPrxType,
                InterfacePrxType,
                DataType,
                DescriptionType>::loadGetDescription( orcacm::OperationData& data )
{
    DescriptionType result;
    orcacm::ResultHeader res;

    try
    {
        InterfacePrxType derivedPrx = InterfacePrxType::checkedCast(prx_);
        result = derivedPrx->getDescription();
        orcaprobe::reportResult( data, "data", orcaice::toString(result) );
    }
    catch( const Ice::Exception& e )
    {
        std::stringstream ss;
        ss<<e<<std::endl;
        orcaprobe::reportException( data, ss.str() );
    }
    return 0;
}

template<typename ConsumerType,
         typename ConsumerPrxType,
         typename InterfacePrxType,
         typename DataType,
         typename DescriptionType>
int 
GenericGpsProbe<ConsumerType,
                ConsumerPrxType,
                InterfacePrxType,
                DataType,
                DescriptionType>::loadSubscribe( orcacm::OperationData& data )
{
    try
    {
        InterfacePrxType derivedPrx = InterfacePrxType::checkedCast(prx_);
        derivedPrx->subscribe( callbackPrx_ );
        orcaprobe::reportSubscribed( data );

        // save the op data structure so we can use it when the data arrives
        subscribeOperationData_ = data;
    }
    catch( const Ice::Exception& e )
    {
        std::stringstream ss;
        ss<<e<<std::endl;
        orcaprobe::reportException( data, ss.str() );
    }
    return 0;
}

template<typename ConsumerType,
         typename ConsumerPrxType,
         typename InterfacePrxType,
         typename DataType,
         typename DescriptionType>
int 
GenericGpsProbe<ConsumerType,
                ConsumerPrxType,
                InterfacePrxType,
                DataType,
                DescriptionType>::loadUnsubscribe( orcacm::OperationData& data )
{    
    try
    {
        InterfacePrxType derivedPrx = InterfacePrxType::checkedCast(prx_);
        derivedPrx->unsubscribe( callbackPrx_ );
        orcaprobe::reportUnsubscribed( data );
    }
    catch( const Ice::Exception& e )
    {
        std::stringstream ss;
        ss<<e<<std::endl;
        orcaprobe::reportException( data, ss.str() );
    }
    return 0;
}

template<typename ConsumerType,
         typename ConsumerPrxType,
         typename InterfacePrxType,
         typename DataType,
         typename DescriptionType>
void 
GenericGpsProbe<ConsumerType,
                ConsumerPrxType,
                InterfacePrxType,
                DataType,
                DescriptionType>::setData( const DataType& result,
                                           const Ice::Current& )
{
    subscribeOperationData_.results.clear();
    orcaprobe::reportResult( subscribeOperationData_, "data", orcaice::toString(result) );
    display_.setOperationData( subscribeOperationData_ );
};

} // namespace

#endif
