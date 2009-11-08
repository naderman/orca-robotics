#ifndef SCANUTIL_H
#define SCANUTIL_H

#include <string>

namespace hydroscanutil {

struct ScannerConfig {
    ScannerConfig() {}
    ScannerConfig( double scannerMaxRange,
                   double scannerStartAngle,
                   double scannerAngleIncrement,
                   int    scannerNumReturns )
        : maxRange(scannerMaxRange),
          startAngle(scannerStartAngle),
          angleIncrement(scannerAngleIncrement),
          numReturns(scannerNumReturns)
        {}
    double maxRange;
    double startAngle;
    double angleIncrement;
    int    numReturns;

    bool isSane() const;
};
std::string toString( const ScannerConfig &c );
inline std::ostream &operator<<( std::ostream &s, const ScannerConfig &c )
{ return s << toString(c); }

inline double calcAngleIncrement( double fieldOfView,
                                  int    numReturns )
{ return fieldOfView / double(numReturns-1); }

inline double scanBearing( double startAngle,
                           double angleIncrement,
                           int i )
{ return (startAngle + angleIncrement*i); }

inline double scanBearing( const ScannerConfig &cfg,
                           int i )
{ return scanBearing( cfg.startAngle, cfg.angleIncrement, i ); }

inline bool isValidRange( double range,
                          const ScannerConfig &cfg )
{
    const double EPS = 1e-9;
    return ( range > 0.0 && range < cfg.maxRange-EPS );
}

}

#endif
