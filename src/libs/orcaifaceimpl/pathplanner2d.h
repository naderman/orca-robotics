/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_PATHPLANNER2D_H
#define ORCAIFACEIMPL_PATHPLANNER2D_H

// TODO: this one does not exist but probably should.
// #include <orcaifaceimpl/pathplanner2dImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

#include <orca/pathplanner2d.h>

namespace orcaifaceimpl {

typedef StoringConsumerImpl<orca::PathPlanner2d, orca::PathPlanner2dPrx,
                    orca::PathPlanner2dConsumer, orca::PathPlanner2dConsumerPrx,
                    orca::PathPlanner2dData> StoringPathPlanner2dConsumerImpl;
typedef IceUtil::Handle<StoringPathPlanner2dConsumerImpl> StoringPathPlanner2dConsumerImplPtr;

typedef BufferedConsumerImpl<orca::PathPlanner2d, orca::PathPlanner2dPrx,
                        orca::PathPlanner2dConsumer, orca::PathPlanner2dConsumerPrx,
                        orca::PathPlanner2dData> BufferedPathPlanner2dConsumerImpl;
typedef IceUtil::Handle<BufferedPathPlanner2dConsumerImpl> BufferedPathPlanner2dConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::PathPlanner2d, orca::PathPlanner2dPrx,
                        orca::PathPlanner2dConsumer,
                        orca::PathPlanner2dConsumerPrx,
                        orca::PathPlanner2dData> NotifyingPathPlanner2dConsumerImpl;
typedef IceUtil::Handle<NotifyingPathPlanner2dConsumerImpl> NotifyingPathPlanner2dConsumerImplPtr;

typedef PrintingConsumerImpl<orca::PathPlanner2d, orca::PathPlanner2dPrx,
                        orca::PathPlanner2dConsumer, orca::PathPlanner2dConsumerPrx,
                        orca::PathPlanner2dData> PrintingPathPlanner2dConsumerImpl;
typedef IceUtil::Handle<PrintingPathPlanner2dConsumerImpl> PrintingPathPlanner2dConsumerImplPtr;

}

#endif
