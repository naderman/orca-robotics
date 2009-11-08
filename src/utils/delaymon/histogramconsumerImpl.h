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

#include <orcaice/timeutils.h>
#include <orcaifaceimpl/consumerImpl.h>
#include <orcaifaceutil/laserscanner2d.h>
#include "histogram.h"
#include "histogram2d.h"
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
                        const std::string& fileprefix, int maxCount=-1, int sendWarningMs = 1000 ) :
        orcaifaceimpl::ConsumerImpl<ProviderType,ConsumerType,ObjectType>(context),
        receiveHistogram_(start,end,stride,fileprefix+"_rec"),
        sendHistogram_(start,end,stride,fileprefix+"_send"),
        sendRecHistogram_(start,end,stride,fileprefix),
        dataCounter_(0),
        maxCount_(maxCount),
        sendWarningMs_(sendWarningMs)
    {
    }

    virtual void dataEvent( const ObjectType& data ) 
    {
        IceUtil::Time recCurTimeStamp = IceUtil::Time::now();
        ++dataCounter_;
    
        // need at least 2 data points (the 1st one is sent by IceStorm on subscription)
        if ( dataCounter_<3 ) {
            sendPrevTimeStamp_ = orcaice::toIceTime(data->timeStamp);
            recPrevTimeStamp_ = recCurTimeStamp;
            return;
        }
    
        // Receive histogram
        int recDiffMs = (recCurTimeStamp-recPrevTimeStamp_).toMilliSeconds();
        receiveHistogram_.addValue( recDiffMs );
        recPrevTimeStamp_ = recCurTimeStamp;
    
        // Send histogram
        IceUtil::Time sendCurTimestamp = orcaice::toIceTime(data->timeStamp);
        int sendDiffMs = (sendCurTimestamp-sendPrevTimeStamp_).toMilliSeconds();
        if (sendDiffMs > sendWarningMs_) {
            std::cout << "Time difference of send msg: " << sendDiffMs << "ms. Warning threshhold is " << sendWarningMs_ << "ms." << std::endl;
            std::cout << "Timestamp of send msg: " << sendCurTimestamp.toDateTime() << std::endl;
        }
        sendHistogram_.addValue( sendDiffMs );
        sendPrevTimeStamp_ = orcaice::toIceTime(data->timeStamp);

        // 2d histogram
        sendRecHistogram_.addValue( sendDiffMs, recDiffMs );
        
        if ( maxCount_>0 && dataCounter_>maxCount_ )
            context_.shutdown();
    
        if ( (dataCounter_ % 100) == 0 )
            std::cout<<"processed "<<dataCounter_<<" messages"<<std::endl;
    }

private:
    
    Histogram receiveHistogram_;
    Histogram sendHistogram_;
    Histogram2d sendRecHistogram_;
    
    IceUtil::Time recPrevTimeStamp_;
    IceUtil::Time sendPrevTimeStamp_;
    int dataCounter_;
    int maxCount_;
    int sendWarningMs_;
};

} // namespace

#endif
