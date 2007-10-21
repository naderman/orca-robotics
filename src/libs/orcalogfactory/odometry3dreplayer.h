/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOGFACTORY_ODOMETRY3D_REPLAYER_H
#define ORCA2_ORCALOGFACTORY_ODOMETRY3D_REPLAYER_H

#include <IceStorm/IceStorm.h>
#include <orca/odometry3d.h>
#include <orcalog/replayer.h>

#include <hydroutil/proxy.h>

namespace orcalogfactory
{

class Odometry3dReplayer : public orca::Odometry3d, public orcalog::Replayer
{
public:
    Odometry3dReplayer( const std::string      &format,
                        const std::string      &filename,
                        const orcaice::Context &context );
    virtual ~Odometry3dReplayer();
    
    // from Orca object

    virtual ::orca::Odometry3dData getData(const ::Ice::Current& ) const;

    virtual ::orca::VehicleDescription getDescription(const ::Ice::Current& ) const;
    
    virtual void subscribe(const ::orca::Odometry3dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::Odometry3dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // from Replayer

    virtual void replayData( int index, bool isTest=false );

private:
    hydroutil::Proxy<orca::Odometry3dData> dataPipe_;
    
    orca::Odometry3dData data_;
    orca::VehicleDescription descr_;
    orca::Odometry3dConsumerPrx publisher_;
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
