#ifndef ORCA2_FAITH_LOCALISER_LOCALISE2D_I_H
#define ORCA2_FAITH_LOCALISER_LOCALISE2D_I_H

#include <IceStorm/IceStorm.h>
#include <orca/localise2d.h>

#include <orcaice/ptrbuffer.h>

namespace faithlocaliser
{

// Implements the Localise2d interface: Handles all our remote calls.
class Localise2dI : public orca::Localise2d
{
public:
    Localise2dI( const IceStorm::TopicPrx &localiseTopic,
                 orcaice::PtrBuffer<orca::Localise2dDataPtr> &posBuffer );

    // remote calls:

    virtual ::orca::Localise2dDataPtr getData(const ::Ice::Current& ) const;

    virtual void subscribe(const ::orca::Localise2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::Localise2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

private:

    const IceStorm::TopicPrx localiseTopic_;
    orcaice::PtrBuffer<orca::Localise2dDataPtr> &locBuffer_;
};

} // namespace

#endif
