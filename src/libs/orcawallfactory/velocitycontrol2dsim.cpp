/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcamisc/orcamisc.h>
#include <orcaifaceimpl/velocitycontrol2diface.h>

#include "velocitycontrol2dsim.h"

using namespace std;
using namespace orcawallfactory;


class PrivateNotifyHandler : public orcaice::NotifyHandler<orca::VelocityControl2dData>
{
public:
    PrivateNotifyHandler( orcaice::Tracer* tracer ) :
        tracer_(tracer) {};

    // from orcaice::NotifyHandler
    virtual void handleData( const orca::VelocityControl2dData& data )
    {
        tracer_->debug( "VelocityControl2dSim::handleData: received "+orcaice::toString(data), 6 );
    }
private:
    orcaice::Tracer* tracer_;
};

VelocityControl2dSim::VelocityControl2dSim( const std::string& tag, const orcaice::Context& context ) : 
    InterfaceSim(tag,context)
{
}
 
void 
VelocityControl2dSim::run()
{
    orca::VehicleDescription descr;
    orcamisc::readVehicleDescription( context_.properties(), configPrefix(), descr );

    orcaifaceimpl::VelocityControl2dIfacePtr iface;
    iface = new orcaifaceimpl::VelocityControl2dIface( descr, tag_, context_ );
    iface->initInterface( this );

    // create notify handler and hook him up to the notify guy.
    PrivateNotifyHandler notifyHandler( context_.tracer() );
    iface->setNotifyHandler( &notifyHandler );

    while ( isActive() )
    {
        try 
        {
            // unlike DriveBicycle, this interface currently does not serve reference command
//             orca::VelocityControl2dData data;
//             orcaice::setSane( data );
//             iface->localSetAndSend( data );

            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
            
        } // end of try
        catch ( Ice::CommunicatorDestroyedException & )
        {
            // This is OK: it means that the communicator shut down (eg via Ctrl-C)
            // somewhere in mainLoop. Eventually, component will tell us to stop.
        }
        catch ( const Ice::Exception &e )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected exception: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch ( const std::exception &e )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected exception: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch ( ... )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected unknown exception.";
            context_.tracer()->error( ss.str() );
        }

    } // end of while
};
