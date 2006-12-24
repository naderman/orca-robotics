#include "pointcloudconsumerI.h"
#include <Ice/Ice.h>

using namespace pcviwere;
using namespace std; 

PointCloudConsumerI::PointCloudConsumerI(std::string in_endpoint){
  endpoint = in_endpoint;
}

void 
PointCloudConsumerI::setData(const ::orca::PointCloudData&, const Ice::Current&){
  // Does nothing for now 
}

void 
PointCloudConsumerI::init(int argc, char **argv){
    cerr << "PointCloudConsumer started with " << endpoint.c_str() << endl;
    Ice::CommunicatorPtr ic;
    ic = Ice::initialize(argc, argv);
    cerr << "Ice initialised" << endl; 
    try {
	Ice::ObjectPrx base = ic->stringToProxy("PointCloudServer:" + endpoint);
	cerr << "Proxy set up" << endl;
	pcpPrx = orca::PointCloudProducerPrx::checkedCast(base);
	if (!pcpPrx)
	{
	    throw "Invalid proxy";
	}
	adapter = ic->createObjectAdapterWithEndpoints("PointCloudConsumer", "tcp -z");
	Ice::ObjectPrx objPrx = adapter->add(this, ic->stringToIdentity("PointCloudConsumer"));
	myPrx = PointCloudConsumerPrx::checkedCast(objPrx);
	adapter->activate();
	cerr << "About to subscribe ..." << endl;  
	pcpPrx->subscribe(myPrx);
	cerr << "Subscribed." << endl; 
    }
    
    catch(const ::Ice::Exception & e){
	cerr << "Exception caught ..." << endl;
	cerr << e << endl;     
    }
}

orca::PointCloud 
PointCloudConsumerI::getPointCloud(){
  return pcpPrx->getPointCloud(); 
}


