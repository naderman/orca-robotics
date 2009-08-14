#include "features.h"
#include <iostream>
#include <gbxutilacfr/mathdefs.h>
#include <hydrogeom2d/geom2d.h>
#include <sstream>

using namespace std;

namespace hydrofeatureobs {

std::string featureTypeToString( int featureType )
{
    switch( featureType )
    {
    case LASERREFLECTOR: return "LASERREFLECTOR";
    case FOREGROUNDPOINT: return "FOREGROUNDPOINT";
    case DOOR: return "DOOR";
    case CORNER: return "CORNER";
    case LINE: return "LINE";
    case INTERNALCORNER: return "INTERNALCORNER";
    case EXTERNALCORNER: return "EXTERNALCORNER";
    case VEHICLEPOSE: return "VEHICLEPOSE";
    case GPSFIX: return "GPSFIX";
    default:
    {
        stringstream ss;
        ss << featureType;
        return ss.str();
    }
    }
}

bool isSane( double pFalsePositive, double pTruePositive )
{
    if ( pFalsePositive < 0.0 || pFalsePositive > 1.0 ) return false;
    if ( pTruePositive < 0.0 || pTruePositive > 1.0 ) return false;
    if ( pFalsePositive > pTruePositive ) return false;
    return true;
}

//////////////////////////////////////////////////////////////////////

std::string
PointFeatureObs::toString() const
{
    stringstream ss;
    ss << "PNT [r="<<range()<<", b="<<bearing()*180.0/M_PI<<"deg]"
       << "(t="<<featureType()<<",pf="<<pFalsePositive()<<",pt="<<pTruePositive()<<",sd="<<rangeSd()<<","<<bearingSd()*180.0/M_PI<<"deg)";
    return ss.str();
}

bool
PointFeatureObs::isSane() const
{
    if ( !hydrofeatureobs::isSane( pFalsePositive(), pTruePositive() ) ) return false;
    if ( range() < 0 ) return false;
    if ( bearing() < -M_PI || bearing() > M_PI ) return false;
    if ( rangeSd() < 0 || bearingSd() < 0 ) return false;
    return true;
}

//////////////////////////////////////////////////////////////////////

std::string
PoseFeatureObs::toString() const
{
    stringstream ss;
    ss << "PSE [r="<<range()<<", b="<<bearing()*180.0/M_PI<<"deg, o="<<orientation()*180.0/M_PI<<"deg]"
       << "(t="<<featureType()<<",pf="<<pFalsePositive()<<",pt="<<pTruePositive()<<",sd="<<rangeSd()<<","<<bearingSd()*180.0/M_PI<<"deg"<<orientationSd()*180.0/M_PI<<"deg)";
    return ss.str();
}

bool
PoseFeatureObs::isSane() const
{
    if ( !hydrofeatureobs::isSane( pFalsePositive(), pTruePositive() ) ) return false;
    if ( range() < 0 ) return false;
    if ( bearing() < -M_PI || bearing() > M_PI ) return false;
    if ( orientation() < -M_PI || orientation() > M_PI ) return false;
    if ( rangeSd() < 0 || bearingSd() < 0 || orientationSd() < 0 ) return false;
    return true;
}

//////////////////////////////////////////////////////////////////////

LineFeatureObs::LineFeatureObs( int featureType, double pFalsePositive, double pTruePositive,
                                double rangeStart, double bearingStart,
                                double rangeEnd, double bearingEnd,
                                bool startSighted, bool endSighted,
                                double rhoSd, double alphaSd )
        : featureType_(featureType),
          pFalsePositive_(pFalsePositive),
          pTruePositive_(pTruePositive),
      start_(rangeStart,bearingStart),
      end_(rangeEnd,bearingEnd),
      startSighted_(startSighted),
      endSighted_(endSighted),
      rhoSd_(rhoSd),
      alphaSd_(alphaSd)
{
    assert( rangeStart >= 0 && rangeEnd >= 0 );

    geom2d::Line line( geom2d::Point(start_.x(),start_.y()), geom2d::Point(end_.x(),end_.y()) );
    geom2d::convertToRhoAlpha( line, rho_, alpha_ );
    assert( rhoSd_ >= 0 && alphaSd_ >= 0 );
}

void
LineFeatureObs::setEndpoints( double rangeStart, double bearingStart,
                              double rangeEnd, double bearingEnd )
{
    start_ = geom2d::PolarPoint( rangeStart, bearingStart );
    end_   = geom2d::PolarPoint( rangeEnd, bearingEnd );

    geom2d::Line line( geom2d::Point(start_.x(),start_.y()), geom2d::Point(end_.x(),end_.y()) );
    geom2d::convertToRhoAlpha( line, rho_, alpha_ );
}

std::string
LineFeatureObs::toString() const
{
    stringstream ss;
    ss << "LIN s=["<<rangeStart()<<","<<bearingStart()*180.0/M_PI<<"deg],e=["
       << rangeEnd()<<","<<bearingEnd()*180.0/M_PI<<"deg] "
       << "(t="<<featureType()<<",pf="<<pFalsePositive()<<",pt="<<pTruePositive()<<")";
    ss << endl << "  (rho="<<rho()<<",a="<<alpha()*180.0/M_PI<<"deg), sd=("<<rhoSd()<<","<<alphaSd()*180.0/M_PI<<"deg)";
   return ss.str();
}

bool
LineFeatureObs::isSane() const
{
    if ( !hydrofeatureobs::isSane( pFalsePositive(), pTruePositive() ) ) return false;

    if ( rangeStart() < 0 ) return false;
    if ( bearingStart() < -M_PI || bearingStart() > M_PI ) return false;

    if ( rangeEnd() < 0 ) return false;
    if ( bearingEnd() < -M_PI || bearingEnd() > M_PI ) return false;

    if ( rho() < 0 ) return false;
    if ( alpha() < -M_PI || alpha() > M_PI ) return false;

    if ( rhoSd() < 0 || alphaSd() < 0 ) return false;

    return true;
}

//////////////////////////////////////////////////////////////////////



}

