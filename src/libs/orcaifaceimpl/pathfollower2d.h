/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_PATHFOLLOWER2D_H
#define ORCAIFACEIMPL_PATHFOLLOWER2D_H

#include <orcaifaceimpl/pathfollower2dImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

#include <orca/pathfollower2d.h>

namespace orcaifaceimpl {

typedef StoringConsumerImpl<orca::PathFollower2d, orca::PathFollower2dPrx,
                    orca::PathFollower2dConsumer, orca::PathFollower2dConsumerPrx,
                    orca::PathFollower2dData> StoringPathFollower2dConsumerImpl;
typedef IceUtil::Handle<StoringPathFollower2dConsumerImpl> StoringPathFollower2dConsumerImplPtr;

typedef BufferedConsumerImpl<orca::PathFollower2d, orca::PathFollower2dPrx,
                        orca::PathFollower2dConsumer, orca::PathFollower2dConsumerPrx,
                        orca::PathFollower2dData> BufferedPathFollower2dConsumerImpl;
typedef IceUtil::Handle<BufferedPathFollower2dConsumerImpl> BufferedPathFollower2dConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::PathFollower2d, orca::PathFollower2dPrx,
                        orca::PathFollower2dConsumer,
                        orca::PathFollower2dConsumerPrx,
                        orca::PathFollower2dData> NotifyingPathFollower2dConsumerImpl;
typedef IceUtil::Handle<NotifyingPathFollower2dConsumerImpl> NotifyingPathFollower2dConsumerImplPtr;

typedef PrintingConsumerImpl<orca::PathFollower2d, orca::PathFollower2dPrx,
                        orca::PathFollower2dConsumer, orca::PathFollower2dConsumerPrx,
                        orca::PathFollower2dData> PrintingPathFollower2dConsumerImpl;
typedef IceUtil::Handle<PrintingPathFollower2dConsumerImpl> PrintingPathFollower2dConsumerImplPtr;

}

#endif
