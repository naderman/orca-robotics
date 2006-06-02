/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <iostream>
#include <assert.h>
#include <IceUtil/Thread.h>     // for sleep()
#include <orcaice/orcaice.h>

#include "staticdriver.h"

using namespace std;
using namespace orca;
using namespace simlocaliser;

StaticDriver::StaticDriver( const orca::Frame2d & pose )
    : location2d_(new orca::Localise2dData)
{
    location2d_->hypotheses.resize(1);
    location2d_->hypotheses[0].weight = 1.0;
    
    location2d_->hypotheses[0].mean = pose;

    // should this also be configurable?
    location2d_->hypotheses[0].cov.xx   = 0.01;
    location2d_->hypotheses[0].cov.yy   = 0.01;
    location2d_->hypotheses[0].cov.tt   = 0.01*M_PI/180.0;
    location2d_->hypotheses[0].cov.xy   = 0.0;
    location2d_->hypotheses[0].cov.xt   = 0.0;
    location2d_->hypotheses[0].cov.yt   = 0.0;
}

int
StaticDriver::read( orca::Localise2dDataPtr & localise2d )
{
    orcaice::setToNow( location2d_->timeStamp );

    // it's ok just to copy smart pointers because we never modify on our side.
    localise2d = location2d_;

    IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(1000));

    return 0;
}
