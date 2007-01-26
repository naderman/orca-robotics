/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef __pixmapI_h__
#define __pixmapI_h__

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/pixmap.h>

#include <orcaice/proxy.h>
#include <orcaice/context.h>

namespace orcaifaceimpl {

//!
//! Implements the PixMap interface: Handles remote calls.
//!
class PixMapI : public orca::PixMap
{
public:

    PixMapI( const std::string      &ifaceTag,
            const orcaice::Context &context );

    // Remote calls:
    orca::PixMapData getData(const Ice::Current&) const;

    virtual void subscribe(const ::orca::PixMapConsumerPrx&,
                           const Ice::Current&);

    virtual void unsubscribe(const ::orca::PixMapConsumerPrx&,
                             const Ice::Current&);

    // Local calls:

    // may throw orcaice::Exceptions
    void initInterface();
    // A local call which sets the data reported by the interface, 
    // and sends it through IceStorm
    void localSetAndSend( const ::orca::PixMapData &data );

private:

    // Holds the latest data
    orcaice::Proxy<orca::PixMapData> dataProxy_;

    // The topic to which we'll publish
    IceStorm::TopicPrx             topicPrx_;
    // The interface to which we'll publish
    orca::PixMapConsumerPrx         consumerPrx_;

    std::string ifaceTag_;
    orcaice::Context context_;
};

}

#endif
