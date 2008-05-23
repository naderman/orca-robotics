/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAPROBE_UTILS_H
#define ORCA_ORCAPROBE_UTILS_H

#include <orca/common.h>
#include <orcacm/types.h>

namespace orcaprobe
{

void reportResult( orcacm::OperationData& data, const std::string& type, const std::string& text );

void reportException( orcacm::OperationData& data, const std::string& error );

void reportSubscribed( orcacm::OperationData& data, const std::string& proxy="" );

void reportUnsubscribed( orcacm::OperationData& data );

void reportNotImplemented( orcacm::OperationData& data );

} // namespace

#endif
