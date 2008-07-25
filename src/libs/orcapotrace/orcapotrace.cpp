#include "orcapotrace.h"

#ifdef __cplusplus
extern "C" {
#endif
#  include <potrace/potracelib.h>
#ifdef __cplusplus
}
#endif

#include <potrace/potracelib.h>
#include <iostream>
#include <sstream>
#include <gbxutilacfr/exceptions.h>

using namespace std;

namespace orcapotrace {

PotraceBitmap::PotraceBitmap( const hydroogmap::OgMap &ogMap )
    : bitmap_(new potrace_bitmap_t)
{
    // Allocate
    bitmap_->w = ogMap.numCellsX();
    bitmap_->h = ogMap.numCellsY();

    pixelsPerWord_ = 8*sizeof( potrace_word );
    bitmap_->dy = (int)(ceil(bitmap_->w/(float)pixelsPerWord_));

    bitmap_->map = new potrace_word[bitmap_->h*bitmap_->dy];

    // Set
    for ( int y=0; y < ogMap.numCellsY(); y++ )
    {
        for ( int x=0; x < ogMap.numCellsX(); x++ )
        {
            const bool on = (ogMap.gridCell(x,y)>=hydroogmap::CELL_UNKNOWN);
            setPixel( x, y, on );
            assert( getPixel(x,y) == on );
        }
    }       
}

PotraceBitmap::~PotraceBitmap()
{
    delete[] bitmap_->map;
    delete bitmap_;
}

void 
PotraceBitmap::setPixel( int x, int y, bool on )
{
    assert( x <= bitmap_->w );
    assert( y <= bitmap_->h );

    const int wordI = (y*bitmap_->dy) + x/pixelsPerWord_;
    potrace_word &word = bitmap_->map[wordI];

    const int pixelPos = (pixelsPerWord_-1-x%pixelsPerWord_);
    const potrace_word mask = (1 << pixelPos);

    if ( on )
        word = word | mask;
    else
        word = word & (~mask);
}

bool
PotraceBitmap::getPixel( int x, int y ) const
{
    assert( x < bitmap_->w );
    assert( y < bitmap_->h );

    const int wordI = (y*bitmap_->dy) + x/pixelsPerWord_;
    const potrace_word &word = bitmap_->map[wordI];

    const int pixelPos = (pixelsPerWord_-1-x%pixelsPerWord_);
    const potrace_word mask = (1 << pixelPos);

    return word & mask;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

Potracer::Potracer()
    : params_(potrace_param_default()),
      state_(NULL)
{
    params_->alphamax = -1.0;
}

Potracer::~Potracer()
{
    potrace_param_free(params_); 
    deleteState();    
}
void
Potracer::trace( const potrace_bitmap_s *bitmap )
{
    deleteState();

    state_ = potrace_trace( params_,
                            bitmap );
            
    if ( state_->status != POTRACE_STATUS_OK )
    {
        deleteState();
        throw gbxutilacfr::Exception( ERROR_INFO,
                                      "Tracing failed." );
    }
}
const potrace_path_t *
Potracer::paths() const
{
    assert( state_ != NULL );
    return state_->plist;
}
void
Potracer::deleteState()
{ 
    if ( state_ != NULL )
    {
        potrace_state_free( state_ );
        state_ = NULL;
    }
}


//////////////////////////////////////////////////////////////////////

std::string toDetailString( const PotraceBitmap &b )
{
    stringstream ss;
    for ( int x=0; x<b.bitmap()->w; x++ )
        ss << (x%10);
    ss << endl;
    for ( int y=0; y<b.bitmap()->h; y++ )
    {
        for ( int x=0; x<b.bitmap()->w; x++ )
        {
            bool occ = b.getPixel(x,y);
            if ( occ ) ss << "x";
            else ss << ".";
        }
        ss << endl;
    }
    return ss.str();
}
std::string toString( const potrace_bitmap_s &bitmap )
{
    stringstream ss;
    ss << " w=" << bitmap.w << ", h=" << bitmap.h << ", dy=" << bitmap.dy;
    ss << " (sizeof(potrace_word)="<<sizeof(potrace_word)<<")";
    return ss.str();
}
std::string toString( const potrace_dpoint_s &p )
{
    stringstream ss;
    ss << "[ " << p.x << ", " << p.y << " ]";
    return ss.str();
}

}

