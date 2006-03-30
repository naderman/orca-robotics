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

#include "cameraconsumer_i.h"

#include <orcaice/objutils.h>
#include <orcaice/mathdefs.h>
#include <iostream>

using namespace imageviewer;
using orcaice::operator<<;

CameraConsumerI::CameraConsumerI( orcaice::PtrBuffer<orca::CameraDataPtr> &cameraDataBuffer ) : cameraDataBuffer_(cameraDataBuffer)
{
}
        
void CameraConsumerI::setData(const orca::CameraDataPtr& data, const Ice::Current&)
{
    try {
        // Is it a camera image?
        //std::cout << orca::CameraDataPtr::dynamicCast( data ) << std::endl;
        //std::cout << "Received camera data... " << std::endl;
        // put image into buffer
        cameraDataBuffer_.push( orca::CameraDataPtr::dynamicCast( data ) );
    }
    catch ( IceUtil::NullHandleException &e )
    {
        std::cout << "cameraconsumer_i.cpp: Couldn't get image from camera" << std::endl;
    }

}

