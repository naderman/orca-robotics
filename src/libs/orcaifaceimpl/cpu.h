/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_CPU_H
#define ORCAIFACEIMPL_CPU_H

#include <orca/cpu.h>
#include <orcaifaceutil/cpu.h>

#include <orcaifaceimpl/providerWithDescriptionImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

namespace orcaifaceimpl {

typedef ProviderWithDescriptionImpl<orca::Cpu, orca::CpuConsumer,
                    orca::CpuData, orca::CpuDescription> CpuImpl;
typedef IceUtil::Handle<CpuImpl> CpuImplPtr;

typedef StoringConsumerImpl<orca::Cpu, orca::CpuConsumer,
                    orca::CpuData> StoringCpuConsumerImpl;
typedef IceUtil::Handle<StoringCpuConsumerImpl> StoringCpuConsumerImplPtr;

typedef BufferedConsumerImpl<orca::Cpu, orca::CpuConsumer,
                        orca::CpuData> BufferedCpuConsumerImpl;
typedef IceUtil::Handle<BufferedCpuConsumerImpl> BufferedCpuConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::Cpu, orca::CpuConsumer,
                        orca::CpuData> NotifyingCpuConsumerImpl;
typedef IceUtil::Handle<NotifyingCpuConsumerImpl> NotifyingCpuConsumerImplPtr;

typedef PrintingConsumerImpl<orca::Cpu, orca::CpuConsumer,
                        orca::CpuData> PrintingCpuConsumerImpl;
typedef IceUtil::Handle<PrintingCpuConsumerImpl> PrintingCpuConsumerImplPtr;

}

#endif
