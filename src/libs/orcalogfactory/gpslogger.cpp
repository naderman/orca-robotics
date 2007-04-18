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

#include "gpslogger.h"

using namespace std;
using namespace orcalogfactory;


GpsLogger::GpsLogger( orcalog::LogMaster *master, 
                      const std::string & typeSuffix,
                      const std::string & format,
                      const std::string & filenamePrefix,
                      const orcaice::Context & context )
    : orcalog::Logger( master, 
             "Gps",
             "Gps"+typeSuffix,
             format,
             filenamePrefix+"gps"+typeSuffix+".log",
             context )
{
    // check that we support this format
    if ( (format_!="ice") && (format_!="ascii") )
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
}

GpsLogger::~GpsLogger()
{
}

void 
GpsLogger::init()
{ 
    // throws orcaice::NetworkException but it's ok to quit here
    orca::GpsPrx objectPrx;
    orcaice::connectToInterfaceWithTag<orca::GpsPrx>( context_, objectPrx, interfaceTag_ );

    // Config and geometry
    orca::GpsDescription descr = objectPrx->getDescription();
    writeDescriptionToFile( descr );
    
    // data consumer
    consumerGps_ = new GpsConsumerI(this);
    orca::GpsConsumerPrx callbackGpsPrx = 
        orcaice::createConsumerInterface<orca::GpsConsumerPrx>( context_, consumerGps_ );
    objectPrx->subscribe( callbackGpsPrx );
}

void 
GpsLogger::writeDescriptionToFile( const orca::GpsDescription& obj )
{
    context_.tracer()->print( "GpsLogger: Writing description to file" );

    if ( format_=="ice" )
    {
        orcalog::IceWriteHelper helper( context_.communicator() );
        ice_writeGpsDescription( helper.stream_, obj );
        helper.write( file_ );  
    }
    else if ( format_=="ascii" )
    {
        (*file_) << orcalog::toLogString(obj) << endl;
    }
    else
        assert( false && "unknown format" );
}

void 
GpsLogger::localSetData( const orca::GpsData& data )
{
    // Write reference to master file
    appendMasterFile();
    if ( format_=="ice" )
    {
        orcalog::IceWriteHelper helper( context_.communicator() );
        ice_writeGpsData( helper.stream_, data );
        helper.write( file_, 0 );
    }
    else if ( format_ == "ascii" )
    {
        (*file_) << orcalog::toLogString(data) << endl;
    }
    else
        assert( false && "Unknown format." );
}
