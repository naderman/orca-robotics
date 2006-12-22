/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOGFACTORY_POWER_LOGGER_H
#define ORCA2_ORCALOGFACTORY_POWER_LOGGER_H

#include <orcaice/context.h>
#include <orca/power.h>
#include <orcalog/logger.h>

namespace orcalogfactory
{

class PowerLogger : public orca::PowerConsumer, public orcalog::Logger
{
    public:
        PowerLogger( orcalog::LogMaster *master, 
                    const std::string & typeSuffix,
                    const std::string & format,
                    const std::string & filenamePrefix,
                    const orcaice::Context & context );
        virtual ~PowerLogger();
        
        // from Logger
        virtual void init();

        // from *Consumer
        virtual void setData(const orca::PowerData & data, const Ice::Current&);
};

}

#endif
