#include "fakemaploader.h"
#include <iostream>
#include <orcaice/objutils.h>

using namespace orca;
using namespace std;

void fakeLoadMap( orca::OgMapDataPtr theMap )
{
    orcaice::setSane( theMap );
}

