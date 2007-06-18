/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef __pixmapIface_h__
#define __pixmapIface_h__

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
class PixMapIface : public IceUtil::Shared
{
friend class PixMapI;

public:
    //! Constructor
    PixMapIface( const std::string      &ifaceTag,
             const orcaice::Context &context );
    ~PixMapIface();

    // Local calls:
    // may throw orcaice::Exceptions
    void initInterface();
    // A local call which sets the data reported by the interface, 
    // and sends it through IceStorm
    void localSetAndSend( const ::orca::PixMapData &data );

private:
    // remote call implementations, mimic (but do not inherit) the orca interface
    orca::PixMapData getData() const;
    void subscribe(const ::orca::PixMapConsumerPrx&);
    void unsubscribe(const ::orca::PixMapConsumerPrx&);

    // Holds the latest data
    orcaice::Proxy<orca::PixMapData> dataProxy_;

    // The topic to which we'll publish
    IceStorm::TopicPrx             topicPrx_;
    // The interface to which we'll publish
    orca::PixMapConsumerPrx         consumerPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    std::string ifaceTag_;
    orcaice::Context context_;
};
typedef IceUtil::Handle<PixMapIface> PixMapIfacePtr;

}

#endif
