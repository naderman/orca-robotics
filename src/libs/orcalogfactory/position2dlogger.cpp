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

#include "position2dlogger.h"

using namespace std;
using namespace orcalogfactory;

Position2dLogger::Position2dLogger(orcalog::LogMaster *master, 
                        const std::string & typeSuffix,
                        const std::string & format,
                        const std::string & filenamePrefix,
                        const orcaice::Context & context )
    : orcalog::Logger( master, 
             "Position2d",
             "Position2d"+typeSuffix,
             format,
             filenamePrefix+"position2d"+typeSuffix+".log",
             context )
{
    // check that we support this format
    if ( format_!="ice" && format_!="ascii" )
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
}

Position2dLogger::~Position2dLogger()
{
}

void 
Position2dLogger::init()
{   
    // throws orcaice::NetworkException but it's ok to quit here
    orca::Position2dPrx objectPrx;
    orcaice::connectToInterfaceWithTag<orca::Position2dPrx>( context_, objectPrx, interfaceTag_ );
    
    // Config and geometry
    orca::Position2dGeometry geometry = objectPrx->getGeometry();
    writeConfigToFile( geometry );

    // consumer
    Ice::ObjectPtr consumer = this;
    orca::Position2dConsumerPrx callbackPrx = 
        orcaice::createConsumerInterface<orca::Position2dConsumerPrx>( context_, consumer );

    context_.tracer()->debug("Subscribing to IceStorm now.",5);
    objectPrx->subscribe( callbackPrx );
}

void 
Position2dLogger::setData(const orca::Position2dData& data, const Ice::Current&)
{
    // Write reference to master file
    appendMasterFile();
        
    if ( format_ == "ice" )
    {
        orcalog::IceWriteHelper helper( context_.communicator() );
        ice_writePosition2dData( helper.stream_, data );
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

void 
Position2dLogger::writeConfigToFile( const orca::Position2dGeometry& obj )
{
    context_.tracer()->print( "Writing geometry to file" );
    
    if ( format_ == "ice" )
    {
        orcalog::IceWriteHelper helper( context_.communicator() );
        ice_writePosition2dGeometry( helper.stream_, obj );
        helper.write( file_ ); 
    }
    else if ( format_ == "ascii" )
    {
        (*file_) << "Implement streaming Position2dGeometry" << endl;
//         (*file_) << orcaice::toString(obj) << endl;
    }
    else
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
} 

