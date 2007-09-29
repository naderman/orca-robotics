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
#include <orcaogfusion/orcaogfusion.h>

#include "component.h"
#include "handler.h"
#include "ogfusionI.h"

using namespace std;

namespace ognode {

Component::Component()
    : orcaice::Component( "OgNode", orcaice::HomeInterface  )
{
}

Component::~Component()
{
}

void
Component::start()
{
    handler_ = new Handler( context() );
    handler_->start();
}

void
Component::stop()
{
    orcaiceutil::stopAndJoin( handler_ );
}

}
