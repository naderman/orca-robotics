/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOGFACTORY_DRIVEBICYCLE_LOGGER_H
#define ORCA2_ORCALOGFACTORY_DRIVEBICYCLE_LOGGER_H

#include <orcaice/context.h>
#include <orca/drivebicycle.h>
#include <orcalog/logger.h>

namespace orcalogfactory
{
/*!
Supports two formats:
- ice
- ascii: see orcalog::toLogString( const orca::DriveBicycleData& obj ) for log format
*/
class DriveBicycleLogger : public orca::DriveBicycleConsumer, public orcalog::Logger
{
    public:
        DriveBicycleLogger( orcalog::LogMaster *master, 
                    const std::string & typeSuffix,
                    const std::string & format,
                    const std::string & filenamePrefix,
                    const orcaice::Context & context );
        virtual ~DriveBicycleLogger();

        // from Logger
        virtual void init();

        // from *Consumer
        virtual void setData(const orca::DriveBicycleData& data, const Ice::Current&);
        
    private:
        // utility
        void writeDescriptionToFile( const orca::VehicleDescription& obj );
};

}
#endif
