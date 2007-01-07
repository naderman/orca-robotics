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

#include "laserscanner2dlogger.h"

using namespace std;
using namespace orcalogfactory;


LaserScanner2dLogger::LaserScanner2dLogger( orcalog::LogMaster *master, 
                        const std::string & typeSuffix,
                        const std::string & format,
                        const std::string & filenamePrefix,
                        const orcaice::Context & context )
    : orcalog::Logger( master, 
             "LaserScanner2d",
             "LaserScanner2d"+typeSuffix,
             format,
             filenamePrefix+"laserscanner2d"+typeSuffix+".log",
             context )
{
    // check that we support this format
    if ( format_!="ice" && format_!="ascii" )
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
}

LaserScanner2dLogger::~LaserScanner2dLogger()
{
}

void 
LaserScanner2dLogger::init()
{   
    // throws orcaice::NetworkException but it's ok to quit here
    orca::LaserScanner2dPrx objectPrx;
    orcaice::connectToInterfaceWithTag<orca::LaserScanner2dPrx>( context_, objectPrx, interfaceTag_ );
    
    // get description. should do try/catch
    orca::RangeScanner2dDescription obj = objectPrx->getDescription();
    writeDescription( obj );

    // consumer
    Ice::ObjectPtr consumer = this;
    orca::RangeScanner2dConsumerPrx callbackPrx = 
            orcaice::createConsumerInterface<orca::RangeScanner2dConsumerPrx>( context_, consumer );

    context_.tracer()->debug("Subscribing to IceStorm now.",5);
    objectPrx->subscribe( callbackPrx );
}

void 
LaserScanner2dLogger::setData(const orca::RangeScanner2dDataPtr& rangedata, const Ice::Current&)
{
    // special case: do not copy to other loggers!
    // we assume that the data is really LaserScanner2dDataPtr but it has to be cast
    // @todo: what if it's not the right type?
    orca::LaserScanner2dDataPtr data =  orca::LaserScanner2dDataPtr::dynamicCast( rangedata );

    // Write reference to master file
    appendMasterFile( data->timeStamp.seconds, data->timeStamp.useconds );
    
    if ( format_ == "ice" )
    {
        orcalog::IceWriteHelper helper( context_.communicator() );
        ice_writeRangeScanner2dData( helper.stream_, data );
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
LaserScanner2dLogger::writeDescription( const orca::RangeScanner2dDescription& obj )
{
    context_.tracer()->debug( "Writing description to file", 5 );
    
    if ( format_ == "ice" )
    {
        orcalog::IceWriteHelper helper( context_.communicator() );
        ice_writeRangeScanner2dDescription( helper.stream_, obj );
        helper.write( file_ );  
    }
    else if ( format_ == "ascii" )
    {
       (*file_) << orcaice::toString(obj) << endl;
    }
    else
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
} 
