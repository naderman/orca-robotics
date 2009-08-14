#ifndef HYDROQGUI_COORDINATEFRAMEMANAGER_H
#define HYDROQGUI_COORDINATEFRAMEMANAGER_H

#include <QString>
#include <hydroutil/uncopyable.h>

namespace hydroqgui {

//!
//! @brief Manages on which platform the origin of the coordinate frame is located.
//!        The special platform "global" puts the origin on no particular platform but
//!        colocates it with the map origin instead.     
//!
//! @author Alex Brooks
//!
class CoordinateFrameManager : public hydroutil::Uncopyable
{

public: 

    CoordinateFrameManager();

    // sets the coordinate frame to the guielement's platform
    void setCoordinateFramePlatform( const QString &platformName );
    void setOriginPlatform( const QString &platformName );
    
    // access method for platform coincident with origin
    const QString& coordinateFramePlatform() { return coordinateFramePlatform_; }
    QString coordinateFramePlatform() const { return coordinateFramePlatform_; }

    bool ignoreCoordinateFrameRotation() const { return ignoreCoordinateFrameRotation_; }

private: 

    QString coordinateFramePlatform_;
    bool ignoreCoordinateFrameRotation_;
};

}

#endif
