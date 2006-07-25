/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef CAMERA_UTIL_H
#define CAMERA_UTIL_H

#include <orca/camera.h>

namespace camerautil {

bool isPositive( int val);
bool isPositive( float val);

bool cameraCanDoConfig( const orca::CameraConfigPtr &cfg );

}

#endif
