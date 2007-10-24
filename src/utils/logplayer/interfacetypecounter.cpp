#include "interfacetypecounter.h"
#include <iostream>

using namespace std;

namespace logplayer {

int
InterfaceTypeCounter::nextAvailableId( std::string interfaceType )
{
    if ( lastHandedOut_.find(interfaceType) == lastHandedOut_.end() )
    {
        // First one.
        lastHandedOut_[interfaceType] = 0;
        return 0;
    }
    else
    {
        int last = lastHandedOut_[interfaceType];
        lastHandedOut_[interfaceType] = last+1;
        return last+1;
    }
}


}

