/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_REGVIEW_NETWORK_HANDLER_H
#define ORCA2_REGVIEW_NETWORK_HANDLER_H

#include <map>

#include <orcaice/context.h>
#include <hydroutil/notify.h>
#include <orcacm/types.h>

#include "callqueue.h"

namespace orcaqcm
{

typedef std::map<std::string,std::string> PropertiesMessage;

class NetworkThread : public CallQueue
{

public:

    NetworkThread( const orcaice::Context & context );
    ~NetworkThread();    
    
    void getComponentInfo();
    hydroutil::Notify<orcacm::ComponentData> homePipe_;

    void getProperties( const std::string & proxy );
    hydroutil::Notify<PropertiesMessage> propPipe_;
    
private:

    orcaice::Context context_;
};

} // namespace

#endif
