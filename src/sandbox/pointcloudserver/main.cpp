#include "pcserver.h"
#include <Ice/Ice.h>

class PointCloudServerApp : virtual public Ice::Application
{
public:
  
  virtual int run(int argc, char** argv)
  {
    Ice::CommunicatorPtr ic;
    ic = Ice::initialize(argc, argv);
    // Note: This actually works if there is no parameter since argv[argc] is required to be NULL according to the 
    // ISO C++ standard
    orca::PointCloudServer *pcs = new orca::PointCloudServer(argv[1]);
    printf("Setting up adaptor ...\n"); 
    Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapterWithEndpoints("pointcloudserver", "default -p 10101 -z");
    adapter->add(pcs, ic->stringToIdentity("PointCloudServer"));
    adapter->activate();
    printf("Waiting for requests ...\n"); 
    communicator()->waitForShutdown();

    // Now we should clean up
   
   
    
    return EXIT_SUCCESS;
    }
};



int main(int argc, char* argv[])
{
  printf("starting up ..."); 
  PointCloudServerApp app;
  return app.main(argc, argv);
}
