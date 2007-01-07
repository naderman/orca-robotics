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
#include <orcaice/orcaice.h>

#include "playerclientdriverconfig.h"

using namespace std;
using namespace segwayrmp;

PlayerClientDriverConfig::PlayerClientDriverConfig()
{
    host            = "localhost";
    port            = 6665;
}

int
PlayerClientDriverConfig::checkSanity( std::string &warnings, std::string &errors )
{
    std::stringstream ssWarn, ssErr;

    warnings = ssWarn.str();
    errors   = ssErr.str();

    if ( warnings == "" && errors == "" ) return 0;
    return -1;
}

std::ostream &
segwayrmp::operator<<( std::ostream &s, const PlayerClientDriverConfig &c )
{
    s << "PlayerClient Driver Configuration Parameters:"                <<endl
      << "\thost:              " << c.host                              <<endl
      << "\tport:              " << c.port;

    return s;
}

void
segwayrmp::readFromProperties( const orcaice::Context & context, PlayerClientDriverConfig & c )
{
    Ice::PropertiesPtr prop = context.properties();
    std::string prefix = context.tag() + ".Config.PlayerClient.";

    std::string playerHost = orcaice::getPropertyWithDefault( prop, prefix+"Host", "localhost" );
    c.host = playerHost.c_str();
    c.port = orcaice::getPropertyAsIntWithDefault( prop, prefix+"Port", 6665 );
}
