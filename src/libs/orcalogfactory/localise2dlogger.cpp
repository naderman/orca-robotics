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

#include "localise2dlogger.h"

using namespace std;
using namespace orcalogfactory;

Localise2dLogger::Localise2dLogger( orcalog::LogMaster *master, 
                        const std::string & typeSuffix,
                        const std::string & format,
                        const std::string & filenamePrefix,
                        const orcaice::Context & context )
    : orcalog::Logger( master, 
             "Localise2d",
             "Localise2d"+typeSuffix,
             format,
             filenamePrefix+"localise2d"+typeSuffix+".log",
             context )
{
    // check that we support this format
    if ( format_!="ice" && format_!="ascii" )
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
}

Localise2dLogger::~Localise2dLogger()
{
}

void 
Localise2dLogger::init()
{   
    // throws orcaice::NetworkException but it's ok to quit here
    orca::Localise2dPrx objectPrx;
    orcaice::connectToInterfaceWithTag<orca::Localise2dPrx>( context_, objectPrx, interfaceTag_ );
    
    // write geometry to file
    writeGeometryToFile( objectPrx->getVehicleGeometry() );
    
    // consumer
    Ice::ObjectPtr consumer = this;
    orca::Localise2dConsumerPrx callbackPrx = 
        orcaice::createConsumerInterface<orca::Localise2dConsumerPrx>( context_, consumer );

    context_.tracer()->debug("Subscribing to IceStorm now.",5);
    objectPrx->subscribe( callbackPrx );
}


void
Localise2dLogger::writeGeometryToFile( const orca::VehicleGeometryDescriptionPtr& obj )
{
    context_.tracer()->print( "Localise2dLogger: Writing geometry to file" );
    
    if ( format_ == "ice" )
    {
        orcalog::IceWriteHelper helper( context_.communicator() );
        ice_writeVehicleGeometryDescription( helper.stream_, obj );
        helper.write( file_ ); 
    }
    else if ( format_ == "ascii" )
    {
        (*file_) << orcalog::toLogString(obj) << endl;
    }
    else
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
    
}

void 
Localise2dLogger::setData(const orca::Localise2dData& data, const Ice::Current&)
{
    // Write reference to master file
    appendMasterFile();

    if ( format_ == "ice" )
    {
        orcalog::IceWriteHelper helper( context_.communicator() );
        ice_writeLocalise2dData( helper.stream_, data );
        helper.write( file_ );
    }
    else if ( format_ == "ascii" )
    {
        (*file_) << orcalog::toLogString(data) << flush;
    }
    else
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
}

