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
        SubscribeJob( const orcaice::Context &context, 
                      StatusConsumerImplPtr   statusConsumer ) :
            context_(context),
            statusConsumer_(statusConsumer)
        {};
              
        virtual void execute()
        {
            context_.tracer().debug("Executing a SubscribeJob", 2);
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
        orcaice::Context       context_;
        StatusConsumerImplPtr  statusConsumer_;
};

} // end of namespace


ComponentMonitor::ComponentMonitor( hydroiceutil::JobQueuePtr  jobQueue,
                                    const std::string         &platformName,
                                    const std::string         &componentName,
                                    const orcaice::Context    &context )
    : currentState_(orca::ObsCompInactive),
      jobQueue_(jobQueue),
      platformName_(platformName),
      componentName_(componentName),
      context_(context)
{
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag()+".Config.";
    int resubscribeInterval = orcaice::getPropertyAsIntWithDefault( prop, prefix+"ResubscribeInterval", 5 );
    
    StatusConsumerImpl::Config config( platformName, componentName, resubscribeInterval ) ;
    statusConsumer_ = new StatusConsumerImpl( config, context_ );
    
    // try to subscribe
    hydroiceutil::JobPtr job = new SubscribeJob( context_, statusConsumer_ );
    jobQueue_->add( job );   
}

ComponentMonitor::~ComponentMonitor()
{
}

bool
ComponentMonitor::isHomeInterfaceReachable()
{
    orca::FQComponentName fqName;
    fqName.platform = platformName_;
    fqName.component = componentName_;
    
    string interfaceType = "::orca::Home";
    string diagnostic = "";
   
    bool isReachable = orcaice::isAdminInterfaceReachable( context_, fqName, interfaceType, diagnostic );
    
    if (!isReachable)
    {
        stringstream ss;
        ss << "Home interface of " << orcaice::toString( fqName ) << " is not reachable. Diagnostics: " << diagnostic;
        context_.tracer().warning( ss.str() );
    }
    
    return isReachable;
}

void
ComponentMonitor::setObservedState( orca::ObservedComponentStatus &obsCompStat )
{
    obsCompStat.name.platform = platformName_;
    obsCompStat.name.component = componentName_; 
    obsCompStat.timeUp = 0;
    obsCompStat.publishIntervalSec = 10.0;
    obsCompStat.state = currentState_; 
    obsCompStat.health = orca::ObsCompOk;
}

void
ComponentMonitor::setReportedState( const StatusDetails &statDetails,
                                    orca::ObservedComponentStatus &obsCompStat )
{
    obsCompStat.name = statDetails.data.compStatus.name;
    obsCompStat.timeUp = statDetails.data.compStatus.timeUp;
    obsCompStat.publishIntervalSec = statDetails.data.compStatus.publishIntervalSec;
    obsCompStat.subsystems = statDetails.data.compStatus.subsystems;
    
    //
    // Health
    //
    if (statDetails.isDataStale) 
    {
        obsCompStat.health = orca::ObsCompStale;
    } 
    else 
    {        
        switch( statDetails.data.compStatus.health )
        {
            case orca::CompOk: 
                obsCompStat.health = orca::ObsCompOk; break;
            case orca::CompWarning: 
                obsCompStat.health = orca::ObsCompWarning; break;
            case orca::CompFault: 
                obsCompStat.health = orca::ObsCompFault; break;
            case orca::CompStalled: 
                obsCompStat.health = orca::ObsCompStalled; break;
            default:
                assert( false && "Unknown component health" );
        }
    }
    
    //
    // State
    //
    switch( statDetails.data.compStatus.state )
    {
        case orca::CompInitialising:
            obsCompStat.state = orca::ObsCompInitialising; break;
        case orca::CompActive:
            obsCompStat.state = orca::ObsCompActive; break;
        case orca::CompFinalising:
            obsCompStat.state = orca::ObsCompFinalising; break;
        default:
            assert( false && "Unknown component state" );
    }
    
    currentState_ = obsCompStat.state;
}

void
ComponentMonitor::addSubscribeJob()
{
    hydroiceutil::JobPtr job = new SubscribeJob( context_, statusConsumer_ );
    jobQueue_->add( job );
}


void 
ComponentMonitor::getComponentStatus( orca::ObservedComponentStatus &obsCompStat )
{   
    
    if (currentState_ == orca::ObsCompInactive)
    {
        if ( isHomeInterfaceReachable() )
        {
            addSubscribeJob();
            currentState_ = orca::ObsCompConnecting;
        }
        setObservedState( obsCompStat );
        return;
    }
    
    // currentState_ > ObsCompInactive: try to get status from interface
    StatusDetails statDetails;
    bool shouldResubscribe = statusConsumer_->getStatus( statDetails );
    if (shouldResubscribe) 
        addSubscribeJob();
    
    // if no data has ever been available, we're not connected yet
    if (!statDetails.dataAvailable)
    {
        setObservedState( obsCompStat );
        return;
    }
    
    // we only get here if we have some reported data from the Status interface
    setReportedState( statDetails, obsCompStat );
}

}
