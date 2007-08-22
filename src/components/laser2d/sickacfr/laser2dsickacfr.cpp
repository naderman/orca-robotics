/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include "sickacfrdriver.h"

using namespace laser2d;

int main()
{
    std::cout<<"This just exists to test linking -- it doesn't do anything." << std::endl;

    // Force the thing to link, so it's easier to find undefined symbols.
    orcaice::Context *context = NULL;
    laser2d::Driver::Config *config = NULL;
    new sickacfr::SickAcfrDriver( *config, *context );

    return 0;
}
