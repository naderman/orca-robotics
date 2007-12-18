/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef CAMERA_DRIVER_H
#define CAMERA_DRIVER_H

#include <orca/camera.h>
#include <orcaice/context.h>

static const ::std::string __orca__cameradriver_default_heartbeat_msg = "";

namespace imageserver {

/*
@brief Abstract class for a camera.

All cameras should implement this, making it simple
to add support for different cameras.

This guy is _not_ guaranteed to be thread-safe!

@author Ben Upcroft b.upcroft at cas.edu.au
*/
class Driver 
{

public:

    class Config
    {   
    public:
        Config();
        bool validate() const;
        std::string toString() const;
        bool operator==( const Config & other );
        bool operator!=( const Config & other );

        int imageWidth;
        int imageHeight;
        int imageSize;
        double frameRate;
        orca::ImageFormat format;
        orca::ImageCompression compression;
    };

    Driver( const Config& cfg, const orcaice::Context& context );
    virtual ~Driver() {};

    // Initializes the device. If it's aleady initialized, then it
    // quietly re-initializes it.
    // returns: 0 = success, non-zero = failure
    virtual int init()=0;

    // Utility function.
    // Initializes static data fields (e.g. imageWidth, etc.)
    // Reimplement this in derived drivers if something
    // non-standard is required.
    // returns: 0 = success, non-zero = failure
    virtual int initData( orca::CameraData& data ) const;

    // Blocks till new data is available
    virtual int read( orca::CameraData& data )=0;

    // TODO: Should also have getData here as we don't always want all info from the read

    // mechanism to get error messages etc back from driver.
    virtual const std::string &infoMessages() { return infoMessages_; };

    // Any special info to put in the heartbeat messages?
    virtual const std::string heartbeatMessage() { return __orca__cameradriver_default_heartbeat_msg; };

    // Simply returns the current stored configuration. Does not talk to device 
    Config config() const { return config_; };

protected:

    Config config_;

    std::string infoMessages_;
    
    orcaice::Context context_;
         
};

} // namespace

#endif
