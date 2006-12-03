/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOGFACTORY_POSITION2D_LOGGER_H
#define ORCA2_ORCALOGFACTORY_POSITION2D_LOGGER_H

#include <orcaice/context.h>
#include <orca/position2d.h>
#include <orcalog/logger.h>

namespace orcalogfactory
{

class Position2dLogger : public orca::Position2dConsumer, public orcalog::Logger
{
    public:
        Position2dLogger( orcalog::LogMaster *master, 
                    const std::string & typeSuffix,
                    const std::string & format,
                    const std::string & filenamePrefix,
                    const orcaice::Context & context );
        virtual ~Position2dLogger();

        // from Logger
        virtual void init();

        // from *Consumer
        virtual void setData(const orca::Position2dDataPtr& data, const Ice::Current&);
        
    private:
        // utility
        void writeConfigToFile( const orca::Position2dGeometryPtr & obj );
};

}
#endif
