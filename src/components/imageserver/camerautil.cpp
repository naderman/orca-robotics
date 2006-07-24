/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "camerautil.h"
#include <iostream>

#include <orcaobj/mathdefs.h>

using namespace std;
using namespace orca;

namespace camerautil {

bool isPositive( int val)
{
    return ( val > 0 );
}

bool isPositive( float val)
{
    return ( val > 0.0 );
}

bool cameraCanDoConfig( const CameraConfigPtr &cfg )
{
    if ( cfg->isEnabled == false ) return true;

    if ( (!isPositive(cfg->imageWidth)) && (!isPositive(cfg->imageHeight)) )
    {
        cout<<"bad image width and/or height" << endl;

        // bad imageWidth or imageHeight
        return false;
    }
    if ( (!isPositive(cfg->frameRate)) )
    {
        cout<<"bad frame rate" << endl;

        // bad frameRate
        return false;
    }
    // TODO: is there a way to check the type of image format and compression
    return true;

}

}
