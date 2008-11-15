/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include "consumerImpl.h"

using namespace orcaifaceimpl;

ConsumerSubscriber::ConsumerSubscriber( const orcaice::Context& context ) :
    context_(context) 
{
}

void 
ConsumerSubscriber::subscribeWithTag( const std::string& interfaceTag )
{
    // this may throw ConfigFileException, we don't catch it, let the user catch it at the component level
    std::string proxyString = orcaice::getRequiredInterfaceAsString( context_, interfaceTag );

    // now that we have the stingified proxy, use the function above.
    subscribeWithString( proxyString );
}

bool 
ConsumerSubscriber::subscribeWithTag( const std::string& interfaceTag, 
                        gbxiceutilacfr::Thread*  thread, const std::string& subsysName, 
                        int retryInterval, int retryNumber )
{
    // this may throw ConfigFileException, we don't catch it, let the user catch it at the component level
    std::string proxyString = orcaice::getRequiredInterfaceAsString( context_, interfaceTag );

    // now that we have the stingified proxy, use the function above.
    return subscribeWithString( proxyString, thread, subsysName, retryInterval, retryNumber );
}

