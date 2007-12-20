/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef __pixmapIface_h__
#define __pixmapIface_h__

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/pixmap.h>

#include <hydroutil/store.h>
#include <orcaice/context.h>

namespace orcaifaceimpl {

//!
//! Implements the PixMap interface: Handles remote calls.
//!
class PixMapImpl : public IceUtil::Shared
{
friend class PixMapI;

public:
    //! Constructor using interfaceTag (may throw ConfigFileException)
    PixMapImpl( const std::string      &interfaceTag,
                const orcaice::Context &context );
    //! constructor using interfaceName
    PixMapImpl( const orcaice::Context &context,
                const std::string      &interfaceName );                
    ~PixMapImpl();

    // Local calls:
    // may throw hydroutil::Exceptions
    void initInterface();
    // A local call which sets the data reported by the interface, 
    // and sends it through IceStorm
    void localSetAndSend( const ::orca::PixMapData &data );

private:
    // remote call implementations, mimic (but do not inherit) the orca interface
    orca::PixMapData internalGetData() const;
    void internalSubscribe(const ::orca::PixMapConsumerPrx&);
    void internalUnsubscribe(const ::orca::PixMapConsumerPrx&);

    // Holds the latest data
    hydroutil::Store<orca::PixMapData> dataStore_;

    // The topic to which we'll publish
    IceStorm::TopicPrx             topicPrx_;
    // The interface to which we'll publish
    orca::PixMapConsumerPrx         consumerPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string interfaceName_;
    const std::string topicName_;
    orcaice::Context context_;
};
typedef IceUtil::Handle<PixMapImpl> PixMapImplPtr;

}

#endif
