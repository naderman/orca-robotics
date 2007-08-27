/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOGFACTORY_LOCALISE3D_LOGGER_H
#define ORCA2_ORCALOGFACTORY_LOCALISE3D_LOGGER_H

#include <orcaice/context.h>
#include <orca/localise3d.h>
#include <orcalog/logger.h>

namespace orcalogfactory
{

/*!
Supports two formats:
- ice
- ascii: see orcalog::toLogString( const orca::Localise3dData& obj )  for log format
*/
class Localise3dLogger : public orca::Localise3dConsumer, public orcalog::Logger
{
    public:
        Localise3dLogger( orcalog::LogMaster *master, 
                    const std::string & typeSuffix,
                    const std::string & format,
                    const std::string & filenamePrefix,
                    const orcaice::Context & context );
        virtual ~Localise3dLogger();

        // from Logger
        virtual void init();

        // from *Consumer
        virtual void setData(const orca::Localise3dData& data, const Ice::Current&);

    private:
        
        void writeGeometryToFile( const orca::VehicleGeometryDescriptionPtr& obj );

    
};

}
#endif
