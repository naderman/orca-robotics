/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "component.h"
#include <orcaice/application.h>

//
// Build the component into a stand-alone application
//
int
main(int argc, char * argv[])
{
    pingserver::Component component;
    bool installCtrlCMainThread = false;
    orcaice::Application app( component, installCtrlCMainThread );
    return app.main(argc, argv);
}
