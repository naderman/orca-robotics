#include <iostream>

#include "localise2d_i.h"

using namespace std;
using namespace orca;
using namespace faithlocaliser;

Localise2dI::Localise2dI( const IceStorm::TopicPrx              &localiseTopic,
                          orcaice::PtrBuffer<Localise2dDataPtr> &locBuffer )
    : localiseTopic_(localiseTopic),
      locBuffer_(locBuffer)
{
}

::orca::Localise2dDataPtr 
Localise2dI::getData(const ::Ice::Current& ) const
{
    // create a null pointer. data will be cloned into it.
    Localise2dDataPtr data;

    // we don't need to pop the data here because we don't block on it.
    // we always want to have the latest copy in there
    if ( locBuffer_.isEmpty() )
    {
        cout << "ERROR(localise2d_i.cpp): Buffer was empty!" << endl;
        exit(1);
    }
    locBuffer_.get( data );
    return data;
}

void 
Localise2dI::subscribe(const ::orca::Localise2dConsumerPrx& subscriber, const ::Ice::Current&)
{
    cout<<"subscribe()"<<endl;
    IceStorm::QoS qos;
    localiseTopic_->subscribe( qos, subscriber );
}

void 
Localise2dI::unsubscribe(const ::orca::Localise2dConsumerPrx& subscriber, const ::Ice::Current&)
{
    cout<<"unsubscribe()"<<endl;
    localiseTopic_->unsubscribe( subscriber );
}
