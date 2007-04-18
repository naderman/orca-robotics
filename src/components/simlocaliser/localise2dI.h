/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_STAGE_LOCALISER_LOCALISE2D_I_H
#define ORCA2_STAGE_LOCALISER_LOCALISE2D_I_H

#include <IceStorm/IceStorm.h>
#include <orca/localise2d.h>

#include <orcaice/ptrbuffer.h>

namespace simlocaliser
{

// Implements the Localise2d interface: Handles all our remote calls.
class Localise2dI : public orca::Localise2d
{
public:
    Localise2dI( const IceStorm::TopicPrx &localiseTopic,
                 orcaice::Buffer<orca::Localise2dData> & locBuffer );

    // remote calls:

    virtual ::orca::Localise2dData getData(const ::Ice::Current& ) const;

    virtual ::orca::Localise2dData getDataAtTime(const orca::Time&, const ::Ice::Current& ) const;

    virtual void subscribe(const ::orca::Localise2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::Localise2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

private:

    const IceStorm::TopicPrx localiseTopic_;
    orcaice::Buffer<orca::Localise2dData> &locBuffer_;
};

} // namespace

#endif
