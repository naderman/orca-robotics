#ifndef LASERFEATURES_FEATURES_H
#define LASERFEATURES_FEATURES_H

#include <hydrogeom2d/geom2d.h>
#include <assert.h>

namespace hydrofeatureobs {

// Define a few feature types.  These are defined as int's rather than enum, so
// users can define their own.

//! A laser-reflective beacon.
const int LASERREFLECTOR   = 0;
//! Something like a pole or a tree.
const int FOREGROUNDPOINT  = 1;
//! An opening in a wall, represented by the mid-point of the opening.
const int DOOR             = 2;
//! The junction of two lines.
const int CORNER           = 3;
//! A line segment.
const int LINE             = 4;
//! corner on eg the inside of a room
const int INTERNALCORNER   = 5;
//! corner on eg the outside of a building
const int EXTERNALCORNER   = 6;
//! Vehicle pose
const int VEHICLEPOSE      = 7;
//! Gps Fix
const int GPSFIX           = 8;

std::string featureTypeToString( int featureType );

//!
//! @brief The base-class from which all feature observations derive
//!
//! @author Alex Brooks
//!
class FeatureObs
{
public:
    virtual ~FeatureObs() {}

    virtual std::string toString()  const=0;

    virtual int    featureType()    const=0;
    virtual double pFalsePositive() const=0;
    virtual double pTruePositive()  const=0;

    // Allow sanity checks for debugging
    virtual bool isSane() const=0;
};
inline std::ostream &operator<<( std::ostream &s, const FeatureObs &o )
{ return s << o.toString(); }

//! Utility function
bool isSane( double pFalsePositive, double pTruePositive );

//!
//! @brief A range-bearing observation of a single point feature
//!
//! @author Alex Brooks
//!
class PointFeatureObs : public virtual FeatureObs
{
public:
    PointFeatureObs( int featureType,
                     double pFalsePositive,
                     double pTruePositive,
                     double range,
                     double bearing,
                     double rangeSd,
                     double bearingSd )
        : featureType_(featureType),
          pFalsePositive_(pFalsePositive),
          pTruePositive_(pTruePositive),
          range_(range),
          bearing_(bearing),
          rangeSd_(rangeSd),
          bearingSd_(bearingSd)
        { assert(rangeSd>=0 && bearingSd>=0); }

    // const access
    double range()   const { return range_; }
    double bearing() const { return bearing_; }
    double rangeSd()    const { return rangeSd_; }
    double bearingSd()  const { return bearingSd_; }

    // non-const access
    double &range()      { return range_; }
    double &bearing()    { return bearing_; }
    double &rangeSd()    { return rangeSd_; }
    double &bearingSd()  { return bearingSd_; }

    std::string toString() const;

    int    featureType()    const { return featureType_; }
    double pFalsePositive() const { return pFalsePositive_; }
    double pTruePositive()  const { return pTruePositive_; }

    bool isSane() const;

private:
    int   featureType_;
    double pFalsePositive_;
    double pTruePositive_;

    double range_;
    double bearing_;
    double rangeSd_;
    double bearingSd_;
};

//!
//! @brief A range-bearing observation of a single pose feature
//!
//! @author Alex Brooks
//!
class PoseFeatureObs : public virtual FeatureObs
{
public:
    PoseFeatureObs( int featureType,
                    double pFalsePositive,
                    double pTruePositive,
                    double range,
                    double bearing,
                    double orientation,
                    double rangeSd,
                    double bearingSd,
                    double orientationSd )
        : featureType_(featureType),
          pFalsePositive_(pFalsePositive),
          pTruePositive_(pTruePositive),
          range_(range),
          bearing_(bearing),
          orientation_(orientation),
          rangeSd_(rangeSd),
          bearingSd_(bearingSd),
          orientationSd_(orientationSd)
        { assert(rangeSd>=0 && bearingSd>=0); }

    // const access
    double range()       const { return range_; }
    double bearing()     const { return bearing_; }
    double orientation() const { return orientation_; }
    double rangeSd()        const { return rangeSd_; }
    double bearingSd()      const { return bearingSd_; }
    double orientationSd() const { return orientationSd_; }

    // non-const access
    double &range()         { return range_; }
    double &bearing()       { return bearing_; }
    double &orientation()   { return orientation_; }
    double &rangeSd()       { return rangeSd_; }
    double &bearingSd()     { return bearingSd_; }
    double &orientationSd() { return orientationSd_; }

    std::string toString() const;

    int    featureType()    const { return featureType_; }
    double pFalsePositive() const { return pFalsePositive_; }
    double pTruePositive()  const { return pTruePositive_; }

    bool isSane() const;

private:
    int   featureType_;
    double pFalsePositive_;
    double pTruePositive_;

    double range_;
    double bearing_;
    double orientation_;
    double rangeSd_;
    double bearingSd_;
    double orientationSd_;
};

//!
//! @brief An observation of a single line.
//!
//! Observations of lines are represented with two endpoints.  
//!
//! It's a bit more complicated though: the endpoints will not always
//! be visible.  So the observation endpoints are set to the most
//! extreme points that can be seen.  The 'sighted' flags are set if the 
//! observer is confident that the end of the line can be seen.
//!
//! @author Alex Brooks
//!
class LineFeatureObs : public virtual FeatureObs
{
public:
    LineFeatureObs( int featureType, double pFalsePositive, double pTruePositive,
                    double rangeStart, double bearingStart,
                    double rangeEnd, double bearingEnd,
                    bool startSighted, bool endSighted,
                    double rhoSd, double alphaSd );

    double rangeStart()   const { return start_.range(); }
    double bearingStart() const { return start_.bearing(); }
    double rangeEnd()     const { return end_.range(); }
    double bearingEnd()   const { return end_.bearing(); }

    void setEndpoints( double rangeStart, double bearingStart,
                       double rangeEnd, double bearingEnd );

    bool startSighted() const { return startSighted_; }
    bool endSighted() const { return endSighted_; }

    // polar observation of infinite line
    double rho()   const { return rho_; }
    double alpha() const { return alpha_; }

    // uncertainty
    double rhoSd()   const { return rhoSd_; }
    double alphaSd() const { return alphaSd_; }

    std::string toString() const;

    virtual int    featureType()    const { return featureType_; }
    virtual double pFalsePositive() const { return pFalsePositive_; }
    virtual double pTruePositive()  const { return pTruePositive_; }

    bool isSane() const;

private:

    int   featureType_;
    double pFalsePositive_;
    double pTruePositive_;

    geom2d::PolarPoint start_, end_;

    bool startSighted_;
    bool endSighted_;

    // obs of infinite line
    double rho_, alpha_;
    double rhoSd_, alphaSd_;
};

}

#endif
