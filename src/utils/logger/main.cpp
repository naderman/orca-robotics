/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaice/application.h>
#include "component.h"

//
// Build the component into a stand-alone application
//
int main(int argc, char * argv[])
{
    logger::Component component( "Logger" );
    orcaice::Application app( component );
    return app.main(argc, argv);
}
