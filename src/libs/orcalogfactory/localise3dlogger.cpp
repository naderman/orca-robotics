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

#include "localise3dlogger.h"

using namespace std;
using namespace orcalogfactory;

Localise3dLogger::Localise3dLogger( orcalog::LogMaster *master, 
                        const std::string & typeSuffix,
                        const std::string & format,
                        const std::string & filenamePrefix,
                        const orcaice::Context & context )
    : orcalog::Logger( master, 
             "Localise3d",
             "Localise3d"+typeSuffix,
             format,
             filenamePrefix+"localise3d"+typeSuffix+".log",
             context )
{
    // check that we support this format
    if ( format_!="ice" && format_!="ascii" )
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
}

Localise3dLogger::~Localise3dLogger()
{
}

void 
Localise3dLogger::init()
{   
    // throws orcaice::NetworkException but it's ok to quit here
    orca::Localise3dPrx objectPrx;
    orcaice::connectToInterfaceWithTag<orca::Localise3dPrx>( context_, objectPrx, interfaceTag_ );
    
    // write geometry to file
    writeGeometryToFile( objectPrx->getVehicleGeometry() );

    // consumer
    Ice::ObjectPtr consumer = this;
    orca::Localise3dConsumerPrx callbackPrx = 
        orcaice::createConsumerInterface<orca::Localise3dConsumerPrx>( context_, consumer );

    context_.tracer()->debug("Subscribing to IceStorm now.",5);
    objectPrx->subscribe( callbackPrx );
}

void
Localise3dLogger::writeGeometryToFile( const orca::VehicleGeometryDescriptionPtr& obj )
{
    context_.tracer()->print( "Localise3dLogger: Writing geometry to file" );
    
    if ( format_ == "ice" )
    {
        orcalog::IceWriteHelper helper( context_.communicator() );
        ice_writeVehicleGeometryDescription( helper.stream_, obj );
        helper.write( file_ ); 
    }
    else if ( format_ == "ascii" )
    {
        (*file_) << orcalog::toLogString(obj) << flush;
    }
    else
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
    
}

void 
Localise3dLogger::setData(const orca::Localise3dData& data, const Ice::Current&)
{
    // Write reference to master file
    appendMasterFile();

    if ( format_ == "ice" )
    {
        orcalog::IceWriteHelper helper( context_.communicator() );
        ice_writeLocalise3dData( helper.stream_, data );
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

