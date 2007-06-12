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

#include "drivebicyclelogger.h"

using namespace std;
using namespace orcalogfactory;

DriveBicycleLogger::DriveBicycleLogger(orcalog::LogMaster *master, 
                        const std::string & typeSuffix,
                        const std::string & format,
                        const std::string & filenamePrefix,
                        const orcaice::Context & context )
    : orcalog::Logger( master, 
             "DriveBicycle",
             "DriveBicycle"+typeSuffix,
             format,
             filenamePrefix+"drivebicycle"+typeSuffix+".log",
             context )
{
    // check that we support this format
    if ( format_!="ice" && format_!="ascii" )
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
}

DriveBicycleLogger::~DriveBicycleLogger()
{
}

void 
DriveBicycleLogger::init()
{   
    // throws orcaice::NetworkException but it's ok to quit here
    orca::DriveBicyclePrx objectPrx;
    orcaice::connectToInterfaceWithTag<orca::DriveBicyclePrx>( context_, objectPrx, interfaceTag_ );
    
    // Config and geometry
    writeDescriptionToFile( objectPrx->getDescription() );

    // consumer
    Ice::ObjectPtr consumer = this;
    orca::DriveBicycleConsumerPrx callbackPrx = 
        orcaice::createConsumerInterface<orca::DriveBicycleConsumerPrx>( context_, consumer );

    context_.tracer()->debug("Subscribing to IceStorm now.",5);
    objectPrx->subscribe( callbackPrx );
}

void 
DriveBicycleLogger::setData(const orca::DriveBicycleData& data, const Ice::Current&)
{
    // Write reference to master file
    appendMasterFile();
        
    if ( format_ == "ice" )
    {
        orcalog::IceWriteHelper helper( context_.communicator() );
        ice_writeDriveBicycleData( helper.stream_, data );
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
DriveBicycleLogger::writeDescriptionToFile( const orca::VehicleDescription& obj )
{
    context_.tracer()->print( "DriveBicycleLogger: Writing description to file" );
    
    if ( format_ == "ice" )
    {
        orcalog::IceWriteHelper helper( context_.communicator() );
        ice_writeVehicleDescription( helper.stream_, obj );
        helper.write( file_ ); 
    }
    else if ( format_ == "ascii" )
    {
        (*file_) << "Implement streaming VehicleDescription" << endl;
//         (*file_) << orcaice::toString(obj) << endl;
    }
    else
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
} 
