/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_IMAGE_PROPERTY_UTILS_H
#define ORCA_IMAGE_PROPERTY_UTILS_H

#include <hydroimage/structures.h>

#include <orcaice/context.h>
#include <orca/image.h>
#include <orca/camera.h>
#include <orca/multicamera.h>

namespace orcaimage
{
    //! grab all properties from a config file related to the ImageDescription structure
    void getImageProperties(const orcaice::Context& context, const std::string& prefix, orca::ImageDescriptionPtr description);
    //! grab all properties from a config file related to the CameraDescription structure
    void getCameraProperties(const orcaice::Context& context, const std::string& prefix, orca::CameraDescriptionPtr description);
    //! grab all properties from a config file related to the MultiCameraDescription structure
    void getMultiCameraProperties(const orcaice::Context& context, const std::string& prefix, orca::MultiCameraDescriptionPtr description);

    //! copy from a ImageDescription to a hydro image Config structure
    void copy( hydroimage::Config& config, const orca::ImageDescriptionPtr description);

    //! copy from a hydro image config structure to an ImageDescription structure
    void copy( orca::ImageDescriptionPtr description, const hydroimage::Config& config);
}

#endif
