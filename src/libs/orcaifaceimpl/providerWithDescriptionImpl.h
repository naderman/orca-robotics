/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_PROVIDER_WITH_DESCRIPTION_IMPL_H
#define ORCA_PROVIDER_WITH_DESCRIPTION_IMPL_H

#include <memory>
#include <orcaice/context.h>
#include <orcaice/topichandler.h>
#include <orcaice/configutils.h>
#include <orcaice/convertutils.h>
#include <orcaice/iceutils.h>
#include <orcaice/multiiceutils.h>
#include <IceUtil/Mutex.h>

namespace gbxutilacfr { class Stoppable; }

namespace orcaifaceimpl {

/*!
Implements an interface with the following methods:

- idempotent DescriptionType getDescription();
- idempotent DataType getData();
- IceStorm::Topic* subscribe( ConsumerType* );

and an associated consumer with
- void setData( DataType );

*/
// template<class ProviderType, class ProviderPrxType, class ConsumerType, class ConsumerPrxType, class DataType, class DescriptionType>
template<class ProviderType, class ConsumerType, class DataType, class DescriptionType>
class ProviderWithDescriptionImpl : public IceUtil::Shared
{
typedef typename ProviderType::ProxyType ProviderPrxType;
typedef typename ConsumerType::ProxyType ConsumerPrxType;

friend class ProviderTypeI;

public:
    //! Constructor using interfaceTag (may throw ConfigFileException).
    //! This class must assume ownership of the description, i.e. if DescriptionType is a smart pointer, it should not be
    //! modified after this class is created.
    ProviderWithDescriptionImpl( const DescriptionType& descr, const std::string& interfaceTag, const orcaice::Context& context ) :
        descr_(descr),
        isDataSet_(false),
        interfaceName_(orcaice::getProvidedInterface(context,interfaceTag).iface),
        context_(context)
    {
        init();
    }

    //! Constructor using interfaceName
    ProviderWithDescriptionImpl( const DescriptionType& descr, const orcaice::Context& context, const std::string& interfaceName ) :
        descr_(descr),
        isDataSet_(false),
        interfaceName_(interfaceName),
        context_(context)
    {
        init();
    }           

    ~ProviderWithDescriptionImpl()
    {
        orcaice::tryRemoveInterface( context_, interfaceName_ );
    }

    // local interface:
    //! Creates the interface and connects to the IceStorm topic.
    //! May throw gbxutilacfr::Exceptions.
    void initInterface()
    {
        // Set up the topic before creating the interface, so no-one
        // can subscribe before the topic is kosher.
        topicHandler_->connectToTopic();
        orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );    
    }
    //! Same as above but sets the data before initializing the interface and sends it out after.
    void initInterface( const DataType& data )
    {
        // by setting the data first we avoid the possibility of initializing the interface
        // but not having the data in it.
        localSet( data );

        // Set up the topic before creating the interface, so no-one
        // can subscribe before the topic is kosher.
        topicHandler_->connectToTopic();
        topicHandler_->publish( data );
        orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );    
    }

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the activity was stopped.
    void initInterface( gbxutilacfr::Stoppable* activity, const std::string& subsysName="", int retryInterval=2 )
    {
        topicHandler_->connectToTopic( activity, subsysName, retryInterval );
        orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, activity, subsysName, retryInterval );
    }
    //! Same as above but sets the data before initializing the interface and sends it out after.
    void initInterface( const DataType& data, 
                        gbxutilacfr::Stoppable* activity, const std::string& subsysName="", int retryInterval=2 )
    {
        // by setting the data first we avoid the possibility of initializing the interface
        // but not having the data in it.
        localSet( data );

        topicHandler_->connectToTopic( activity, subsysName, retryInterval );
        topicHandler_->publish( data );
        orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, activity, subsysName, retryInterval );
    }

    //! A local call which sets the data reported by the interface
    void localSet( const DataType& data )
    {
        IceUtil::Mutex::Lock lock(mutex_);
        data_ = data;
        isDataSet_ = true;
    }

    //! A local call which sends the data through IceStorm
    void localSend( const DataType& data )
    {
        topicHandler_->publish(data);
    }

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const DataType& data )
    {
        {
            IceUtil::Mutex::Lock lock(mutex_);
            data_ = data;
            isDataSet_ = true;
        }
        topicHandler_->publish( data );
    }

    //! A local call which sets the data reported by the interface, 
    //! and sends a different thing through IceStorm.
    //! This is useful if e.g. you want to push an update to a
    //! structure, but have the full structure available to subscribers.
    void localSetAndSend( const DataType& dataToSet, const DataType& dataToSend )
    {
        // By sending first, we avoid the possibility that a client
        // gets our data on subscription then gets pushed the update.
        topicHandler_->publish( dataToSend );
        {
            IceUtil::Mutex::Lock lock(mutex_);
            data_ = dataToSet;
            isDataSet_ = true;
        }
    }

private:    
    //
    // This is the implementation of the slice-defined interface
    //
    class ProviderTypeI : public ProviderType
    {
    public:
        ProviderTypeI( ProviderWithDescriptionImpl &impl )
            : impl_(impl) {}
        virtual DescriptionType getDescription(const ::Ice::Current&)
            { return impl_.internalGetDescription(); }
        virtual DataType getData(const Ice::Current&)
            { return impl_.internalGetData(); }
        virtual IceStorm::TopicPrx subscribe(const ConsumerPrxType& subscriber, const ::Ice::Current& = ::Ice::Current())
            { return impl_.internalSubscribe(subscriber); }
    private:
        ProviderWithDescriptionImpl &impl_;
    };
    //////////////////////////////////////////////////////////////////////

    DescriptionType descr_;

    // Holds the current data
    DataType data_;
    bool isDataSet_;
    // Protects the data_
    IceUtil::Mutex mutex_;

    typedef orcaice::TopicHandler<ConsumerPrxType,DataType> TopicHandlerType;
    std::auto_ptr<TopicHandlerType> topicHandler_;

    Ice::ObjectPtr ptr_;
    const std::string interfaceName_;
    orcaice::Context context_;

    void init()
    {
        topicHandler_.reset( new TopicHandlerType( orcaice::toTopicAsString(context_.name(),interfaceName_), context_ ) );    
        ptr_ = new ProviderTypeI( *this );
    }

    // remote call implementations, mimic (but do not inherit) the orca interface
    DescriptionType internalGetDescription() const
    {
        return descr_;
    }

    DataType internalGetData() const
    {
        context_.tracer().debug( "ProviderWithDescriptionImpl::internalGetData()", 5 );
    
        IceUtil::Mutex::Lock lock(mutex_);
        if ( !isDataSet_ )
        {
            std::stringstream ss;
            ss << "No data available! (interface="<<interfaceName_<<")";
            throw orca::DataNotExistException( ss.str() );
        }
        return data_;
    }

    IceStorm::TopicPrx internalSubscribe( const ConsumerPrxType& subscriber )
    {
        if ( !topicHandler_.get() ) 
        {
            throw orca::SubscriptionFailedException("topicHandler_ is not initialised!");
        }
        
        // Grab the data in a crit section
        DataType data;
        bool gotData = false;
        {        
            IceUtil::Mutex::Lock lock(mutex_);
            if ( isDataSet_ )
            {
                data = data_;
                gotData = true;
            }
        }

        // EXPERIMENTAL!
        IceStorm::QoS qos;
        qos["reliability"] = "ordered";

        // if we have data, send all the information we have to the new subscriber (and to no one else)
        if ( gotData )
        {
            return topicHandler_->subscribe( subscriber, data, qos );
        }
        else
        {
            return topicHandler_->subscribe( subscriber, qos );
        }
    }
};

}

#endif
