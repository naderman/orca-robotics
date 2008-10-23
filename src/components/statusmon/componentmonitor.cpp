/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "componentmonitor.h"

namespace statusmon {
    
class ConnectJob : public hydroiceutil::Job 
{
    public:  
        ConnectJob( const orcaice::Context &context, 
                    StatusConsumerImplPtr   statusConsumer ) :
            context_(context),
            statusConsumer_(statusConsumer)
        {};
              
        virtual void execute()
        {
            context_.tracer().debug("Executing job", 2);
            statusConsumer_->init();
        };

        virtual std::string toString() const
        {
            return "ConnectJob";
        };
        
    private:
        orcaice::Context       context_;
        StatusConsumerImplPtr  statusConsumer_;
};

ComponentMonitor::ComponentMonitor( hydroiceutil::JobQueuePtr  jobQueue,
                                    const std::string         &platform,
                                    const std::string         &component,
                                    const orcaice::Context    &context )
    : context_(context),
      config_( platform, component )
{
    statusConsumer_ = new StatusConsumerImpl( config_, context_ );
    
    job_ = new ConnectJob( context_, statusConsumer_ );
    jobQueue->add( job_ );   
}
    
StatusDetails 
ComponentMonitor::getStatus()
{
    return statusConsumer_->getStatus();
}

}
