/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAIFACEIMPL_INTERFACE_MANAGER_H
#define ORCAIFACEIMPL_INTERFACE_MANAGER_H

#include <Ice/Ice.h>

namespace orcaifaceimpl
{

/*!

class MainTread
{
public:
    void run();

private:
    orcaifaceimpl::LaserScanner2dImplPtr producer_;
    orcaifaceimpl::InterfaceManagerPtr producerManager_;

    orcaifaceimpl::StoringLaserScanner2dConsumerImplPtr consumer_;
    orcaifaceimpl::InterfaceManagerPtr consumerManager_;
};

void MainTread::run()
{

    for ( ;; ) 
    {
        producer_ = new orcaifaceimpl::LaserScanner2dImpl( descr, "LaserScanner2d", context_ );
        producerManager_ = new orcaifaceimpl::InterfaceManager( producer_ );
        producer_->initInterface( this, subsysName() );
        producer_->localSetAndSend( data );
        // producer_->destroy();

        consumer_ = new orcaifaceimpl::LaserScanner2dConsumerImplPtr( context_ );
        consumerManager_ = new orcaifaceimpl::InterfaceManager( consumer_ );
        consumer_->subscribeWithTag( "Laser2d" );
        consumer_->store().get( data );
        // consumer_->destroy();
    }
}

*/
class InterfaceManager : public IceUtil::Shared
{
public:
    InterfaceManager( const Ice::ObjectAdapterPtr& adapter );
    InterfaceManager( const Ice::ObjectAdapterPtr& adapter, const Ice::ObjectPtr& ptr, const Ice::ObjectPrx& prx );
    ~InterfaceManager();

    Ice::ObjectPrx prx();
    Ice::ObjectPtr ptr();
    void setInterface( const Ice::ObjectPtr& ptr, const Ice::ObjectPrx& prx );

private:
    IceUtil::Mutex mutex_;
    Ice::ObjectAdapterPtr adapter_;
    Ice::ObjectPtr ptr_;
    Ice::ObjectPrx prx_;
    
};
typedef IceUtil::Handle<InterfaceManager> InterfaceManagerPtr;

} // namespace

#endif
