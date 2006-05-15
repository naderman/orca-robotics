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

#include <iostream>
#include <orcaice/objutils.h>

#include "localise2dI.h"

using namespace std;
using namespace orca;
using namespace faithlocaliser;
using namespace orcaice;

Localise2dI::Localise2dI( const IceStorm::TopicPrx              &localiseTopic,
                          orcaice::PtrBuffer<Localise2dDataPtr> &locBuffer,
                          orcaice::PtrBuffer<Localise2dDataPtr> &historyBuffer )
    : localiseTopic_(localiseTopic),
      locBuffer_(locBuffer),
      historyBuffer_(historyBuffer)
{
}

::orca::Localise2dDataPtr 
Localise2dI::getData(const ::Ice::Current& ) const
{
    // create a null pointer. data will be cloned into it.
    Localise2dDataPtr data;

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

::orca::Localise2dDataPtr
Localise2dI::getDataAtTime(const orca::Time& timeStamp, const ::Ice::Current& ) const
{
    // create a null pointer. data will be cloned into it.
    Localise2dDataPtr latestData;

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
    Localise2dDataPtr ancientHistory;
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
    if(timeDiffAsDouble(ancientHistory->timeStamp, timeStamp) > 0.0 )
    {
	throw orca::DataNotExistException( "insufficient history (bounds)" );
        return NULL;
    }

    Localise2dDataPtr Data0;
    Localise2dDataPtr Data1;

    // case 2: timestamp in future
    if(timeDiffAsDouble(timeStamp, latestData->timeStamp) > 0.0 )
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
	    Localise2dDataPtr data;
            // get second item from the front
            historyBuffer_.get( data, 1 );
            // keep popping until we pass the timestamp
            if(timeDiffAsDouble(timeStamp, data->timeStamp) > 0.0 )
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
    Localise2dDataPtr interpData=Localise2dDataPtr::dynamicCast(Data0->ice_clone());

    //cout << "interpolating between: " << timeAsDouble(Data0->timeStamp) << " and " <<
    //    timeAsDouble(Data1->timeStamp) << " to : " << timeAsDouble(timeStamp) << endl;

    double dt=timeDiffAsDouble(timeStamp,Data0->timeStamp);

    double dt01=timeDiffAsDouble(Data1->timeStamp,Data0->timeStamp);
    double dx01=Data1->hypotheses[0].mean.p.x-Data0->hypotheses[0].mean.p.x;
    double dy01=Data1->hypotheses[0].mean.p.y-Data0->hypotheses[0].mean.p.y;
    double do01=Data1->hypotheses[0].mean.o-Data0->hypotheses[0].mean.o;

    interpData->hypotheses[0].mean.p.x=Data0->hypotheses[0].mean.p.x+dx01/dt01*dt;
    interpData->hypotheses[0].mean.p.y=Data0->hypotheses[0].mean.p.y+dy01/dt01*dt;;
    interpData->hypotheses[0].mean.o=Data0->hypotheses[0].mean.o+do01/dt01*dt;;

    //fix up timestamp
    interpData->timeStamp=timeStamp;

    return interpData;

}

void 
Localise2dI::subscribe(const ::orca::Localise2dConsumerPrx& subscriber, const ::Ice::Current&)
{
//     cout<<"subscribe()"<<endl;
    IceStorm::QoS qos;
    localiseTopic_->subscribe( qos, subscriber );
}

void 
Localise2dI::unsubscribe(const ::orca::Localise2dConsumerPrx& subscriber, const ::Ice::Current&)
{
//     cout<<"unsubscribe()"<<endl;
    localiseTopic_->unsubscribe( subscriber );
}
