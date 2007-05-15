/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "localise2dI.h"

using namespace std;
using namespace orca;
using namespace faithlocaliser;
using namespace orcaice;

Localise2dI::Localise2dI( const IceStorm::TopicPrx              &topic,
                          orcaice::Buffer<orca::Localise2dData> &locBuffer,
                          orcaice::Buffer<orca::Localise2dData> &historyBuffer,
                          const orcaice::Context                &context)
    : topic_(topic),
      locBuffer_(locBuffer),
      historyBuffer_(historyBuffer),
      context_(context)
{
}

::orca::Localise2dData
Localise2dI::getData(const ::Ice::Current& ) const
{
    Localise2dData data;

    // we don't need to pop the data here because we don't block on it.
    // we always want to have the latest copy in there
    try
    {
        locBuffer_.get( data );
    }
    catch ( const orcaice::Exception & e )
    {
        throw orca::DataNotExistException( "localisation buffer is not populated yet" );
    }
    
    locBuffer_.get( data );
    return data;
}

::orca::Localise2dData
Localise2dI::getDataAtTime(const orca::Time& timeStamp, const ::Ice::Current& ) const
{
    Localise2dData latestData;

    // we don't need to pop the data here because we don't block on it.
    // we always want to have the latest copy in there
    try
    {
        locBuffer_.get( latestData );
    }
    catch ( const orcaice::Exception & e )
    {
        throw orca::DataNotExistException( "localisation buffer is not populated yet" );
    }

    // the oldest piece of history
    Localise2dData ancientHistory;
    // we need at least two pieces of history
    // i.e. the latest pieces of data and the one before
    if(historyBuffer_.size()<2){
        throw orca::DataNotExistException( "insufficient history (<2)" );
    }

    try
    {
        historyBuffer_.get(ancientHistory);
    }
    catch ( const orcaice::Exception & e )
    {
        throw orca::DataNotExistException( "history buffer empty" );
    }

    // case 1: timestamp outside bounds of history
    if(orcaice::timeDiffAsDouble(ancientHistory.timeStamp, timeStamp) > 0.0 )
    {
	throw orca::DataNotExistException( "insufficient history (bounds)" );
    }

    Localise2dData Data0;
    Localise2dData Data1;

    // case 2: timestamp in future
    if(timeDiffAsDouble(timeStamp, latestData.timeStamp) > 0.0 )
    {
	// remove all but 2 pieces of history
	while(historyBuffer_.size() > 2){
            historyBuffer_.pop();
	}

	// set up the interval
	historyBuffer_.get(Data0,0);
	historyBuffer_.get(Data1,1);
    }else{ //case 3: timestamp within bounds of history
        while(historyBuffer_.size() > 2){
	    Localise2dData data;
            // get second item from the front
            historyBuffer_.get( data, 1 );
            // keep popping until we pass the timestamp
            if(timeDiffAsDouble(timeStamp, data.timeStamp) > 0.0 )
	    {
                historyBuffer_.pop();
            }else{
                break;
            }
	}
        // set up the interval
        historyBuffer_.get(Data0,0);
        historyBuffer_.get(Data1,1);
    }

    // Perform linear interpolation/prediction

    //make a copy
    Localise2dData interpData = Data0;

    //cout << "interpolating between: " << timeAsDouble(Data0.timeStamp) << " and " <<
    //    timeAsDouble(Data1.timeStamp) << " to : " << timeAsDouble(timeStamp) << endl;

    double dt=timeDiffAsDouble(timeStamp,Data0.timeStamp);

    double dt01=timeDiffAsDouble(Data1.timeStamp,Data0.timeStamp);
    double dx01=Data1.hypotheses[0].mean.p.x-Data0.hypotheses[0].mean.p.x;
    double dy01=Data1.hypotheses[0].mean.p.y-Data0.hypotheses[0].mean.p.y;
    double do01=Data1.hypotheses[0].mean.o-Data0.hypotheses[0].mean.o;

    interpData.hypotheses[0].mean.p.x=Data0.hypotheses[0].mean.p.x+dx01/dt01*dt;
    interpData.hypotheses[0].mean.p.y=Data0.hypotheses[0].mean.p.y+dy01/dt01*dt;;
    interpData.hypotheses[0].mean.o=Data0.hypotheses[0].mean.o+do01/dt01*dt;;

    //fix up timestamp
    interpData.timeStamp=timeStamp;

    return interpData;

}

void 
Localise2dI::subscribe(const ::orca::Localise2dConsumerPrx& subscriber, const ::Ice::Current&)
{    
    context_.tracer()->debug( "Localise2dI::subscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    try {
        topic_->subscribeAndGetPublisher( IceStorm::QoS(), subscriber->ice_twoway() );
    }
    catch ( const IceStorm::AlreadySubscribed & e ) {
        std::stringstream ss;
        ss <<"Request for subscribe but this proxy has already been subscribed, so I do nothing: "<< e;
        context_.tracer()->debug( ss.str(), 2 );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"Localise2dI::subscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    } 
}

void 
Localise2dI::unsubscribe(const ::orca::Localise2dConsumerPrx& subscriber, const ::Ice::Current&)
{
    context_.tracer()->debug( "Localise2dIface::unsubscribe(): unsubscriber='"+subscriber->ice_toString()+"'", 4 );
    topic_->unsubscribe( subscriber );
}
