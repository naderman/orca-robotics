#ifndef HYDROTIME_TIME_H
#define HYDROTIME_TIME_H

#include <string>
#include <cmath>

namespace hydrotime {

//!
//! @brief Lightweight time class with minimal dependencies
//!
//! @author Alex Brooks
//!
class Time
{

public: 

    Time() {}
    Time( int seconds, int useconds )
        : seconds_(seconds), useconds_(useconds) {}

    int seconds() const { return seconds_; }
    int useconds() const { return useconds_; }

    int &seconds() { return seconds_; }
    int &useconds() { return useconds_; }

private: 

    int seconds_;
    int useconds_;
};

//! returns the time right now
Time getNow();

//! Returns the difference between two time values as a double (in seconds) (ie: t1-t2)
//! The result is negative if t1 is earlier than t2.
inline double timeDiffAsDouble( const Time &t1, const Time &t2 )
{ return ( double(t1.seconds() - t2.seconds()) + 1.0e-6 * double(t1.useconds() - t2.useconds()) ); }

//! Converts from number of seconds to Time.
//! Time can be negative, in which case both seconds and
//! microseconds are returned as negatives,
//! e.g. -10.1s will translate to -10s:-100000us,
//! not -11s:900000us.
Time toTime( double seconds );

//! Returns the difference between two time values (ie: t1-t2)
//! The result is negative if t1 is earlier than t2.
inline Time timeDiff( const Time &t1, const Time &t2 )
{ return toTime( timeDiffAsDouble( t1, t2 ) ); }

//! Converts a hydrotime::Time to a double in seconds.
//! Time can be negative, but remember that seconds and
//! microseconds are added to together.
//! e.g. -10s:-100000us and -11s:900000us will both translate to -10.1s
inline double timeAsDouble( const Time &t )
{ return ((double)(t.seconds()) + 1.0e-6 * (double)(t.useconds())); }

//! Adds the specified number of seconds to t
void add( Time &t, double secondsAsDouble );
//! Subtracts the specified number of seconds from t
void subtract( Time &t, double secondsAsDouble );
//! Adds the specified time to t1
void add( Time &t1, const Time &t2 );
//! Subtracts the specified time from t1
void subtract( Time &t1, const Time &t2 );

std::string toString( const Time &t );
inline std::ostream &operator<<( std::ostream &s, const Time &t )
{ return s << toString(t); }

std::string toStringAsSecUSec( const Time &t );

//////////////////////////////////////////////////////////////////////
// operators
//////////////////////////////////////////////////////////////////////

// add
inline Time operator+( const Time &t, double secondsToAdd )
{ Time result=t; add(result,secondsToAdd); return result; }
inline Time operator+( const Time &t1, const Time &t2 )
{ Time result=t1; add(result,t2); return result; }

inline void operator+=( Time &t1, const double secondsToAdd )
{ t1 = t1+secondsToAdd; }
inline void operator+=( Time &t1, const Time &t2 )
{ t1 = t1+t2; }

// subtract
inline Time operator-( const Time &t, double secondsToSubtract )
{ Time result=t; subtract(result,secondsToSubtract); return result; }
inline Time operator-( const Time &t1, const Time &t2 )
{ Time result=t1; subtract(result,t2); return result; }

// compare
inline bool operator>( const Time &t1, const Time &t2 )
{ return timeDiffAsDouble( t1, t2 ) > 0; }
inline bool operator<( const Time &t1, const Time &t2 )
{ return timeDiffAsDouble( t1, t2 ) < 0; }
inline bool operator==( const Time &t1, const Time &t2 )
{ return t1.seconds()==t2.seconds() && t1.useconds()==t2.useconds(); }
inline bool operator!=( const Time &t1, const Time &t2 )
{ return !(t1==t2); }
inline bool operator<=( const Time &t1, const Time &t2 )
{ return !(t1>t2); }
inline bool operator>=( const Time &t1, const Time &t2 )
{ return !(t1<t2); }

inline bool near( const Time &t1, const Time &t2, double secondsTolerance )
{ return fabs(timeDiffAsDouble(t1,t2)) <= secondsTolerance; }

inline Time min( const Time &t1, const Time &t2 )
{ if (t1<t2) return t1; else return t2; }
inline Time max( const Time &t1, const Time &t2 )
{ if (t1>t2) return t1; else return t2; }


}

#endif
