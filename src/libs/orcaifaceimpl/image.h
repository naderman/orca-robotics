/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_IMAGE_H
#define ORCAIFACEIMPL_IMAGE_H

#include <orca/image.h>
#include <orcaifaceutil/image.h>

#include <orcaifaceimpl/providerWithDescriptionImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

namespace orcaifaceimpl {

typedef ProviderWithDescriptionImpl<orca::Image, orca::ImageConsumer,
                    orca::ImageDataPtr, orca::ImageDescriptionPtr> ImageImpl;
typedef IceUtil::Handle<ImageImpl> ImageImplPtr;

typedef StoringConsumerImpl<orca::Image, orca::ImageConsumer,
                    orca::ImageDataPtr> StoringImageConsumerImpl;
typedef IceUtil::Handle<StoringImageConsumerImpl> StoringImageConsumerImplPtr;

typedef BufferedConsumerImpl<orca::Image, orca::ImageConsumer,
                        orca::ImageDataPtr> BufferedImageConsumerImpl;
typedef IceUtil::Handle<BufferedImageConsumerImpl> BufferedImageConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::Image, orca::ImageConsumer,
                        orca::ImageDataPtr> NotifyingImageConsumerImpl;
typedef IceUtil::Handle<NotifyingImageConsumerImpl> NotifyingImageConsumerImplPtr;

typedef PrintingConsumerImpl<orca::Image, orca::ImageConsumer,
                        orca::ImageDataPtr> PrintingImageConsumerImpl;
typedef IceUtil::Handle<PrintingImageConsumerImpl> PrintingImageConsumerImplPtr;

}

#endif
