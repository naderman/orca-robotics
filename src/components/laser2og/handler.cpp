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
                Laser2Og& Ogger,
                orcaice::Context current)
    : rangeScannerDataBuffer_(RangeScannerDataBuffer),
    localise2dPrx_(localise2dPrx),
    ogFusionPrx_(ogFusionPrx),
    Ogger_(Ogger),
    context_(current)
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
	RangeScannerDataPtr data = new RangeScannerData;
	Localise2dDataPtr pose = new Localise2dData;

	OgFusionDataPtr obs = new OgFusionData;

        //
        // IMPORTANT: Have to keep this loop rolling, because the 'isActive()' call checks for requests to shut down.
        //            So we have to avoid getting stuck in a loop anywhere within this main loop.
        //
        while ( isActive() )
	{
	    int ret=rangeScannerDataBuffer_.getAndPopNext(data,1000);
	    if(ret==0){
                //cout << "got an range scan object\n";
		//cout << data << endl ;
		// now we need the current pose
		try{
		    pose=localise2dPrx_->getDataAtTime(data->timeStamp);
		}catch( orca::DataNotExistException e ){
		    cout << "could not fetch pose\n";
		    cout << "reason: " << e.what << endl;

		}
		//cout << "fetched pose\n";
		//cout << pose << endl;

		//laserScan + pose to OgObservation
		Ogger_.process(*pose,*data);
                Ogger_.getObs(obs->observation);
		//obs.timeStamp = orcaice::toOrcaTime(IceUtil::Time::now());
                obs->timeStamp = data->timeStamp;
		//cout << "have " << obs->observation.size() << " cells\n";
		//send out OgFusionData
                ogFusionPrx_->setData(obs);

	    }
	    else if(ret==1){
		//interrupted
                cout << "interrupted\n";
	    }else{
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

