/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOGFACTORY_LOCALISE2D_LOGGER_H
#define ORCA2_ORCALOGFACTORY_LOCALISE2D_LOGGER_H

#include <orcaice/context.h>
#include <orca/localise2d.h>
#include <orcalog/logger.h>

namespace orcalogfactory
{

class Localise2dLogger : public orca::Localise2dConsumer, public orcalog::Logger
{
    public:
        Localise2dLogger( orcalog::LogMaster *master, 
                    const std::string & typeSuffix,
                    const std::string & format,
                    const std::string & filenamePrefix,
                    const orcaice::Context & context );
        virtual ~Localise2dLogger();

        // from Logger
        virtual void init();

        // from *Consumer
        virtual void setData(const orca::Localise2dDataPtr& data, const Ice::Current&);
};

}
#endif
