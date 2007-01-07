/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_REGVIEW_NETWORK_HANDLER_H
#define ORCA2_REGVIEW_NETWORK_HANDLER_H

#include <map>

#include <orcaice/context.h>
#include <orcaice/notify.h>
#include <orcacm/types.h>

#include "callqueue.h"

namespace orcaqcm
{

typedef std::map<std::string,std::string> PropertiesMessage;

class NetworkHandler : public CallQueue
{

public:

    NetworkHandler( const orcaice::Context & context );
    ~NetworkHandler();    
    
    void getComponentInfo();
    orcaice::Notify<orcacm::ComponentData> homePipe_;

    void getProperties( const std::string & proxy );
    orcaice::Notify<PropertiesMessage> propPipe_;
    
private:

    orcaice::Context context_;
};

} // namespace

#endif
