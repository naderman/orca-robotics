/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_MULTICAMERA_H
#define ORCAIFACEIMPL_MULTICAMERA_H

#include <orcaifaceimpl/providerWithDescriptionImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

#include <orca/camera.h>

namespace orcaifaceimpl {

typedef ProviderWithDescriptionImpl<orca::MultiCamera, orca::MultiCameraPrx,
                    orca::ImageConsumer, orca::ImageConsumerPrx,
                    orca::ImageDataPtr, orca::ImageDescriptionPtr> MultiCameraImpl;
typedef IceUtil::Handle<MultiCameraImpl> MultiCameraImplPtr;

typedef StoringConsumerImpl<orca::MultiCamera, orca::MultiCameraPrx,
                    orca::ImageConsumer, orca::ImageConsumerPrx,
                    orca::ImageDataPtr> StoringMultiCameraConsumerImpl;
typedef IceUtil::Handle<StoringMultiCameraConsumerImpl> StoringMultiCameraConsumerImplPtr;

typedef BufferedConsumerImpl<orca::MultiCamera, orca::MultiCameraPrx,
                        orca::ImageConsumer, orca::ImageConsumerPrx,
                        orca::ImageDataPtr> BufferedMultiCameraConsumerImpl;
typedef IceUtil::Handle<BufferedMultiCameraConsumerImpl> BufferedMultiCameraConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::MultiCamera, orca::MultiCameraPrx,
                        orca::ImageConsumer,
                        orca::ImageConsumerPrx,
                        orca::ImageDataPtr> NotifyingMultiCameraConsumerImpl;
typedef IceUtil::Handle<NotifyingMultiCameraConsumerImpl> NotifyingMultiCameraConsumerImplPtr;

typedef PrintingConsumerImpl<orca::MultiCamera, orca::MultiCameraPrx,
                        orca::ImageConsumer, orca::ImageConsumerPrx,
                        orca::ImageDataPtr> PrintingMultiCameraConsumerImpl;
typedef IceUtil::Handle<PrintingMultiCameraConsumerImpl> PrintingMultiCameraConsumerImplPtr;

}

#endif
