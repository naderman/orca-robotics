/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/application.h>
#include "component.h"

using namespace std;

//
// Build the component into a stand-alone application
//
int main(int argc, char * argv[])
{
    replier::Component component;
    orcaice::Application app( component );
    return app.main(argc, argv);
}
