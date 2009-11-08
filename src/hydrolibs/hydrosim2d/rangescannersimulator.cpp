#include "rangescannersimulator.h"
#include <iostream>
#include <sstream>

using namespace std;

namespace hydrosim2d {

bool
isSane( const RangeScannerSimulator::Config &config, std::string &reason )
{
    stringstream ss;

    if ( config.maxRange < 0.0 )
    {
        reason = "maxRange < 0"; 
        return false;
    }
    if ( config.startAngle < -M_PI || config.startAngle > M_PI )
    {
        ss << "startAngle < M_PI || startAngle > M_PI: " << config.startAngle*180.0/M_PI <<"deg";
        reason = ss.str();
        return false;
    }
    if ( config.angleIncrement < 0.0 || config.angleIncrement > 2*M_PI )
    {
        reason = "angleIncrement < 0 || angleIncrement > 2*M_PI"; 
        return false;
    }
    if ( config.numReturns <= 0 )
    {
        reason = "numReturns <= 0"; 
        return false;
    }
    if ( config.angleIncrement*config.numReturns > 2*M_PI )
    {
        reason = "angleIncrement*numReturns > 2*M_PI"; 
        return false;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////

RangeScannerSimulator::RangeScannerSimulator( const Config                          &config,
                                              const hydroogmap::OgMap               &ogMap,
                                              hydropublish::RangeScanner2dPublisher &rangeScanPublisher )
    : config_(config),
      ogMap_(ogMap),
      rayTracer_(ogMap),
      rangeScanPublisher_(rangeScanPublisher)
{
    std::string reason;
    if ( !isSane( config, reason ) )
    {
        cout<<"TRACE(rangescannersimulator.cpp): reason: " << reason << endl;
        stringstream ss;
        ss << "RangeScannerSimulator: bad config: " << reason;
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
}

void 
RangeScannerSimulator::getRangesFromPose( const hydronavutil::Pose &sensorPose,
                                          std::vector<float>       &ranges,
                                          const hydrotime::Time    &time ) const
{
    ranges.resize( config_.numReturns );

    for ( int i=0; i < config_.numReturns; i++ )
    {
        double thetaSensor = config_.startAngle + i*config_.angleIncrement;
        double thetaRay = sensorPose.theta() + thetaSensor;

        const double maxEndX = sensorPose.x() + config_.maxRange*cos(thetaRay);
        const double maxEndY = sensorPose.y() + config_.maxRange*sin(thetaRay);
        
        double endX, endY;
        bool isClear = rayTracer_.isClearWorld( sensorPose.x(), sensorPose.y(),
                                                maxEndX, maxEndY,
                                                endX, endY );
        if ( isClear )
            ranges[i] = config_.maxRange;
        else
            ranges[i] = hypot( endY-sensorPose.y(),
                               endX-sensorPose.x() );
    }

    rangeScanPublisher_.localSetAndSend( ranges, time );
}

}

