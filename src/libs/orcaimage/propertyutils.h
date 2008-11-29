/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_IMAGE_PROPERTY_UTILS_H
#define ORCA_IMAGE_PROPERTY_UTILS_H

#include <orcaice/context.h>
#include <orca/image.h>
#include <orca/camera.h>
#include <orca/cameracollection.h>

namespace orcaimage
{
    void getImageProperties(const orcaice::Context& context, const std::string& prefix, orca::ImageDescriptionPtr description);
    void getCameraProperties(const orcaice::Context& context, const std::string& prefix, orca::CameraDescriptionPtr description);
    void getCameraCollectionProperties(const orcaice::Context& context, const std::string& prefix, orca::CameraCollectionDescriptionPtr description);
}

#endif
