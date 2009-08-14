#include "coordinateframemanager.h"
#include <iostream>

using namespace std;

namespace hydroqgui {

CoordinateFrameManager::CoordinateFrameManager()
    : coordinateFramePlatform_("global"),
      ignoreCoordinateFrameRotation_(false)
{
}

void
CoordinateFrameManager::setCoordinateFramePlatform( const QString &platformName )
{
    coordinateFramePlatform_ = platformName;
    ignoreCoordinateFrameRotation_ = false;
}

void
CoordinateFrameManager::setOriginPlatform( const QString &platformName )
{
    coordinateFramePlatform_ = platformName;
    ignoreCoordinateFrameRotation_ = true;
}

}

