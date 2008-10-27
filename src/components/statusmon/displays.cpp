
#include <orcaifacestring/systemstatus.h>
#include "displays.h"

namespace statusmon
{
    
TextDisplay::TextDisplay( const orcaice::Context &context )
    : AbstractDisplay( context )
{}
    
void 
TextDisplay::setSystemStatus( const std::vector<StatusDetails> &systemStatusDetails )
{
    for (unsigned int i=0; i<systemStatusDetails.size(); i++)
    {
        const StatusDetails &details = systemStatusDetails[i];
            
        if (!details.dataAvailable) {
            context_.tracer().info("TextDisplay: no data available yet");
            return;
        }
        
        // we have some data
        
        // is it stale?
        if (details.isStale) {
            context_.tracer().warning("TextDisplay: data is stale!");
        }
        
        // output data
        stringstream ss; 
        ss << "TextDisplay: system status:" << endl << ifacestring::toString( details.statusData ) << endl;
        context_.tracer().info( ss.str() );
    }
}
    
InterfaceDisplay::InterfaceDisplay( const orcaice::Context &context,
                                    gbxiceutilacfr::Thread *thread )
    : AbstractDisplay( context )
{
    iface_ = new orcaifaceimpl::SystemStatusImpl( "SystemStatus", context );
    iface_->initInterface( thread );
}

void 
InterfaceDisplay::setSystemStatus( const std::vector<StatusDetails> &systemStatusDetails )
{
    orca::SystemStatusData data;
    
    //TODO: convert from "systemStatusDetails" to "data"
    
    context_.tracer().warning("TODO: implement conversion");
    
    iface_->localSetAndSend( data );
}
    
    
}
