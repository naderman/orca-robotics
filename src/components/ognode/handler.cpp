/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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
                orcaice::Buffer<orca::OgFusionData>& OgFusionDataBuffer,
                orcaogmap::OgMap& localMap,
                const orcaice::Context & context)
    : ogMapInterface_(OgMapInterface),
    OgFusionDataBuffer_(OgFusionDataBuffer),
    localMap_(localMap),
    context_(context)
{
}

Handler::~Handler(){
}

void
Handler::run()
{
    orca::OgFusionData data;
    orca::OgMapData map;
    
    while ( isActive() )
    {
        try
        {
            while ( isActive() )
            {
                int ret=OgFusionDataBuffer_.getAndPopNext(data,1000);
                if(ret!=0) {
                    context_.tracer()->info("no ogfusion data available: waiting ...");
                } else {
                    break;
                }
            }
                
            //cout << "got an object with " << data.observation.size() << " cells\n";
            for(unsigned int i=0;i<data.observation.size();i++)
            {
                ogfusion::add(localMap_,data.observation[i]);
            }
            convert(localMap_,map);
                    map.timeStamp=data.timeStamp;
                    ogMapInterface_.localSetData(map);
                        
        } // try
        catch ( const orca::OrcaException & e )
        {
            stringstream ss;
            ss << "unexpected (remote?) orca exception: " << e << ": " << e.what;
            context_.tracer()->error( ss.str() );
        }
        catch ( const orcaice::Exception & e )
        {
            stringstream ss;
            ss << "unexpected (local?) orcaice exception: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch ( const Ice::Exception & e )
        {
            stringstream ss;
            ss << "unexpected Ice exception: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch ( const std::exception & e )
        {
            // once caught this beast in here, don't know who threw it 'St9bad_alloc'
            stringstream ss;
            ss << "unexpected std exception: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch ( ... )
        {
            context_.tracer()->error( "unexpected exception from somewhere.");
        }
    } // end of main loop
    
    
    context_.tracer()->debug( "dropping out from run()", 5 );
    waitForStop();
}

