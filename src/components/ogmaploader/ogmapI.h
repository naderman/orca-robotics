/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef __ogmapI_h__
#define __ogmapI_h__

#include <orca/ogmap.h>

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/ogmap.h>

#include <orcaice/context.h>

namespace ogmaploader {

//
// Implements this interface: Handles all our remote calls.
//
class OgMapI : public orca::OgMap
{
public:

    OgMapI( orca::OgMapDataPtr  theMap,
            const std::string  &tag,
            orcaice::Context    context );

    // Remote calls:

    // Hand out data to people
    orca::OgMapDataPtr getData(const Ice::Current&) const;

    virtual void subscribe(const ::orca::OgMapConsumerPrx&,
                           const Ice::Current&);

    virtual void unsubscribe(const ::orca::OgMapConsumerPrx&,
                             const Ice::Current&);

private:

    // The topic to which we'll publish
    IceStorm::TopicPrx             topicPrx_;
    // The interface to which we'll publish
    orca::OgMapConsumerPrx         consumerPrx_;

    orca::OgMapDataPtr theMap_;

    orcaice::Context context_;
};

}

#endif
