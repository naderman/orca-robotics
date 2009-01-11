/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include "connectutils.h"
#include "configutils.h"
#include "exceptions.h"

using namespace std;

namespace orcaice {

std::string 
getInterfaceIdWithString( const Context& context, const std::string& proxyString )
{
    // for indirect proxies only: if platform is set to 'local', replace it by hostname
    std::string resolvedProxyString = resolveLocalPlatform( context, proxyString );

    Ice::ObjectPrx prx = context.communicator()->stringToProxy( resolvedProxyString );

    // check with the server that the one we found is of the right type
    // the check operation is remote and may fail (see sec.6.11.2)
    try {
        return prx->ice_id();
    }
    // typically we get ConnectionRefusedException, but it may be ObjectNotExistException
    catch ( Ice::LocalException &e )
    {
        // Give some feedback as to why shit isn't working
        std::stringstream ss;
        ss << "Failed to lookup ID of '" << proxyString << "': "<<e;
        initTracerWarning( context, ss.str(), 2 );
        throw orcaice::NetworkException( ERROR_INFO, ss.str() );
    }
}

std::string 
getInterfaceIdWithTag( const Context& context, const std::string& interfaceTag )
{
    // this may throw ConfigFileException, we don't catch it, let the user catch it at the component level
    std::string proxyString = orcaice::getRequiredInterfaceAsString( context, interfaceTag );

    // now that we have the stingified proxy, use the function above.
    return getInterfaceIdWithString( context, proxyString );
}

bool 
isInterfaceReachable( const Context& context, const std::string& proxyString, std::string& diagnostic )
{
    try {
        Ice::ObjectPrx obj = context.communicator()->stringToProxy( proxyString );
        obj->ice_ping();
    }
    catch ( const Ice::Exception &e )
    {
        stringstream ss; 
        ss << e;
        diagnostic = ss.str();
        return false;
    }

    return true;
}

} // namespace
