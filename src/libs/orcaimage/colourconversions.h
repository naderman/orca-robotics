/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Ben Upcroft, Alex Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_IMAGE_COLOURCONVERSIONS_H
#define ORCA_IMAGE_COLOURCONVERSIONS_H

#include <opencv/cv.h>

#include <orca/camera.h>
#include <orca/image.h>

#ifdef USE_QT
#include <QImage>
#endif

namespace orcaimage{

#ifdef USE_QT
    //! Convert orca::ImageData in any known format to RGB QImage
    void cvtToQImage( QImage* image, const orca::ImageDataPtr src, const orca::ImageDescriptionPtr descr );
#endif

#ifdef USE_OPENCV
    //! Convert orca::ImageData in any known format to RGB IplImage 
    void cvtToRgb( IplImage* dest, const orca::ImageDataPtr src, const orca::ImageDescriptionPtr descr );

    //! Often use this function for converting to bgr image format used in opencv
    void cvtToBgr( IplImage* dest, const orca::ImageDataPtr src, const orca::ImageDescriptionPtr descr );
#endif

} // namespace
    
#endif
