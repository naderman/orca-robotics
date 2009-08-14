#include <iostream>
#include <gbxutilacfr/trivialtracer.h>
#include <gbxutilacfr/trivialstatus.h>
#include <hydroutil/hydroutil.h>
#include <hydrolaserfeatureextract/hydrolaserfeatureextract.h>
#include <hydromapload/ogmaploadutil.h>
#include <hydrosim2d/rangescannersimulator.h>
#ifdef NDEBUG
#  undef NDEBUG
#endif

using namespace std;

void
getScan( const char                          *mapFileName,
         const hydroscanutil::ScannerConfig  &scannerConfig,
         std::vector<float>                  &ranges,
         std::vector<unsigned char>          &intensities )
{
    hydroogmap::OgMap ogMap;
    const double offsetX=-1.0, offsetY=-10.0, offsetTheta=0, metresPerCell=0.1;
    hydromapload::loadMap( mapFileName,
                           ogMap,
                           false,
                           offsetX,
                           offsetY,
                           offsetTheta,
                           metresPerCell );
    assert( ogMap.numCellsX() == 200 && ogMap.numCellsY() == 200 );

    hydrosim2d::RangeScannerSimulator::Config simConfig( scannerConfig.maxRange,
                                                         scannerConfig.startAngle,
                                                         scannerConfig.angleIncrement,
                                                         181 );
    hydrosim2d::NullRangeScanPublisher nullPublisher;
    hydrosim2d::RangeScannerSimulator scannerSimulator( simConfig, ogMap, nullPublisher );

    scannerSimulator.getRangesFromPose( hydronavutil::Pose(0,0,0), ranges );

    intensities.resize(181);
    std::fill( intensities.begin(), intensities.end(), 0 );
    intensities[30] = 5;
}

int main( int argc, char **argv )
{
    if ( argc != 2 )
    {
        cout << "USAGE: " << argv[0] << " <map_filename>" << endl;
        exit(1);
    }

    const double maxRange=16.0, startAngle=-90.0*M_PI/180.0, angleIncrement=1.0*M_PI/180.0;
    hydroscanutil::ScannerConfig scannerConfig( maxRange, startAngle, angleIncrement );

    std::vector<float> ranges;
    std::vector<unsigned char>  intensities;
    getScan( argv[1], scannerConfig, ranges, intensities );

    const bool tracerDebug=true;
    gbxutilacfr::TrivialTracer tracer( tracerDebug );
    gbxutilacfr::TrivialStatus status( tracer );
    hydroutil::Properties properties;
    hydroutil::Context context( properties, tracer, status );
    
    //
    // Reflectors
    //
    hydrolfextract::ReflectorExtractor::Config reflectorConfig(scannerConfig);
    hydrolfextract::ReflectorExtractor reflectorExtractor( reflectorConfig );
    std::vector<hydrolfextract::Reflector> reflectors =
        reflectorExtractor.extractReflectors( ranges, intensities );

    cout<<"TRACE(extract.cpp): reflectors: " << endl;
    for ( size_t i=0; i < reflectors.size(); i++ )
        cout << "  " << hydrolfextract::toString( reflectors[i] ) << endl;
    assert( reflectors.size() == 1 );

    //
    // Foreground Points
    //
    std::vector<hydrolfextract::ForegroundExtractor::Config> configs;
    std::vector<int> expectedNumFgPoints;

    configs.push_back( hydrolfextract::ForegroundExtractor::Config(scannerConfig) );
    configs[0].clearForegroundBackgroundSeparation = configs[0].minForegroundBackgroundSeparation;
    expectedNumFgPoints.push_back(2);

    configs.push_back( hydrolfextract::ForegroundExtractor::Config(scannerConfig) );
    expectedNumFgPoints.push_back(2);

    for ( unsigned int i=0; i < configs.size(); i++ )
    {
        cout<<"TRACE(extract.cpp): Config: " << toString(configs[i]) << endl;

        hydrolfextract::ForegroundExtractor fgExtractor( configs[i] );
        const bool debug=true;
        std::vector<hydrolfextract::AnnotatedPole> fgPoints = 
            fgExtractor.extractForegroundPoints( ranges, debug );
    
        cout<<"TRACE(extract.cpp): Foreground points: " << endl;
        for ( size_t j=0; j < fgPoints.size(); j++ )
        {
            cout << "  " << hydrolfextract::toString( fgPoints[j] ) << endl;
            cout << "    globalCS: x="<<fgPoints[j].pole.range*cos(fgPoints[j].pole.bearing)<<", y=" << fgPoints[j].pole.range*sin(fgPoints[j].pole.bearing)<<endl;
        }
        if ( (int)(fgPoints.size()) != expectedNumFgPoints[i] )
        {
            cout<<"TRACE(extract.cpp): expected " << expectedNumFgPoints[i] << " fg points." << endl;
            assert( false && "wrong num fg points." );
        }

        hydrolfextract::ForegroundFeatureExtractor fext( configs[i],
                                                         0.5, 
                                                         1*M_PI/180.0,
                                                         0.2,
                                                         0.8 );
        std::vector<hydrofeatureobs::PointFeatureObs*> features = fext.extractFeatures( ranges, fgPoints );
        for ( size_t j=0; j < features.size(); j++ )
        {
            cout<<"TRACE(extract.cpp): features["<<j<<"]: " << features[j]->toString() << endl;
            delete features[j];
        }
    }
    
    //
    // Doors
    //
    hydrolfextract::DoorExtractor::Config doorConfig( scannerConfig );
    hydrolfextract::DoorExtractor doorExtractor( doorConfig, context );
    std::vector<hydrolfextract::Door> doors = doorExtractor.extractDoors( ranges );
    cout<<"TRACE(extract.cpp): doors: " << endl;
    for ( size_t i=0; i < doors.size(); i++ )
        cout << "  " << hydrolfextract::toString(doors[i]) << endl;
    assert( doors.size() == 2 );

    //
    // Lines
    //
    hydrolfextract::LineExtractor::Config lineConfig( scannerConfig );
    hydrolfextract::LineExtractor lineExtractor( lineConfig, context );
    hydrolfextract::AnnotatedLines annLines = lineExtractor.extractLines( ranges );

    cout<<"TRACE(extract.cpp): Annotated Lines: " << annLines << endl;


    cout<<"TRACE(extract.cpp): test PASSED" << endl;

    return 0;
}
