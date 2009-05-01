/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include "componentmonitor.h"

namespace systemstatus {
        
namespace {
    
class SubscribeJob : public hydroiceutil::Job 
{
    public:  
        SubscribeJob( const orca::FQComponentName& compName,
                      StatusConsumerImplPtr statusConsumer,
                      const orcaice::Context& context ) :
            compName_(compName),
            statusConsumer_(statusConsumer),
            context_(context)
        {};
              
        virtual void execute()
        {        
            context_.tracer().debug("Executing a SubscribeJob", 2);

            string interfaceType = "::orca::Status";
            string diagnostic = "";
            
            if ( !orcaice::isAdminInterfaceReachable( context_, compName_, interfaceType, diagnostic ) )
            {
                stringstream ss;
                ss << "Status interface of " << orcaice::toString( compName_ ) << " is not reachable. Diagnostics: " << diagnostic;
                context_.tracer().warning( ss.str() );
                return;
            }

            try 
            {
                statusConsumer_->subscribe();
            } 
            catch (std::exception &e) 
            {
                stringstream ss;
                ss << "Caught exception while executing SubscribeJob: " << e.what();
                context_.tracer().warning( ss.str() );
            }
        };

        virtual std::string toString() const
        {
            return "SubscribeJob";
        };
            
    private:
        orca::FQComponentName compName_;
        StatusConsumerImplPtr  statusConsumer_;
        orcaice::Context       context_;
};

} // end of namespace

/////////////////////////////////////////////

ComponentMonitor::ComponentMonitor( const orca::FQComponentName& compName,
                                    hydroiceutil::JobQueuePtr  jobQueue,
                                    const orcaice::Context    &context ) :
    compName_(compName),
    jobQueue_(jobQueue),
    context_(context)
{
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag()+".Config.";
    
    StatusConsumerImpl::Config config;
    config.resubscribeIntervalReporting = orcaice::getPropertyAsIntWithDefault( prop, prefix+"ResubscribeInterval", 5 );
    config.staleTimeout = orcaice::getPropertyAsIntWithDefault( prop, prefix+"StaleTimeout", 40 );
    config.giveUpTimeout = orcaice::getPropertyAsIntWithDefault( prop, prefix+"GiveUpTimeout", 70 );

    statusConsumer_ = new StatusConsumerImpl( compName_, config, context_ );

    jobQueue_->add( new SubscribeJob( compName_, statusConsumer_, context_ ) );
}

orca::EstimatedComponentStatus
ComponentMonitor::getComponentStatus()
{   
    statusConsumer_->checkStatus();

    if ( statusConsumer_->isResubscribeRequested() ) 
        jobQueue_->add( new SubscribeJob(  compName_, statusConsumer_, context_ ) );
    
    return statusConsumer_->estimatedStatus();
}

}
