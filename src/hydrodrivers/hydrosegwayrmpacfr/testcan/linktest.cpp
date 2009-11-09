/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include "../driver.h"

using namespace segwayrmpacfr;

int main()
{
    std::cout<<"This just exists to test linking -- it doesn't do anything." << std::endl;

    // Force the thing to link, so it's easier to find undefined symbols.
    hydroutil::Context *context = NULL;
    new Driver( "TestPowerbase", *context );

    hydrointerfaces::SegwayRmpFactory* factory = new Factory;
    factory->createDriver( "TestPowerbase", *context );

    return 0;
}
