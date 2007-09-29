/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <orcaice/orcaice.h>

#include "component.h"
#include "mainloop.h"

using namespace std;
using namespace simlocaliser;

Component::Component()
    : orcaice::Component( "SimLocaliser" ),
      mainLoop_(0)
{
}

Component::~Component()
{
}

void
Component::start()
{
    //
    // MAIN DRIVER LOOP
    //
    mainLoop_ = new MainLoop( context() );
    mainLoop_->start();    
}

void
Component::stop()
{
    orcaiceutil::stopAndJoin( mainLoop_ );
}
