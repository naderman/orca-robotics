/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "cameraconsumerI.h"

using namespace imageviewer;

CameraConsumerI::CameraConsumerI( orcaice::PtrBuffer<orca::CameraDataPtr> &cameraDataBuffer ) : cameraDataBuffer_(cameraDataBuffer)
{
}
        
void CameraConsumerI::setData(const orca::CameraDataPtr& data, const Ice::Current&)
{
    try {
        // Is it a camera image?
        //std::cout << "Received camera data... " << std::endl;

        // put image into buffer
        cameraDataBuffer_.push( orca::CameraDataPtr::dynamicCast( data ) );
    }
    catch ( IceUtil::NullHandleException &e )
    {
        std::cout << "cameraconsumer_i.cpp: Couldn't get image from camera" << std::endl;
    }

}

