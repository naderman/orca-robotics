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

#include "cpulogger.h"

using namespace std;
using namespace orcalogfactory;


CpuLogger::CpuLogger( orcalog::LogMaster *master, 
                        const std::string & typeSuffix,
                        const std::string & format,
                        const std::string & filenamePrefix,
                        const orcaice::Context & context )
    : orcalog::Logger( master, 
             "Cpu",
             "Cpu"+typeSuffix,
             format,
             filenamePrefix+"cpu"+typeSuffix+".log",
             context )
{
    // check that we support this format
    if ( format_!="ice" && format_!="ascii" )
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
}

CpuLogger::~CpuLogger()
{
}

void 
CpuLogger::init()
{   
    // throws orcaice::NetworkException but it's ok to quit here
    orca::CpuPrx objectPrx;
    orcaice::connectToInterfaceWithTag<orca::CpuPrx>( context_, objectPrx, interfaceTag_ );
    
    // consumer
    Ice::ObjectPtr consumer = this;
    orca::CpuConsumerPrx callbackPrx = orcaice::createConsumerInterface<orca::CpuConsumerPrx>( context_, consumer );

    context_.tracer()->print("Subscribing to IceStorm now.");
    objectPrx->subscribe( callbackPrx );
}

void 
CpuLogger::setData(const orca::CpuData& data, const Ice::Current&)
{
    // Write reference to master file
    appendMasterFile( data.timeStamp.seconds, data.timeStamp.useconds );

    if ( format_ == "ice" )
    {
        orcalog::IceWriteHelper helper( context_.communicator() );
        ice_writeCpuData( helper.stream_, data );
        helper.write( file_ );
    }
    else if ( format_ == "ascii" )
    {
        (*file_) << orcalog::toLogString(data) << endl;
    }
    else
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
}
