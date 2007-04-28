/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "featuremap2dI.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>

using namespace std;
using namespace orca;
using namespace orcaice;

namespace featuremaploader {

FeatureMap2dI::FeatureMap2dI( orca::FeatureMap2dDataPtr theMap,
                              const std::string  &tag,
                              const orcaice::Context & context )
    : theMap_( theMap ),
      context_(context)
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<FeatureMap2dConsumerPrx>
        ( context_, consumerPrx_, tag );

    // Try to push the map out to IceStorm first
    try {
        context_.tracer()->debug( "Pushing to IceStorm", 2 );
        consumerPrx_->setData( theMap );
    }
    catch ( Ice::ConnectionRefusedException &e )
    {
        // This could happen if IceStorm dies.
        // If we're running in an IceBox and the IceBox is shutting down, 
        // this is expected (our co-located IceStorm is obviously going down).
        context_.tracer()->warning( "Failed push to IceStorm." );
    }
    cout<<"TRACE(featuremap2d_i.cpp): Instantiated map interface with features:" << endl;

    cout<<orcaice::toString(theMap_)<<endl;
}

FeatureMap2dDataPtr
FeatureMap2dI::getData(const Ice::Current& current) const
{
    if ( theMap_ == 0 )
        throw orca::DataNotExistException("Map not loaded correctly.");

    std::cout << "getData()" << std::endl;
    return theMap_;
}

void
FeatureMap2dI::subscribe(const ::FeatureMap2dConsumerPrx& subscriber,
                         const Ice::Current& current)
{
    cout<<"TRACE(featuremap2dI.cpp): subscribe()" << endl;
    topicPrx_->subscribeAndGetPublisher( IceStorm::QoS(), subscriber->ice_twoway());
}

void
FeatureMap2dI::unsubscribe(const ::FeatureMap2dConsumerPrx& subscriber,
                           const Ice::Current& current)
{
    cout<<"TRACE(featuremap2dI.cpp): unsubscribe()" << endl;

    topicPrx_->unsubscribe( subscriber );
}

}
