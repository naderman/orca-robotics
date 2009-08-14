/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_MULTICAMERA_H
#define ORCAIFACEIMPL_MULTICAMERA_H

#include <orca/multicamera.h>
#include <orcaifaceutil/multicamera.h>

#include <orcaifaceimpl/providerWithDescriptionImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

namespace orcaifaceimpl {

typedef ProviderWithDescriptionImpl<orca::MultiCamera, orca::MultiCameraConsumer,
                    orca::MultiCameraDataPtr, orca::MultiCameraDescriptionPtr> MultiCameraImpl;
typedef IceUtil::Handle<MultiCameraImpl> MultiCameraImplPtr;

typedef StoringConsumerImpl<orca::MultiCamera, orca::MultiCameraConsumer,
                    orca::MultiCameraDataPtr> StoringMultiCameraConsumerImpl;
typedef IceUtil::Handle<StoringMultiCameraConsumerImpl> StoringMultiCameraConsumerImplPtr;

typedef BufferedConsumerImpl<orca::MultiCamera, orca::MultiCameraConsumer,
                    orca::MultiCameraDataPtr> BufferedMultiCameraConsumerImpl;
typedef IceUtil::Handle<BufferedMultiCameraConsumerImpl> BufferedMultiCameraConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::MultiCamera, orca::MultiCameraConsumer,
                    orca::MultiCameraDataPtr> NotifyingMultiCameraConsumerImpl;
typedef IceUtil::Handle<NotifyingMultiCameraConsumerImpl> NotifyingMultiCameraConsumerImplPtr;

typedef PrintingConsumerImpl<orca::MultiCamera, orca::MultiCameraConsumer,
                    orca::MultiCameraDataPtr> PrintingMultiCameraConsumerImpl;
typedef IceUtil::Handle<PrintingMultiCameraConsumerImpl> PrintingMultiCameraConsumerImplPtr;

}

#endif
