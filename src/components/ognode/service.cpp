#include "service.h"
#include "component.h"

using namespace std;
using namespace ognode;

extern "C"
{
    // Factory function

    IceBox::Service* create( Ice::CommunicatorPtr communicator )
    {
        return new ognode::Service;
    }
}

Service::Service()
{
    component_ = new ognode::Component;
}
