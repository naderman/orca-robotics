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

#include <orcaice/ptrbuffer.h>

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
    
    virtual ::orca::Localise3dData getDataAtTime(const orca::Time&, const Ice::Current&) const;
    
//     virtual orca::Localise3dDescription getDescription(const Ice::Current& c) const;
    
    virtual void subscribe(const ::orca::Localise3dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::Localise3dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // from Replayer

    virtual void replayData( int index, bool isTest=false );

private:
    orcaice::Buffer<orca::Localise3dData> dataPipe_;
//     orcaice::Buffer<orca::Localise3dDescription> descriptionBuffer_;
    
    orca::Localise3dData data_;
    orca::Localise3dConsumerPrx publisher_;
    IceStorm::TopicPrx topic_;

    virtual void initInterfaces();
    virtual void initDescription();

    // this is a counter of instaces of this type, used for registered interface name
    static int _counter;
};

} // namespace

#endif
