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

#include "powerlogger.h"

using namespace std;
using namespace orcalogfactory;


PowerLogger::PowerLogger( orcalog::LogMaster *master, 
                        const std::string & typeSuffix,
                        const std::string & format,
                        const std::string & filenamePrefix,
                        const orcaice::Context & context )
    : orcalog::Logger( master, 
             "Power",
             "Power"+typeSuffix,
             format,
             filenamePrefix+"power"+typeSuffix+".log",
             context )
{
    // check that we support this format
    if ( format_!="ice" && format_!="ascii" )
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
}

PowerLogger::~PowerLogger()
{
}

void 
PowerLogger::init()
{   
    // throws orcaice::NetworkException but it's ok to quit here
    orca::PowerPrx objectPrx;
    orcaice::connectToInterfaceWithTag<orca::PowerPrx>( context_, objectPrx, interfaceTag_ );
    
    // consumer
    Ice::ObjectPtr consumer = this;
    orca::PowerConsumerPrx callbackPrx = 
        orcaice::createConsumerInterface<orca::PowerConsumerPrx>( context_, consumer );

    context_.tracer()->debug("Subscribing to IceStorm now.",5);
    objectPrx->subscribe( callbackPrx );
}

void 
PowerLogger::setData(const orca::PowerData & data, const Ice::Current&)
{
    // Write reference to master file
    appendMasterFile( data.timeStamp.seconds, data.timeStamp.useconds );
    
    if ( format_ == "ice" )
    {
        orcalog::IceWriteHelper helper( context_.communicator() );
        ice_writePowerData( helper.stream_, data );
        helper.write( file_ );
    }
    else if ( format_ == "ascii" )
    {
//         cout<<"writing line :"<<orcalog::toLogString(data)<<endl;
        (*file_) << orcalog::toLogString(data) << endl;
    }
    else
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
}
