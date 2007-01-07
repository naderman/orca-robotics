/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ODOMETRY2D_I_H
#define ORCA2_ODOMETRY2D_I_H

#include <orca/odometry2d.h>
#include <IceStorm/IceStorm.h>

// utilities
#include <orcaice/proxy.h>
#include <orcaice/context.h>

namespace orcaifaceimpl {

//!
//! Implements the orca::Odometry2d interface: Handles remote calls.
//!
class Odometry2dI : public orca::Odometry2d
{
public:
    //! Set publishInterval to zero if you want every measurement to be pushed to IceStorm.
    Odometry2dI( const orca::Odometry2dDescription& descr,
                 const std::string& ifaceTag, 
                 const orcaice::Context& context );
    virtual ~Odometry2dI();

    // remote interface

    virtual ::orca::Odometry2dData getData(const ::Ice::Current& ) const;

    virtual ::orca::Odometry2dDescription getDescription(const ::Ice::Current& ) const;

    virtual void subscribe(const ::orca::Odometry2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::Odometry2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());


    // local interface:

    /*! 
    May throw orcaice::Exceptions. When calling from within a thread, you should set up a loop:
@verbatim
while ( isActive() ) {
    try {
        obj->initInterface();
    }
    catch ( const Ice::Exception& e ) {
        // give feedback
        context_.tracer()->warning( "Failed to setup interface. Check Registry and IceStorm. Will try again in 2 secs...");
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }
}
@endverbatim
    */
    void initInterface();

    //! A local call which sets the data reported by the interface
    void localSet( const orca::Odometry2dData& data );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::Odometry2dData& data );

private:

    orca::Odometry2dDescription     descr_;
    orcaice::Proxy<orca::Odometry2dData> dataProxy_;

    orca::Odometry2dConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx             topicPrx_;

    const std::string              tag_;
    orcaice::Context               context_;
};

typedef IceUtil::Handle<Odometry2dI> Odometry2dIPtr;

}

#endif
