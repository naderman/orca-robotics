/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_STATUS_IMPL_H
#define ORCAICE_STATUS_IMPL_H

#include <hydroiceutil/localstatus.h>
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <gbxsickacfr/gbxiceutilacfr/timer.h>

#include <orca/status.h>
#include <IceStorm/IceStorm.h>
#include <orcaice/context.h>

// class gbxiceutilacfr::Thread;

namespace orcaice
{

class StatusImpl : public hydroiceutil::LocalStatus, public IceUtil::Shared
{
friend class StatusI;

public:
    StatusImpl( const orcaice::Context& context );
    ~StatusImpl();

    void initInterface( const orcaice::Context& context );
//     void initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName="", int retryInterval=2 );

private:

    // from LocalStatus
    virtual void publishEvent( const hydroiceutil::NameStatusMap& subsystems );

    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::StatusData internalGetData() const;
    void internalSubscribe(const ::orca::StatusConsumerPrx&);
    void internalUnsubscribe(const ::orca::StatusConsumerPrx&);
    IceStorm::TopicPrx internalTopic();

    gbxiceutilacfr::Store<orca::StatusData> dataStore_;
    // we don't need to protect it if we only read from it.
    // todo: make a special read-only class.
    gbxiceutilacfr::Timer upTimer_;

    // todo: do we need to protect these as well?
    orca::StatusConsumerPrx    publisherPrx_;
    IceStorm::TopicPrx             topicPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string              interfaceName_;
    const std::string              topicName_;
    orcaice::Context               context_;

//     bool isStatusTopicRequired_;
};

typedef IceUtil::Handle<StatusImpl> StatusImplPtr;

} // namespace

#endif
