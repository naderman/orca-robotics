/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */


#include <orcaice/orcaice.h>
#include <orcaobj/initutils.h>
#include "fakedriver.h"


namespace wifi
{

void FakeDriver::read( orca::WifiData &data )
{
    data.timeStamp = orcaice::getNow();
    
    orca::WifiInterface w;
    orcaobj::setInit(w);
    data.interfaces.push_back( w );
}

}
