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
#include <orcalog/orcalog.h>

#include "wifilogger.h"

using namespace std;

namespace orcalogfactory {

WifiLogger::WifiLogger( orcalog::LogMaster *master, 
                              const std::string & typeSuffix,
                              const std::string & format,
                              const std::string & filenamePrefix,
                              const orcaice::Context & context )
    : orcalog::Logger( master, 
                      "Wifi",
                      "Wifi"+typeSuffix,
                      format,
                      filenamePrefix+"wifi"+typeSuffix+".log",
                      context )
{   
    // check that we support this format
    if ( format_!="ice" && format_!="ascii" )
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
}

void 
WifiLogger::init()
{      
    // throws orcaice::NetworkException but it's ok to quit here
    orca::WifiPrx objectPrx;
    orcaice::connectToInterfaceWithTag<orca::WifiPrx>( context_, objectPrx, interfaceTag_ );
    
    // consumer
    Ice::ObjectPtr consumer = this;
    orca::WifiConsumerPrx callbackPrx = 
        orcaice::createConsumerInterface<orca::WifiConsumerPrx>( context_, consumer );

    context_.tracer()->debug("Subscribing to IceStorm now.",5);
    objectPrx->subscribe( callbackPrx );
}

void 
WifiLogger::setData(const orca::WifiData& data, const Ice::Current&)
{
    // Write a reference in the master file
    appendMasterFile();
    
    if ( format_=="ice" )
    {
        orcalog::IceWriteHelper helper( context_.communicator() );
        ice_writeWifiData( helper.stream_, data );
        helper.write( file_ );
    }
    else if ( format_=="ascii" )
    {
	   (*file_) << orcalog::toLogString(data) << flush;
    }
    else
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
}

}

