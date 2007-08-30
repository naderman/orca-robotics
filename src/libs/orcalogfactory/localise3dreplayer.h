/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOGFACTORY_LOCALISE3D_REPLAYER_H
#define ORCA2_ORCALOGFACTORY_LOCALISE3D_REPLAYER_H

#include <IceStorm/IceStorm.h>
#include <orca/localise2d.h>
#include <orcalog/replayer.h>

#include <orcaice/proxy.h>

namespace orcalogfactory
{

class Localise3dReplayer : public orca::Localise3d, public orcalog::Replayer
{
public:
    Localise3dReplayer( const std::string      &format,
                    const std::string      &filename,
                    const orcaice::Context &context );
    virtual ~Localise3dReplayer();
    
    // from Orca object

    virtual ::orca::Localise3dData getData(const ::Ice::Current& ) const;
    
    virtual ::orca::VehicleGeometryDescriptionPtr getVehicleGeometry( const ::Ice::Current& ) const;
      
    virtual void subscribe(const ::orca::Localise3dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::Localise3dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // from Replayer

    virtual void replayData( int index, bool isTest=false );

private:
    orcaice::Proxy<orca::Localise3dData> dataPipe_;
    orca::VehicleGeometryDescriptionPtr geometry_;
    
    orca::Localise3dData data_;
    orca::Localise3dConsumerPrx publisher_;
    IceStorm::TopicPrx topicPrx_;

    virtual void initInterfaces();
    virtual void initDescription();

    // this is a counter of instaces of this type, used for registered interface name
    static int _counter;
};

} // namespace

#endif
