/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_CAMERA_H
#define ORCAIFACEIMPL_CAMERA_H

#include <orcaifaceimpl/providerWithDescriptionImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

#include <orca/camera.h>

namespace orcaifaceimpl {

typedef ProviderWithDescriptionImpl<orca::Camera, orca::CameraPrx,
                    orca::ImageConsumer, orca::ImageConsumerPrx,
                    orca::ImageDataPtr, orca::ImageDescriptionPtr> CameraImpl;
typedef IceUtil::Handle<CameraImpl> CameraImplPtr;

typedef StoringConsumerImpl<orca::Camera, orca::CameraPrx,
                    orca::ImageConsumer, orca::ImageConsumerPrx,
                    orca::ImageDataPtr> StoringCameraConsumerImpl;
typedef IceUtil::Handle<StoringCameraConsumerImpl> StoringCameraConsumerImplPtr;

typedef BufferedConsumerImpl<orca::Camera, orca::CameraPrx,
                        orca::ImageConsumer, orca::ImageConsumerPrx,
                        orca::ImageDataPtr> BufferedCameraConsumerImpl;
typedef IceUtil::Handle<BufferedCameraConsumerImpl> BufferedCameraConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::Camera, orca::CameraPrx,
                        orca::ImageConsumer,
                        orca::ImageConsumerPrx,
                        orca::ImageDataPtr> NotifyingCameraConsumerImpl;
typedef IceUtil::Handle<NotifyingCameraConsumerImpl> NotifyingCameraConsumerImplPtr;

typedef PrintingConsumerImpl<orca::Camera, orca::CameraPrx,
                        orca::ImageConsumer, orca::ImageConsumerPrx,
                        orca::ImageDataPtr> PrintingCameraConsumerImpl;
typedef IceUtil::Handle<PrintingCameraConsumerImpl> PrintingCameraConsumerImplPtr;

}

#endif
