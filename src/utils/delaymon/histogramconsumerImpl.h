/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef HISTOGRAM_CONSUMER_IMPL_H
#define HISTOGRAM_CONSUMER_IMPL_H

#include <orcaifaceimpl/consumerImpl.h>
#include <orcaifaceutil/laserscanner2d.h>
#include "histogram.h"
#include <iostream>

namespace delaymon
{

// this will work for any interface and data type
template<class ProviderType, class ConsumerType, class ObjectType>
class HistogramConsumerImpl : 
        public orcaifaceimpl::ConsumerImpl<ProviderType,ConsumerType,ObjectType>
{
using orcaifaceimpl::ConsumerImpl<ProviderType,ConsumerType,ObjectType>::context_;
public:
    // start, end, stride in [ms]
    HistogramConsumerImpl( const orcaice::Context &context, int start, int end, int stride, 
                           const std::string& fileprefix, int maxCount=-1 ) :
        orcaifaceimpl::ConsumerImpl<ProviderType,ConsumerType,ObjectType>(context),
        historgram_(start,end,stride,fileprefix),
        dataCounter_(0),
        maxCount_(maxCount)
    {
    }

    virtual void dataEvent( const ObjectType& data ) 
    {
      IceUtil::Time timeStamp = IceUtil::Time::now();
      ++dataCounter_;

      // need at least 2 data points (the 1st one is sent by IceStorm on subscription)
      if ( dataCounter_<3 ) {
          prevTimeStamp_ = timeStamp;
          return;
      }
        
      historgram_.addValue( (timeStamp-prevTimeStamp_).toMilliSeconds() );
      prevTimeStamp_ = timeStamp;

      if ( maxCount_>0 && dataCounter_>maxCount_ )
          context_.shutdown();

      if ( (dataCounter_ % 100) == 0 )
          std::cout<<"processed "<<dataCounter_<<" messages"<<std::endl;
    }
private:

    Histogram historgram_;

    IceUtil::Time prevTimeStamp_;
    int dataCounter_;
    int maxCount_;
};

} // namespace

#endif
