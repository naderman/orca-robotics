#include <orcaice/orcaice.h>

#include "component.h"
#include "handler.h"

using namespace laser2og;

Component::Component()
    : orcaice::Component( "Laser2Og" ),
      handler_(0)
{
}

Component::~Component()
{
}

// NOTE: this function returns after it's done, all variable that need to be permanet must
//       be declared as member variables.
void
Component::start()
{
    activate();
    handler_ = new Handler( context() );
    handler_->start();
}

void
Component::stop()
{
    if ( handler_ ) 
    {
        IceUtil::ThreadControl algoControl = handler_->getThreadControl();
        tracer()->debug("stopping algorithm handler", 5 );
        handler_->stop();
        tracer()->debug("joining algorithm handler", 5 );
        algoControl.join();
    }
}
