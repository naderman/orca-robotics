
#include "position2d_i.h"
#include <orcaiceutil/objutils.h>

using namespace std;
using namespace orca;
using namespace orcaiceutil;

Position2dI::Position2dI( PtrBuffer<Position2dDataPtr> &posBuffer, Position2dGeometryPtr &geomBuffer) :
	posBuffer_(posBuffer), geomBuffer_(geomBuffer)
{
		
}
	
// served out the data to the client (it was stored here by the driver at the last read)
orca::Position2dDataPtr Position2dI::getData(const Ice::Current& current) const
{
    std::cout << "Sending data back" << std::endl;

    // create a null pointer. data will be cloned into it.
    Position2dDataPtr data;

    posBuffer_.get( data );
   
    return data; 

}

orca::Position2dGeometryPtr Position2dI::getGeometry(const Ice::Current& current) const
{
    std::cout << "Sending the geometry we were initialised with" << std::endl;
   
    return geomBuffer_;
}
