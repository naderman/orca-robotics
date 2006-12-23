/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOGFACTORY_LASERSCANNER2D_LOGGER_H
#define ORCA2_ORCALOGFACTORY_LASERSCANNER2D_LOGGER_H

#include <orcaice/context.h>
#include <orca/laserscanner2d.h>
#include <orcalog/logger.h>

namespace orcalogfactory
{

class LaserScanner2dLogger : public orca::RangeScanner2dConsumer, public orcalog::Logger
{
    public:
        LaserScanner2dLogger( orcalog::LogMaster *master, 
                    const std::string & typeSuffix,
                    const std::string & format,
                    const std::string & filenamePrefix,
                    const orcaice::Context & context );
        virtual ~LaserScanner2dLogger();

        // from Logger
        virtual void init();

        // from *Consumer
        virtual void setData(const orca::RangeScanner2dDataPtr& data, const Ice::Current&);

    private:
        // utility
        void writeDescription( const orca::RangeScanner2dDescription& obj );
};

} // namespace

#endif
