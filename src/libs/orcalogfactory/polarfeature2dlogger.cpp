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

#include "polarfeature2dlogger.h"

using namespace std;
using namespace orcalogfactory;


PolarFeature2dLogger::PolarFeature2dLogger( orcalog::LogMaster *master, 
                        const std::string & typeSuffix,
                        const std::string & format,
                        const std::string & filenamePrefix,
                        const orcaice::Context & context )
    : orcalog::Logger( master, 
             "PolarFeature2d",
             "PolarFeature2d"+typeSuffix,
             format,
             filenamePrefix+"polarfeature2d"+typeSuffix+".log",
             context )
{
    // check that we support this format
    if ( format_!="ice" && format_!="ascii" )
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
}

PolarFeature2dLogger::~PolarFeature2dLogger()
{
}

void 
PolarFeature2dLogger::init()
{   
    // throws orcaice::NetworkException but it's ok to quit here
    orca::PolarFeature2dPrx objectPrx;
    orcaice::connectToInterfaceWithTag<orca::PolarFeature2dPrx>( context_, objectPrx, interfaceTag_ );
    
    // consumer
    Ice::ObjectPtr consumer = this;
    orca::PolarFeature2dConsumerPrx callbackPrx = 
        orcaice::createConsumerInterface<orca::PolarFeature2dConsumerPrx>( context_, consumer );

    context_.tracer()->debug("Subscribing to IceStorm now.",5);
    objectPrx->subscribe( callbackPrx );
}

void 
PolarFeature2dLogger::setData(const orca::PolarFeature2dDataPtr& data, const Ice::Current&)
{
    // Write reference to master file
    appendMasterFile( data->timeStamp.seconds, data->timeStamp.useconds );
    
    if ( format_ == "ice" )
    {
        // Write the details to our personal file
        vector<Ice::Byte> byteData;
        Ice::OutputStreamPtr outStreamPtr = Ice::createOutputStream( context_.communicator() );
        ice_writePolarFeature2dData(outStreamPtr, data);
        outStreamPtr->writePendingObjects();
        outStreamPtr->finished(byteData);
        
        size_t length = byteData.size();
        file_->write( (char*)&length, sizeof(size_t) );
        file_->flush();
        file_->write( (char*)&byteData[0], length);
        file_->flush();
        
        ostringstream stream;
        stream << "ByteDataSize: " << byteData.size();
        context_.tracer()->debug( stream.str(), 8 );
    }
    else if ( format_ == "ascii" )
    {
        // (*file_) << orcalog::toLogString(data) << endl;
        context_.tracer()->warning( interfaceType_+"Logger: format not supported: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: format not supported: "+format_ );
        
    }
    else
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
}

// void 
// PolarFeature2dLogger::writeConfigToFile( RangeScanner2dConfigPtr polarFeature2dConfigPtr, orca::PolarFeature2dGeometryPtr polarFeature2dGeometryPtr )
// {
//     context_.tracer()->print( "Writing configuration to file" );
//     
//     if (encodingType_=="ice")
//     {
//         vector<Ice::Byte> byteData;
//         Ice::OutputStreamPtr outStreamConfigPtr = Ice::createOutputStream( context_.communicator() );
//         ice_writeRangeScanner2dConfig(outStreamConfigPtr, polarFeature2dConfigPtr);
//         outStreamConfigPtr->writePendingObjects();
//         outStreamConfigPtr->finished( byteData );
//         size_t length = byteData.size();
//         file_->write( (char*)&length, sizeof(size_t) );
//         file_->flush();
//         file_->write( (char*)&byteData[0], length);
//         file_->flush();      
//     
//         Ice::OutputStreamPtr outStreamGeometryPtr = Ice::createOutputStream( context_.communicator() );
//         byteData.resize(0);
//         ice_writeRangeScanner2dGeometry(outStreamGeometryPtr, polarFeature2dGeometryPtr);
//         outStreamGeometryPtr->writePendingObjects();
//         outStreamGeometryPtr->finished( byteData );
//         size_t length2 = byteData.size();
//         file_->write( (char*)&length2, sizeof(size_t) );
//         file_->flush();
//         file_->write( (char*)&byteData[0], length2); 
//         file_->flush();  
//     }
//     else if (encodingType_=="ascii")
//     {
//        (*file_) << orcaice::toString(polarFeature2dConfigPtr) << endl
//                << orcaice::toString(polarFeature2dGeometryPtr) << endl;
//     }
//     else
//     {
//         context_.tracer()->error( "Unknown encodingType. Quitting" );
//         exit(1);  
//     }
// } 
