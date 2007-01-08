/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOGFACTORY_ODOMETRY2D_LOGGER_H
#define ORCA2_ORCALOGFACTORY_ODOMETRY2D_LOGGER_H

#include <orcaice/context.h>
#include <orca/odometry2d.h>
#include <orcalog/logger.h>

namespace orcalogfactory
{

class Odometry2dLogger : public orca::Odometry2dConsumer, public orcalog::Logger
{
    public:
        Odometry2dLogger( orcalog::LogMaster *master, 
                    const std::string & typeSuffix,
                    const std::string & format,
                    const std::string & filenamePrefix,
                    const orcaice::Context & context );
        virtual ~Odometry2dLogger();

        // from Logger
        virtual void init();

        // from *Consumer
        virtual void setData(const orca::Odometry2dData& data, const Ice::Current&);
        
    private:
        // utility
        void writeDescriptionToFile( const orca::Odometry2dDescription& obj );
};

}
#endif
