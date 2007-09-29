/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOGFACTORY_LASERSCANNER2D_REPLAYER_H
#define ORCA2_ORCALOGFACTORY_LASERSCANNER2D_REPLAYER_H

#include <IceStorm/IceStorm.h>
#include <orca/laserscanner2d.h>
#include <orcalog/replayer.h>

#include <orcaiceutil/proxy.h>

namespace orcalogfactory
{

class LaserScanner2dReplayer : public virtual orca::LaserScanner2d, public orcalog::Replayer
{
public:
    LaserScanner2dReplayer( const std::string      &format,
                            const std::string      &filename,
                            const orcaice::Context &context );
    virtual ~LaserScanner2dReplayer();
    
    // from Orca object

    virtual ::orca::RangeScanner2dDataPtr     getData(const ::Ice::Current& ) const;

    virtual ::orca::RangeScanner2dDescription getDescription(const ::Ice::Current& ) const;
    
    virtual void subscribe(const ::orca::RangeScanner2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::RangeScanner2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // from Replayer

    virtual void replayData( int index, bool isTest=false );

private:
    orcaiceutil::Proxy<orca::LaserScanner2dDataPtr> dataPipe_;
    orcaiceutil::Proxy<orca::RangeScanner2dDescription> laserDescriptionPipe_;
    
    orca::LaserScanner2dDataPtr data_;
    orca::RangeScanner2dConsumerPrx publisher_;
    IceStorm::TopicPrx topicPrx_;

    virtual void initInterfaces();
    virtual void initDescription();
    
    // binary file protocol specifics
    void loadHeaderIce();
    void loadDataIce( int index );

    // this is a counter of instaces of this type, used for registered interface name
    static int _counter;
};

} // namespace

#endif
