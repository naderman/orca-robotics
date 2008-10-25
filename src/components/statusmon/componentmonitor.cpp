/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include "componentmonitor.h"

namespace statusmon {
        
class SubscribeJob : public hydroiceutil::Job 
{
    public:  
        SubscribeJob( const orcaice::Context &context, 
                    StatusConsumerImplPtr   statusConsumer ) :
            context_(context),
            statusConsumer_(statusConsumer)
        {};
              
        virtual void execute()
        {
            context_.tracer().debug("Executing job", 2);
            //TODO: catch exceptions
            statusConsumer_->subscribe();
        };

        virtual std::string toString() const
        {
            return "SubscribeJob";
        };
        
    private:
        orcaice::Context       context_;
        StatusConsumerImplPtr  statusConsumer_;
};

    
class ResubscribeJob : public hydroiceutil::Job 
{
    public:  
        ResubscribeJob( const orcaice::Context &context, 
                    StatusConsumerImplPtr   statusConsumer ) :
            context_(context),
            statusConsumer_(statusConsumer)
        {};
              
        virtual void execute()
        {
            context_.tracer().debug("Executing job", 2);
            //TODO: catch exceptions
            statusConsumer_->resubscribe();
        };

        virtual std::string toString() const
        {
            return "ResubscribeJob";
        };
        
    private:
        orcaice::Context       context_;
        StatusConsumerImplPtr  statusConsumer_;
};

ComponentMonitor::ComponentMonitor( hydroiceutil::JobQueuePtr  jobQueue,
                                    const std::string         &platform,
                                    const std::string         &component,
                                    const orcaice::Context    &context )
    : jobQueue_(jobQueue),
      context_(context)
{
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag()+".Config.";
    int resubscribeTimeout = orcaice::getPropertyAsIntWithDefault( prop, prefix+"ResubscribeTimeout", 45 );
    int resubscribeInterval = orcaice::getPropertyAsIntWithDefault( prop, prefix+"ResubscribeInterval", 5 );
    int staleTimeout = orcaice::getPropertyAsIntWithDefault( prop, prefix+"StaleTimeout", 60 );
    
    StatusConsumerImpl::Config config( platform, component, 
                                       resubscribeTimeout, resubscribeInterval, staleTimeout ) ;
    
    statusConsumer_ = new StatusConsumerImpl( config, context_ );
    
    context_.tracer().info( "Launching a SubscribeJob now" );
    hydroiceutil::JobPtr job = new SubscribeJob( context_, statusConsumer_ );
    jobQueue_->add( job );   
}

ComponentMonitor::~ComponentMonitor()
{
}
    
StatusDetails 
ComponentMonitor::getStatus()
{
    StatusDetails details;
    bool shouldResubscribe = statusConsumer_->getStatus( details );
    
    if (shouldResubscribe) {
        context_.tracer().info( "Launching a ResubscribeJob now" );
        hydroiceutil::JobPtr job = new ResubscribeJob( context_, statusConsumer_ );
        jobQueue_->add( job );
    }
    
    return details;
}

}
