/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include "fakedriver.h"

#include <orcaice/orcaice.h>

using namespace std;
using namespace orca;
using namespace teleop;

FakeDriver::FakeDriver( const InputDriver::Config &cfg ) :
        config_(cfg)
{
}

FakeDriver::~FakeDriver()
{
}

int FakeDriver::enable()
{
    cout<<"FakeDriver is enabled"<<endl;
    return 0;
}

int FakeDriver::disable()
{
    cout<<"FakeDriver is disabled"<<endl;
    return 0;
}

int FakeDriver::read( orca::Velocity2dCommand& data )
{
    orcaice::setSane( data );

    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));

    return 0;
}
