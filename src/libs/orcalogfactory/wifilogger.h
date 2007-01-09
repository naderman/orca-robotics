/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOGFACTORY_WIFI_LOGGER_H
#define ORCA2_ORCALOGFACTORY_WIFI_LOGGER_H

#include <orcaice/context.h>
#include <orca/wifi.h>
#include <orcalog/logger.h>

namespace orcalogfactory
{

class WifiLogger : public orca::WifiConsumer, public orcalog::Logger
{
public:
    WifiLogger( orcalog::LogMaster     *master, 
                   const std::string      &typeSuffix,
                   const std::string      &format,
                   const std::string      &filenamePrefix,
                   const orcaice::Context &context );

    // from Slave
    virtual void init();

    // from *Consumer
    virtual void setData(const orca::WifiData& data, const Ice::Current&);

private:

};

}

#endif
