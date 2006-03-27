/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

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
