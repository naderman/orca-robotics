/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_STATUS_IMPL_H
#define ORCAICE_STATUS_IMPL_H

#include <memory>
#include <hydroiceutil/localstatus.h>
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <gbxsickacfr/gbxiceutilacfr/timer.h>

#include <orca/status.h>
#include <orcaice/topichandler.h>
#include <orcaice/context.h>

// class gbxiceutilacfr::Thread;

namespace orcaice
{
namespace detail
{

class ComponentStatusAggregator;

class StatusImpl : public hydroiceutil::LocalStatus, public IceUtil::Shared
{
friend class StatusI;

public:
    StatusImpl( const orcaice::Context& context );
    ~StatusImpl();

    void updateContext( const orcaice::Context& context ) { context_ = context; };
    void initInterface();
//     void initInterface( gbxutilacfr::Stoppable* activity, const std::string& subsysName="", int retryInterval=2 );

private:

    // from LocalStatus
    virtual void publishEvent( const hydroiceutil::LocalComponentStatus& componentStatus );

    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::StatusData internalGetData();
    IceStorm::TopicPrx  internalSubscribe(const ::orca::StatusConsumerPrx&);

    gbxiceutilacfr::Store<orca::StatusData> dataStore_;
    // we don't need to protect it if we only read from it.
    // todo: make a special read-only class.
    gbxiceutilacfr::Timer upTimer_;

    std::auto_ptr<ComponentStatusAggregator> aggregator_;

    typedef TopicHandler<orca::StatusConsumerPrx,orca::StatusData> StatusTopicHandler;
    std::auto_ptr<StatusTopicHandler> topicHandler_;
    void initTopicHandler();

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;
    const std::string       interfaceName_;
    orcaice::Context        context_;
};

typedef IceUtil::Handle<StatusImpl> StatusImplPtr;

} // namespace
} // namespace

#endif
