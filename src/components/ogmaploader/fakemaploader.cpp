#include <iostream>
#include <orcaice/orcaice.h>

#include "fakemaploader.h"

using namespace orca;
using namespace std;

namespace ogmaploader {

void fakeLoadMap( orca::OgMapDataPtr theMap )
{
    orcaice::setSane( theMap );
}

}
