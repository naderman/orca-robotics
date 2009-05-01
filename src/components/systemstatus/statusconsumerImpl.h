/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef STATUS_CONSUMER_IMPL_H
#define STATUS_CONSUMER_IMPL_H

#include <orcasystemstatusutil/statusconsumerImpl.h>
#include <gbxsickacfr/gbxiceutilacfr/store.h>

namespace systemstatus {

class StatusConsumerImpl : public orcasystemstatusutil::StatusConsumerImpl
{
public: 

    // Platform and component names are different from the ones in the context!
    StatusConsumerImpl( const orca::FQComponentName& fqCompName, 
                        const Config& config, 
                        const orcaice::Context& context );

    // thread-safe access to the latest available estimated component status
    orca::EstimatedComponentStatus estimatedStatus() const;

    // thread-safe acess to the state which remember if resubscribe has been requested.
    // Default behavior of this function is to turn the state off if it's on. I.e. a repeat
    // call will return FALSE (assuming that another request has not been filed).
    bool isResubscribeRequested( bool turnOffIfOn=true );

protected:

    // generated events
    virtual void estimateChangedEvent( const orca::EstimatedComponentStatus& );
    virtual void resubscribeRequestedEvent( const orca::EstimatedComponentStatus& status );

private:

    gbxiceutilacfr::Store<orca::EstimatedComponentStatus> estCompStatus_;

    // there's a small change of missing a repeat request to resubscribe (see
    // comments in the code). it's not important in this case.
    gbxiceutilacfr::Store<bool> resubscribeRequest_;

};
typedef IceUtil::Handle<StatusConsumerImpl> StatusConsumerImplPtr;

}

#endif
