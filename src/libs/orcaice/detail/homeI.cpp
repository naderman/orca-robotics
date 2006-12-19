/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>

#include "homeI.h"

using namespace orcaice;

HomeI::HomeI( const orca::ComponentData & compData, const Ice::PropertyDict & props )
        : properties_(props)
{
    // start the up-time clock
    startTime_ = IceUtil::Time::now();

    // don't forget to creat the object first
    homeData_ = new orca::HomeData;
    
    // component data is static, so this time stamp will never change.
    homeData_->timeStamp = orcaice::toOrcaTime( startTime_ );
    homeData_->comp = compData;

    // debug
    /*
    std::cout<<"*** HOME DEBUG ***"<<std::endl;
    for ( unsigned int i=0; i<homeData->provides.size(); ++i ) {
        std::cout<<homeData_[i]<<":";
    }
    std::cout<<std::endl;
    */
}

orca::HomeDataPtr 
HomeI::getInterfaces(const ::Ice::Current& ) const
{
    //std::cout << "Sending data back" << std::endl;

    IceUtil::Time timeUp = IceUtil::Time::now() - startTime_;
    // we only return the number of seconds
    homeData_->timeUp = (int)timeUp.toSeconds();

    return homeData_;
}

int
HomeI::getTimeUp(const ::Ice::Current& ) const
{
    IceUtil::Time timeUp = IceUtil::Time::now() - startTime_;
    // we only return the number of seconds
    return (int)timeUp.toSeconds();
}

orca::ComponentProperties 
HomeI::getProperties(const ::Ice::Current& ) const
{
    //std::cout << "Sending data back" << std::endl;

    return properties_;
}
