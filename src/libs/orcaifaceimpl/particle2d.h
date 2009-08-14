/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_PARTICLE2D_H
#define ORCAIFACEIMPL_PARTICLE2D_H

#include <orca/particle2d.h>
#include <orcaifaceutil/particle2d.h>

#include <orcaifaceimpl/providerImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

namespace orcaifaceimpl {

typedef ProviderImpl<orca::Particle2d, orca::Particle2dConsumer,
                    orca::Particle2dData> Particle2dImpl;
typedef IceUtil::Handle<Particle2dImpl> Particle2dImplPtr;

typedef StoringConsumerImpl<orca::Particle2d, orca::Particle2dConsumer,
                    orca::Particle2dData> StoringParticle2dConsumerImpl;
typedef IceUtil::Handle<StoringParticle2dConsumerImpl> StoringParticle2dConsumerImplPtr;

typedef BufferedConsumerImpl<orca::Particle2d, orca::Particle2dConsumer,
                        orca::Particle2dData> BufferedParticle2dConsumerImpl;
typedef IceUtil::Handle<BufferedParticle2dConsumerImpl> BufferedParticle2dConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::Particle2d, orca::Particle2dConsumer,
                        orca::Particle2dData> NotifyingParticle2dConsumerImpl;
typedef IceUtil::Handle<NotifyingParticle2dConsumerImpl> NotifyingParticle2dConsumerImplPtr;

typedef PrintingConsumerImpl<orca::Particle2d, orca::Particle2dConsumer,
                        orca::Particle2dData> PrintingParticle2dConsumerImpl;
typedef IceUtil::Handle<PrintingParticle2dConsumerImpl> PrintingParticle2dConsumerImplPtr;

}

#endif
