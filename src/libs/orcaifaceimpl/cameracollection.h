/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_CAMERACOLLECTION_H
#define ORCAIFACEIMPL_CAMERACOLLECTION_H

#include <orcaifaceimpl/providerWithDescriptionImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

#include <orca/cameracollection.h>

namespace orcaifaceimpl {

typedef ProviderWithDescriptionImpl<orca::CameraCollection, orca::CameraCollectionPrx,
                    orca::CameraCollectionConsumer, orca::CameraCollectionConsumerPrx,
                    orca::CameraCollectionDataPtr, orca::CameraCollectionDescriptionPtr> CameraCollectionImpl;
typedef IceUtil::Handle<CameraCollectionImpl> CameraCollectionImplPtr;

typedef StoringConsumerImpl<orca::CameraCollection, orca::CameraCollectionPrx,
                    orca::CameraCollectionConsumer, orca::CameraCollectionConsumerPrx,
                    orca::CameraCollectionDataPtr> StoringCameraCollectionConsumerImpl;
typedef IceUtil::Handle<StoringCameraCollectionConsumerImpl> StoringCameraCollectionConsumerImplPtr;

typedef BufferedConsumerImpl<orca::CameraCollection, orca::CameraCollectionPrx,
                    orca::CameraCollectionConsumer, orca::CameraCollectionConsumerPrx,
                    orca::CameraCollectionDataPtr> BufferedCameraCollectionConsumerImpl;
typedef IceUtil::Handle<BufferedCameraCollectionConsumerImpl> BufferedCameraCollectionConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::CameraCollection, orca::CameraCollectionPrx,
                    orca::CameraCollectionConsumer, orca::CameraCollectionConsumerPrx,
                    orca::CameraCollectionDataPtr> NotifyingCameraCollectionConsumerImpl;
typedef IceUtil::Handle<NotifyingCameraCollectionConsumerImpl> NotifyingCameraCollectionConsumerImplPtr;

typedef PrintingConsumerImpl<orca::CameraCollection, orca::CameraCollectionPrx,
                    orca::CameraCollectionConsumer, orca::CameraCollectionConsumerPrx,
                    orca::CameraCollectionDataPtr> PrintingCameraCollectionConsumerImpl;
typedef IceUtil::Handle<PrintingCameraCollectionConsumerImpl> PrintingCameraCollectionConsumerImplPtr;

}

#endif
