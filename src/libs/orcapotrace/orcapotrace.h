#ifndef ORCAPOTRACE_POTRACE_H
#define ORCAPOTRACE_POTRACE_H

#include <hydroogmap/hydroogmap.h>

// Fwd decls
struct potrace_bitmap_s;
struct potrace_param_s;
struct potrace_state_s;
struct potrace_path_s;
struct potrace_dpoint_s;

namespace orcapotrace {

//////////////////////////////////////////////////////////////////////
//!
//! @brief Encapsulates a potrace_bitmap_t
//!
//! @author Alex Brooks
//!
class PotraceBitmap {
public:

    PotraceBitmap( const hydroogmap::OgMap &ogMap );
    ~PotraceBitmap();

    void setPixel( int x, int y, bool on );
    bool getPixel( int x, int y ) const;

    const potrace_bitmap_s *bitmap() const { return bitmap_; }

private:
    potrace_bitmap_s *bitmap_;
    int pixelsPerWord_;
};

//////////////////////////////////////////////////////////////////////
//!
//! @brief Encapsulates a potrace tracer object
//!        (see http://potrace.sourceforge.net)
//!
//! @author Alex Brooks
//!
class Potracer {
public:
    Potracer();
    ~Potracer();

    const potrace_param_s *params() const { return params_; }

    void trace( const potrace_bitmap_s *bitmap );
    const potrace_path_s *paths() const;

private:

    void deleteState();

    potrace_param_s *params_;
    potrace_state_s *state_;
};

std::string toDetailString( const PotraceBitmap &bitmap );

std::string toString( const potrace_bitmap_s &bitmap );
inline std::ostream &operator<<( std::ostream &s, const potrace_bitmap_s &bitmap )
{ return s << toString(bitmap); }

std::string toString( const potrace_dpoint_s &p );
inline std::ostream &operator<<( std::ostream &s, const potrace_dpoint_s &p )
{ return s << toString(p); }

}

#endif
