#include "service.h"
#include "component.h"

using namespace std;
using namespace noisyrobot2d;

extern "C"
{
    //
    // Factory function
    //
    IceBox::Service* create( Ice::CommunicatorPtr communicator )
    {
        return new Service;
    }
}

Service::Service()
{
    component_ = new Component;
}
