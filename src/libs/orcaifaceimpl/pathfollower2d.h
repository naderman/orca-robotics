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

#include <orca/pathfollower2d.h>
#include <orcaifaceutil/pathfollower2d.h>

#include <orcaifaceimpl/pathfollower2dImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

namespace orcaifaceimpl {

//
// We need a special class here because we have the additional 'setState' function.
//
class PathFollower2dConsumerTypeI : public orca::PathFollower2dConsumer {
public:
    PathFollower2dConsumerTypeI( orcaifaceimpl::AbstractConsumer<orca::PathFollower2dData> &impl ) :
        impl_(impl)
        {}

    void setData( const orca::PathFollower2dData &data, const Ice::Current& )
        {
            impl_.dataEvent( data );
        }
    void setState( const orca::PathFollower2dState &state, const Ice::Current& )
        {
            // ignore this.
        }
private:
    orcaifaceimpl::AbstractConsumer<orca::PathFollower2dData> &impl_;
};

typedef StoringConsumerImpl<orca::PathFollower2d, orca::PathFollower2dConsumer, 
                            orca::PathFollower2dData, PathFollower2dConsumerTypeI> StoringPathFollower2dConsumerImpl;
typedef IceUtil::Handle<StoringPathFollower2dConsumerImpl> StoringPathFollower2dConsumerImplPtr;

typedef BufferedConsumerImpl<orca::PathFollower2d, orca::PathFollower2dConsumer,
                             orca::PathFollower2dData, PathFollower2dConsumerTypeI> BufferedPathFollower2dConsumerImpl;
typedef IceUtil::Handle<BufferedPathFollower2dConsumerImpl> BufferedPathFollower2dConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::PathFollower2d, orca::PathFollower2dConsumer,
                              orca::PathFollower2dData, PathFollower2dConsumerTypeI> NotifyingPathFollower2dConsumerImpl;
typedef IceUtil::Handle<NotifyingPathFollower2dConsumerImpl> NotifyingPathFollower2dConsumerImplPtr;

typedef PrintingConsumerImpl<orca::PathFollower2d, orca::PathFollower2dConsumer,
                              orca::PathFollower2dData, PathFollower2dConsumerTypeI> PrintingPathFollower2dConsumerImpl;
typedef IceUtil::Handle<PrintingPathFollower2dConsumerImpl> PrintingPathFollower2dConsumerImplPtr;

}

#endif
