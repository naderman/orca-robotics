#include "featuremap2dI.h"
#include <iostream>
#include <orcaice/objutils.h>

using namespace std;
using namespace orca;
using namespace orcaice;

FeatureMap2dI::FeatureMap2dI( orca::FeatureMap2dDataPtr theMap )
    : theMap_( theMap )
{
    cout<<"TRACE(featuremap2d_i.cpp): Instantiated map interface with features:" << endl;
    cout<<theMap_<<endl;
}

FeatureMap2dDataPtr
FeatureMap2dI::getData(const Ice::Current& current) const
{
    std::cout << "getData()" << std::endl;
    return theMap_;
}

void
FeatureMap2dI::subscribe(const ::FeatureMap2dConsumerPrx& subscriber,
                         const Ice::Current& current)
{
    cout<<"Got Subscribe() request, but ignoring it since we never publish."<<endl;
}

void
FeatureMap2dI::unsubscribe(const ::FeatureMap2dConsumerPrx& subscriber,
                           const Ice::Current& current)
{
    cout<<"Got UnSubscribe() request, but ignoring it since we don't subscribe people."<<endl;
}
