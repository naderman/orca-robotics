/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAPROBE_UTILS_H
#define ORCA2_ORCAPROBE_UTILS_H

#include <orca/orca.h>
#include <orcacm/types.h>

namespace orcaprobe
{

void reportResult( orcacm::OperationData& data, const std::string& type, const std::string& error );

void reportException( orcacm::OperationData& data, const std::string& error );

void reportSubscribed( orcacm::OperationData& data );

void reportUnsubscribed( orcacm::OperationData& data );

void reportNotImplemented( orcacm::OperationData& data );

} // namespace

#endif
