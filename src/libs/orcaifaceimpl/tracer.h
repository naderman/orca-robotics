/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_TRACER_H
#define ORCAIFACEIMPL_TRACER_H

#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

#include <orca/tracer.h>

namespace orcaifaceimpl {

// NOTE: TracerImpl is only used in libOrcaIce, that's where lives.
//       (Tracer interface does not fit into any of the templates)

typedef StoringConsumerImpl<orca::Tracer, orca::TracerConsumer,
                    orca::TracerData> StoringTracerConsumerImpl;
typedef IceUtil::Handle<StoringTracerConsumerImpl> StoringTracerConsumerImplPtr;

typedef BufferedConsumerImpl<orca::Tracer, orca::TracerConsumer,
                        orca::TracerData> BufferedTracerConsumerImpl;
typedef IceUtil::Handle<BufferedTracerConsumerImpl> BufferedTracerConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::Tracer, orca::TracerConsumer,
                        orca::TracerData> NotifyingTracerConsumerImpl;
typedef IceUtil::Handle<NotifyingTracerConsumerImpl> NotifyingTracerConsumerImplPtr;

typedef PrintingConsumerImpl<orca::Tracer, orca::TracerConsumer,
                        orca::TracerData> PrintingTracerConsumerImpl;
typedef IceUtil::Handle<PrintingTracerConsumerImpl> PrintingTracerConsumerImplPtr;

}

#endif
