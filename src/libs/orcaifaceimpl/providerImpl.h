/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_PROVIDER_IMPL_H
#define ORCA_PROVIDER_IMPL_H

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

- idempotent DataType getData();
- IceStorm::Topic* subscribe( ConsumerType* );

and an associated consumer with
- void setData( DataType );

*/
template<class ProviderType, class ProviderPrxType, class ConsumerType, class ConsumerPrxType, class DataType>
class ProviderImpl : public IceUtil::Shared
{
friend class ProviderTypeI;

public:
    //! Constructor using interfaceTag (may throw ConfigFileException)
    ProviderImpl( const std::string& interfaceTag, const orcaice::Context& context ) :
        isDataSet_(false),
        interfaceName_(orcaice::getProvidedInterface(context,interfaceTag).iface),
        context_(context)
    {
        init();
    }

    //! constructor using interfaceName
    ProviderImpl( const orcaice::Context& context, const std::string& interfaceName ) :
        isDataSet_(false),
        interfaceName_(interfaceName),
        context_(context)
    {
        init();
    }           

    ~ProviderImpl()
    {
        orcaice::tryRemoveInterface( context_, interfaceName_ );
    }

    // local interface:
    //! May throw gbxutilacfr::Exceptions.
    void initInterface()
    {
        orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
    
        topicHandler_->connectToTopic();
    }

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the activity was stopped.
    void initInterface( gbxutilacfr::Stoppable* activity, const std::string& subsysName="", int retryInterval=2 )
    {
        orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, activity, subsysName, retryInterval );
    
        topicHandler_->connectToTopic( activity, subsysName, retryInterval );
    }

    //! A local call which sets the data reported by the interface
    void localSet( const DataType& data )
    {
        IceUtil::Mutex::Lock lock(mutex_);
        data_ = data;
        isDataSet_ = true;
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
        ProviderTypeI( ProviderImpl &impl )
            : impl_(impl) {}
        DataType getData(const Ice::Current&)
            { return impl_.internalGetData(); }
        virtual IceStorm::TopicPrx subscribe(const ConsumerPrxType& subscriber, const ::Ice::Current& = ::Ice::Current())
            { return impl_.internalSubscribe(subscriber); }
    private:
        ProviderImpl &impl_;
    };
    //////////////////////////////////////////////////////////////////////

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
        ptr_ = new ProviderTypeI( *this );

        topicHandler_.reset( new TopicHandlerType( orcaice::toTopicAsString(context_.name(),interfaceName_), context_ ) );    
    }

    // remote call implementations, mimic (but do not inherit) the orca interface
    DataType internalGetData() const
    {
        context_.tracer().debug( "ProviderImpl::internalGetData()", 5 );
    
        IceUtil::Mutex::Lock lock(mutex_);
        if ( !isDataSet_ )
        {
            std::stringstream ss;
            ss << "No data available! (interface="<<interfaceName_<<")";
            throw orca::DataNotExistException( ss.str() );
        }
        return data_;
    }

    IceStorm::TopicPrx internalSubscribe(const ConsumerPrxType& subscriber)
    {
        if ( !topicHandler_.get() ) 
        {
            throw orca::SubscriptionFailedException("Component does not have a topic to publish its traces.");
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

        // if we have data, send all the information we have to the new subscriber (and to no one else)
        if ( gotData )
        {
            return topicHandler_->subscribe( subscriber, data );
        }
        else
        {
            return topicHandler_->subscribe( subscriber );
        }
    }
};

}

#endif
