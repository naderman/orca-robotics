/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOGFACTORY_ODOMETRY2D_REPLAYER_H
#define ORCA2_ORCALOGFACTORY_ODOMETRY2D_REPLAYER_H

#include <IceStorm/IceStorm.h>
#include <orca/odometry2d.h>
#include <orcalog/replayer.h>

#include <orcaice/buffer.h>

namespace orcalogfactory
{

class Odometry2dReplayer : public orca::Odometry2d, public orcalog::Replayer
{
public:
    Odometry2dReplayer( const std::string      &format,
                        const std::string      &filename,
                        const orcaice::Context &context );
    virtual ~Odometry2dReplayer();
    
    // from Orca object

    virtual ::orca::Odometry2dData getData(const ::Ice::Current& ) const;

    virtual ::orca::VehicleDescription getDescription(const ::Ice::Current& ) const;
    
    virtual void subscribe(const ::orca::Odometry2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::Odometry2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // from Replayer

    virtual void replayData( int index, bool isTest=false );

private:
    orcaice::Buffer<orca::Odometry2dData> dataPipe_;
    
    orca::Odometry2dData data_;
    orca::VehicleDescription descr_;
    orca::Odometry2dConsumerPrx publisher_;
    IceStorm::TopicPrx topicPrx_;

    virtual void initInterfaces();
    virtual void initDescription();

    // binary file protocol specifics
    void loadDataIce( int index );

    // this is a counter of instaces of this type, used for registered interface name
    static int _counter;
};

} // namespace

#endif
