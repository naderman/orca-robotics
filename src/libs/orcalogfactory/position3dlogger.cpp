/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>

#include "position3dlogger.h"

using namespace std;
using namespace orcalogfactory;

Position3dLogger::Position3dLogger(orcalog::LogMaster *master, 
                        const std::string & typeSuffix,
                        const std::string & format,
                        const std::string & filenamePrefix,
                        const orcaice::Context & context )
    : orcalog::Logger( master, 
             "Position3d",
             "Position3d"+typeSuffix,
             format,
             filenamePrefix+"position3d"+typeSuffix+".log",
             context )
{
    // check that we support this format
    if ( format_!="ice" && format_!="ascii" )
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
}

Position3dLogger::~Position3dLogger()
{
}

void 
Position3dLogger::init()
{   
    // throws orcaice::NetworkException but it's ok to quit here
    orca::Position3dPrx objectPrx;
    orcaice::connectToInterfaceWithTag<orca::Position3dPrx>( context_, objectPrx, interfaceTag_ );
    
    // Config and geometry
    orca::Position3dDescription descr = objectPrx->getDescription();
    writeDescriptionToFile( descr );

    // consumer
    Ice::ObjectPtr consumer = this;
    orca::Position3dConsumerPrx callbackPrx = 
        orcaice::createConsumerInterface<orca::Position3dConsumerPrx>( context_, consumer );

    context_.tracer()->debug("Subscribing to IceStorm now.",5);
    objectPrx->subscribe( callbackPrx );
}

void 
Position3dLogger::setData(const orca::Position3dData& data, const Ice::Current&)
{
    // Write reference to master file
    appendMasterFile( data.timeStamp.seconds, data.timeStamp.useconds );
        
    if ( format_ == "ice" )
    {
        orcalog::IceWriteHelper helper( context_.communicator() );
        ice_writePosition3dData( helper.stream_, data );
        helper.write( file_ );
    }
    else if ( format_ == "ascii" )
    {
        // (*file_) << "TODO: Implement streaming Position3dDataPtr" << endl;
        (*file_) << orcalog::toLogString(data) << endl;
    }
    else
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
}

void 
Position3dLogger::writeDescriptionToFile( const orca::Position3dDescription& obj )
{
    context_.tracer()->print( "Writing description to file" );
    
    if ( format_ == "ice" )
    {
        orcalog::IceWriteHelper helper( context_.communicator() );
        ice_writePosition3dDescription( helper.stream_, obj );
        helper.write( file_ ); 
    }
    else if ( format_ == "ascii" )
    {
        (*file_) << "Implement streaming Position3dDescriptionPtr" << endl;
//         (*file_) << orcaice::toString(obj) << endl;
    }
    else
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
} 
