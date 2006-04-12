#include "fakemaploader.h"
#include <iostream>
#include <orcaice/objutils.h>

using namespace orca;
using namespace std;

namespace ogmaploader {

void fakeLoadMap( orca::OgMapDataPtr theMap )
{
    orcaice::setSane( theMap );
}

}
