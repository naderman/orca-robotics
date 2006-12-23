/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOGFACTORY_POSITION2D_REPLAYER_H
#define ORCA2_ORCALOGFACTORY_POSITION2D_REPLAYER_H

#include <IceStorm/IceStorm.h>
#include <orca/position2d.h>
#include <orcalog/replayer.h>

#include <orcaice/buffer.h>

namespace orcalogfactory
{

class Position2dReplayer : public orca::Position2d, public orcalog::Replayer
{
public:
    Position2dReplayer( const std::string      &format,
                        const std::string      &filename,
                        const orcaice::Context &context );
    virtual ~Position2dReplayer();
    
    // from Orca object

    virtual ::orca::Position2dData getData(const ::Ice::Current& ) const;

    virtual ::orca::Position2dGeometry getGeometry(const ::Ice::Current& ) const;
    
    virtual void subscribe(const ::orca::Position2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::Position2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // from Replayer

    virtual void replayData( int index, bool isTest=false );

private:
    orcaice::Buffer<orca::Position2dData> dataPipe_;
    orcaice::Buffer<orca::Position2dGeometry> position2dGeometryBuffer_;
    
    orca::Position2dData data_;
    orca::Position2dConsumerPrx publisher_;
    IceStorm::TopicPrx topic_;

    virtual void initInterfaces();
    virtual void initConfigs();

    // binary file protocol specifics
    void loadDataIce( int index );

    // this is a counter of instaces of this type, used for registered interface name
    static int _counter;
};

} // namespace

#endif
