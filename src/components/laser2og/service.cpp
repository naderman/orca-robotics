
#include "service.h"
#include "component.h"

using namespace std;
using namespace laser2og;

extern "C"
{
    // Factory function

    IceBox::Service* create( Ice::CommunicatorPtr communicator )
    {
        return new laser2og::Service;
    }
}

Service::Service()
{
    component_ = new laser2og::Component;
}
