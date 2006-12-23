/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOGFACTORY_POSITION3D_LOGGER_H
#define ORCA2_ORCALOGFACTORY_POSITION3D_LOGGER_H

#include <orcaice/context.h>
#include <orca/position3d.h>
#include <orcalog/logger.h>

namespace orcalogfactory
{

class Position3dLogger : public orca::Position3dConsumer, public orcalog::Logger
{
    public:
        Position3dLogger( orcalog::LogMaster *master, 
                    const std::string & typeSuffix,
                    const std::string & format,
                    const std::string & filenamePrefix,
                    const orcaice::Context & context );
        virtual ~Position3dLogger();

        // from Logger
        virtual void init();

        // from *Consumer
        virtual void setData(const orca::Position3dData& data, const Ice::Current&);

    private:
        // utility
        void writeDescriptionToFile( const orca::Position3dDescription& obj );
};

}
#endif
