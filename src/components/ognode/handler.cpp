/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <orcaogmap/orcaogmap.h>
#include <orca/ogmap.h>

#include "handler.h"

using namespace std;
using namespace ognode;

Handler::Handler(OgMapI& OgMapInterface,
                orcaice::PtrBuffer<orca::OgFusionDataPtr> &OgFusionDataBuffer,
                orcaogmap::OgMap& localMap,
                orcaice::Context current )
    : ogMapInterface_(OgMapInterface),
    OgFusionDataBuffer_(OgFusionDataBuffer),
    localMap_(localMap),
    context_(current)
{
}

Handler::~Handler(){
}

void
Handler::run()
{
    try
    {
	orca::OgFusionDataPtr data = new orca::OgFusionData;
        //object to send
        orca::OgMapDataPtr map = new orca::OgMapData;

        //
        // IMPORTANT: Have to keep this loop rolling, because the 'isActive()' call checks for requests to shut down.
        //            So we have to avoid getting stuck in a loop anywhere within this main loop.
        //
        while ( isActive() )
	{
	    int ret=OgFusionDataBuffer_.getAndPopNext(data,1000);
	    if(ret==0)
	    {
		//cout << "got an object with " << data->observation.size() << " cells\n";
		for(unsigned int i=0;i<data->observation.size();i++)
		{
		    ogfusion::add(localMap_,data->observation[i]);
		}
		convert(localMap_,map);
                map->timeStamp=data->timeStamp;
                ogMapInterface_.localSetData(map);
	    }
	    else if(ret==1){
                //interrupted
                cout << "interrupted\n";
	    }
            else{
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

