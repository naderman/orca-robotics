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
