#include <iostream>

#include <orca/rangescanner.h>
#include <orca/localise2d.h>
#include <orca/ogfusion.h>
#include <orcaice/orcaice.h>

#include "laser2og.h"
#include "handler.h"

using namespace std;
using namespace orca;
using namespace laser2og;
//using namespace orcaogmap;

Handler::Handler(orcaice::PtrBuffer<orca::RangeScannerDataPtr> &RangeScannerDataBuffer,
                const orca::Localise2dPrx  localise2dPrx,
                const orca::OgFusionPrx ogFusionPrx,
                Laser2Og& laser2Og,
                orcaice::Context context)
    : rangeScannerDataBuffer_(RangeScannerDataBuffer),
      localise2dPrx_(localise2dPrx),
      ogFusionPrx_(ogFusionPrx),
      laser2Og_(laser2Og),
      context_(context)
{
}

Handler::~Handler()
{
}

void
Handler::run()
{
    try
    {
	RangeScannerDataPtr rangeScan = new RangeScannerData;
	Localise2dDataPtr pose = new Localise2dData;

	OgFusionDataPtr obs = new OgFusionData;

    //
    // IMPORTANT: Have to keep this loop rolling, because the 'isActive()' call checks for requests to shut down.
    //            So we have to avoid getting stuck in a loop anywhere within this main loop.
    //
    while ( isActive() )
	{
	    int ret=rangeScannerDataBuffer_.getAndPopNext(rangeScan,1000);
	    if(ret==0)
        {
		    try
            {
                pose=localise2dPrx_->getDataAtTime(rangeScan->timeStamp);
            }
            catch( orca::DataNotExistException e )
            {
                cout << "ERROR(handler.cpp): could not fetch pose\n";
                cout << "ERROR(handler.cpp): reason: " << e.what << endl;
//                 throw orca::DataNotExistException("Could not fetch pose from localise2dPrx");
            }

            laser2Og_.process(*pose,*rangeScan);
            laser2Og_.getObs(obs->observation);
            obs->timeStamp = rangeScan->timeStamp;
            
            //send out OgFusionData
            ogFusionPrx_->setData(obs);

	    }
	    else if (ret==1)
        {
            cout << "TRACE(handler.cpp): Interrupted\n";
	    }
        else
        {
                //timeout
	    }
	}
    
    }   // end of try
    catch ( Ice::CommunicatorDestroyedException &e )
    {
        // This is OK: it means that the communicator shut down (eg via Ctrl-C)
        // somewhere in mainLoop.
        //
        // Could probably handle it better for an Application by stopping the component on Ctrl-C
        // before shutting down communicator.
    }
    context_.tracer()->debug( "dropping out from run()", 5 );
}

