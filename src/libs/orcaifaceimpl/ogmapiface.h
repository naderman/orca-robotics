/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef __ogmapIface_h__
#define __ogmapIface_h__

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/ogmap.h>

#include <orcaice/proxy.h>
#include <orcaice/context.h>

namespace orcaifaceimpl {

//!
//! Implements the OgMap interface: Handles remote calls.
//!
class OgMapIface : public IceUtil::Shared
{
public:

    OgMapIface( const std::string      &ifaceTag,
                const orcaice::Context &context );
    ~OgMapIface();

    // Remote calls:
    orca::OgMapData getData() const;

    void subscribe(const ::orca::OgMapConsumerPrx& );

    void unsubscribe(const ::orca::OgMapConsumerPrx& );

    // Local calls:

    // may throw orcaice::Exceptions
    void initInterface();
    // A local call which sets the data reported by the interface, 
    // and sends it through IceStorm
    void localSetAndSend( const ::orca::OgMapData &data );

private:

    // Holds the latest data
    orcaice::Proxy<orca::OgMapData> dataProxy_;

    // The topic to which we'll publish
    IceStorm::TopicPrx             topicPrx_;
    // The interface to which we'll publish
    orca::OgMapConsumerPrx         consumerPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    std::string ifaceTag_;
    orcaice::Context context_;
};
typedef IceUtil::Handle<OgMapIface> OgMapIfacePtr;

}

#endif
