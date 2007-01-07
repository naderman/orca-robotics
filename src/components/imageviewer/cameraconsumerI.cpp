/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "cameraconsumerI.h"

using namespace imageviewer;

CameraConsumerI::CameraConsumerI( orcaice::Buffer<orca::CameraData> &dataPipe ) 
    : dataPipe_(dataPipe)
{
}
        
void 
CameraConsumerI::setData(const orca::CameraData& data, const Ice::Current&)
{
    try {
        // put image into buffer
        dataPipe_.push( data );
    }
    catch ( IceUtil::NullHandleException &e )
    {
        std::cout << "cameraconsumer_i.cpp: Couldn't get image from camera" << std::endl;
    }
}
