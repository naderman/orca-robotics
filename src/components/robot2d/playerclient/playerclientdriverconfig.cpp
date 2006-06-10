/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "playerclientdriverconfig.h"

using namespace std;
using namespace robot2d;

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
robot2d::operator<<( std::ostream &s, const PlayerClientDriverConfig &c )
{
    s << "PlayerClient Driver Configuration Parameters:"                <<endl
      << "\thost:              " << c.host                              <<endl
      << "\tport:              " << c.port;

    return s;
}

void
robot2d::readFromProperties( const orcaice::Context & context, PlayerClientDriverConfig & c )
{
    Ice::PropertiesPtr prop = context.properties();
    std::string prefix = context.tag() + ".Config.PlayerClient.";

    std::string playerHost = orcaice::getPropertyWithDefault( prop, prefix+"Host", "localhost" );
    c.host = playerHost.c_str();
    c.port = orcaice::getPropertyAsIntWithDefault( prop, prefix+"Port", 6665 );
}
