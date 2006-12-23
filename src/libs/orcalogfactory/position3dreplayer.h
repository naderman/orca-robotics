/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOGFACTORY_POSITION3D_REPLAYER_H
#define ORCA2_ORCALOGFACTORY_POSITION3D_REPLAYER_H

#include <IceStorm/IceStorm.h>
#include <orca/position3d.h>
#include <orcalog/replayer.h>

#include <orcaice/buffer.h>

namespace orcalogfactory
{

class Position3dReplayer : public orca::Position3d, public orcalog::Replayer
{
public:

    Position3dReplayer( const std::string      &format,
                        const std::string      &filename,
                        const orcaice::Context &context );
    virtual ~Position3dReplayer();
    
    // from Orca object

    virtual ::orca::Position3dData getData(const ::Ice::Current& ) const;

    virtual ::orca::Position3dDescription getDescription(const ::Ice::Current& ) const;

    virtual void subscribe(const ::orca::Position3dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::Position3dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // from Replayer

    virtual void replayData( int index, bool isTest=false );

private:
    orcaice::Buffer<orca::Position3dData> dataPipe_;
    orcaice::Buffer<orca::Position3dDescription> position3dDescriptionBuffer_;
    
    orca::Position3dData data_;
    orca::Position3dConsumerPrx publisher_;
    IceStorm::TopicPrx topic_;

    virtual void initInterfaces();
    virtual void initConfigs();
    
    // binary file protocol specifics
    void loadHeaderIce();
    void loadDataIce( int index );

    // this is a counter of instaces of this type, used for registered interface name
    static int _counter;
};

} // namespace

#endif
