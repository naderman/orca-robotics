#include <iostream>
#include <hydromapload/ogmaploadutil.h>
#include <hydroutil/rand.h>
#include <gbxutilacfr/mathdefs.h>

using namespace std;

bool areIdentical( const hydroogmap::OgMap &map1,
                   const hydroogmap::OgMap &map2 )
{
    const double EPS=1e-5;
    bool offsetSame = ( NEAR(map1.offset().p.x, map2.offset().p.x, EPS) &&
                        NEAR(map1.offset().p.y, map2.offset().p.y, EPS) &&
                        NEAR(map1.offset().o  , map2.offset().o,   EPS) );
    bool resolutionSame = NEAR(map1.metresPerCell(), map2.metresPerCell(), EPS);
    bool sizeSame = map1.dataVec().size() == map2.dataVec().size();

    bool identicalConfigs = offsetSame && resolutionSame && sizeSame;

    if ( !identicalConfigs )
    {
        cout<<"TRACE(ogloadsave.cpp): Configs are not identical." << endl;
        cout<<"TRACE(ogloadsave.cpp): map1: " << map1 << endl;
        cout<<"TRACE(ogloadsave.cpp): map2: " << map2 << endl;
        cout<<"TRACE(ogloadsave.cpp): map1.dataVec().size(): " << map1.dataVec().size() << endl;
        cout<<"TRACE(ogloadsave.cpp): map2.dataVec().size(): " << map2.dataVec().size() << endl;

        return false;
    }
    for ( size_t i=0; i < map1.dataVec().size(); i++ )
    {
        if ( map1.data()[i] != map2.data()[i] )
        {
            cout<<"TRACE(ogloadsave.cpp): Data is not identical." << endl;
            cout<<"TRACE(ogloadsave.cpp): for i="<<i<<": map1: "<<(int)(map1.data()[i]) << ", map2: "<<(int)(map2.data()[i]) << endl;

            cout<<"TRACE(ogloadsave.cpp): map1: " << hydroogmap::toText(map1) << endl;
            cout<<"TRACE(ogloadsave.cpp): map2: " << hydroogmap::toText(map2) << endl;

            return false;
        }
    }
    return true;
}

int main()
{
    const int MAP_WIDTH=50;
    hydroogmap::OgMap ogMap( MAP_WIDTH, MAP_WIDTH,
                             hydroogmap::Frame2d( 0, 0, 0 ),
                             0.1,
                             0 );

    //
    // Set random values
    //
    for ( size_t i=0; i < ogMap.dataVec().size(); i++ )
    {
        ogMap.data()[i] = hydroutil::randNum( 0, 254 );
    }

    //
    // Save it
    //
    const std::string filename = "/tmp/ogloadsave.xpm";
    hydromapload::saveMap( filename, ogMap );

    //
    // Re-load it
    //
    hydroogmap::OgMap ogMapCopy;
    const bool negate = false;
    hydromapload::loadMap( filename,
                           ogMapCopy,
                           negate,
                           ogMap.offset().p.x,
                           ogMap.offset().p.y,
                           ogMap.offset().o,
                           ogMap.metresPerCell() );

    //
    // Check that they're the same
    //
    cout<<"TRACE(ogloadsave.cpp): checking..." << endl;
    assert( areIdentical( ogMap, ogMapCopy ) );
}
