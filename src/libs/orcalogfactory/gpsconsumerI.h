/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOGFACTORY_GPS_CONSUMER_I_H
#define ORCA2_ORCALOGFACTORY_GPS_CONSUMER_I_H

#include <orca/gps.h>

namespace orcalogfactory
{

class GpsLogger;

class GpsConsumerI : public orca::GpsConsumer
{
    public:
        GpsConsumerI ( GpsLogger *logger )
            :logger_(logger)
        {};
        virtual void setData(const orca::GpsData& data, const Ice::Current&);
        
    private:
        GpsLogger *logger_;
    
};

class GpsTimeConsumerI : public orca::GpsTimeConsumer
{
    public:
        GpsTimeConsumerI ( GpsLogger *logger )
            :logger_(logger)
        {};
        virtual void setData(const orca::GpsTimeData& data, const Ice::Current&);
        
    private:
        GpsLogger *logger_;
    
};

class GpsMapGridConsumerI : public orca::GpsMapGridConsumer
{
    public:
        GpsMapGridConsumerI ( GpsLogger *logger )
            :logger_(logger)
        {};
        virtual void setData(const orca::GpsMapGridData& data, const Ice::Current&);
        
    private:
        GpsLogger *logger_;
    
};

}

#endif
