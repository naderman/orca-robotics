/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOGFACTORY_GPS_LOGGER_H
#define ORCA2_ORCALOGFACTORY_GPS_LOGGER_H

#include <orcaice/context.h>
#include <orca/gps.h>
#include <orcalog/logger.h>
#include "gpsconsumerI.h"

namespace orcalogfactory
{

class GpsLogger : public orcalog::Logger
{
    public:
        GpsLogger( orcalog::LogMaster *master, 
                    const std::string & typeSuffix,
                    const std::string & format,
                    const std::string & filenamePrefix,
                    const orcaice::Context & context );
        virtual ~GpsLogger();

        // from Slave
        virtual void init();

        // these are called from consumers
        void localSetData( const orca::GpsData& data );
        void localSetData( const orca::GpsTimeData& data );
        void localSetData( const orca::GpsMapGridData& data );
        
    private:
        // utility
        void writeDescriptionToFile( const orca::GpsDescription& descr );

        // these are the actual consumer object, they must be members to stay alive
        Ice::ObjectPtr consumerGps_;
        Ice::ObjectPtr consumerTime_;
        Ice::ObjectPtr consumerMap_;
};

}

#endif
