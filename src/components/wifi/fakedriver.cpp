/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */


#include <orcaice/orcaice.h>
#include <orcaobj/wifi.h>
#include "fakedriver.h"


namespace wifi
{

void FakeDriver::read( orca::WifiData &data )
{
    data.timeStamp = orcaice::getNow();
    
    orca::WifiInterface w;
    orcaobj::setInit(w);

    // Set some sane values
    w.interfaceName = "fake";
    w.linkQuality   = 60;
    w.signalLevel = -75;
    w.noiseLevel  = -125;

    data.interfaces.push_back( w );
}

}
