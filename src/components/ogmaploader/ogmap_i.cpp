#include "ogmap_i.h"
#include <iostream>
#include <orcaice/objutils.h>

using namespace std;
using namespace orca;
using namespace orcaice;

OgMapI::OgMapI( orca::OgMapDataPtr theMap )
    : theMap_( theMap )
{
}

OgMapDataPtr
OgMapI::getData(const Ice::Current& current) const
{
    std::cout << "getData()" << std::endl;

    if ( theMap_ == 0 )
        throw orca::DataNotExistException("Map not loaded correctly.");
    
    return theMap_;
}

void
OgMapI::subscribe(const ::OgMapConsumerPrx& subscriber,
                  const Ice::Current& current)
{
    cout<<"Got Subscribe() request, but ignoring it since we never publish."<<endl;
}

void
OgMapI::unsubscribe(const ::OgMapConsumerPrx& subscriber,
                    const Ice::Current& current)
{
    cout<<"Got UnSubscribe() request, but ignoring it since we don't subscribe people."<<endl;
}
