/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PRIVATE_UTILITIES_H
#define ORCA2_PRIVATE_UTILITIES_H

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <string>

#include <orca/orca.h>

//  This file should contain internal functions used inside libOrcaIce
//  Don't make Doxygen tags so these functions are not included in the public documentation.

namespace orcaice
{
    void parseOrcaCommandLineOptions( const Ice::StringSeq & args );

    void setFactoryProperties( Ice::PropertiesPtr &properties, const std::string &compTag );

    void setGlobalProperties( Ice::PropertiesPtr & properties, const std::string & filename );

    void setComponentProperties( Ice::PropertiesPtr & properties, const std::string & filename );

    /*
     *   Sets the ComponentName parameter.
     *   Replaces special platform 'local' with host name.
     *   Combines platform and component names into adapter ID.
     *
     *   The default for IceStorm server location assumes that one is running
     *   on our host at port 10000.
     *
     *   @note Cannot use Current structure because the component is not fully initialized yet.
     */
    orca::FQComponentName parseComponentProperties( const Ice::CommunicatorPtr &communicator,
                                                    const std::string &compTag );

    // Prints out all component properties. Tag is used only for tracing.
    void printComponentProperties( const Ice::PropertiesPtr &properties, const std::string &compTag );

    void printVersion();
    
} // namespace

#endif
