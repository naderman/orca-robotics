/*
 *  Orca-Base: Components for robotics.
 *
 *  Copyright (C) 2004
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

#include "laserconsumer_i.h"

#include <orcaiceutil/objutils.h>

using namespace std;
using namespace orcaiceutil;
using namespace orca;

LaserConsumerI::LaserConsumerI( PtrBuffer<LaserDataPtr> &laserDataBuffer )
    : laserDataBuffer_(laserDataBuffer)
{

}

void LaserConsumerI::setData(const RangeScannerDataPtr& data, const Ice::Current&)
{
    //cout << "INFO(laserconsumer_i.cpp): Received laserscan. Putting it into buffer now" << endl << endl;
    laserDataBuffer_.push( LaserDataPtr::dynamicCast( data ) );
}
