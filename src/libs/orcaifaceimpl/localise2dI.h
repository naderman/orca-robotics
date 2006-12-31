#ifndef LOCALISE2D_I_H
#define LOCALISE2D_I_H

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/localise2d.h>

// utilities
#include <orcaice/proxy.h>
#include <orcaice/context.h>

namespace orcaifaceimpl {

//!
//! Implements the Localise2d interface: Handles remote calls.
//!
class Localise2dI : public orca::Localise2d
{
public:
    Localise2dI( const std::string &ifaceTag,
                 const orcaice::Context &context );

    // remote calls:

    virtual ::orca::Localise2dData getData(const ::Ice::Current& ) const;

    virtual void subscribe(const ::orca::Localise2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::Localise2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Note: could do smoothing/interpolation here...
    virtual orca::Localise2dData getDataAtTime(const orca::Time&, const Ice::Current& c) const
        { return getData(c); }

    // Local calls:

    // may throw orcaice::Exceptions
    void initInterface();
    // A local call which sets the data reported by the interface, 
    // and sends it through IceStorm
    void localSetAndSend( const orca::Localise2dData &data );

private:

    orcaice::Proxy<orca::Localise2dData> dataProxy_;

    orca::Localise2dConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx             topicPrx_;

    const std::string              ifaceTag_;
    orcaice::Context               context_;
};

}

#endif
