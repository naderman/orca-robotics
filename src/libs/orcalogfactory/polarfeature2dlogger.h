/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOGFACTORY_POLARFEATURE2D_LOGGER_H
#define ORCA2_ORCALOGFACTORY_POLARFEATURE2D_LOGGER_H

#include <orcaice/context.h>
#include <orca/polarfeature2d.h>
#include <orcalog/logger.h>

namespace orcalogfactory
{

class PolarFeature2dLogger : public orca::PolarFeature2dConsumer, public orcalog::Logger
{
    public:
        PolarFeature2dLogger( orcalog::LogMaster *master, 
                    const std::string & typeSuffix,
                    const std::string & format,
                    const std::string & filenamePrefix,
                    const orcaice::Context & context );
        virtual ~PolarFeature2dLogger();

        // from Logger
        virtual void init();

        // from *Consumer
        virtual void setData(const orca::PolarFeature2dDataPtr& data, const Ice::Current&);
};

}

#endif
