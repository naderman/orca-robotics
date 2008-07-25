#include "ogmaptopolygons.h"
#include <iostream>
#include <gbxutilacfr/exceptions.h>
#include <cmath>
#include <sstream>
#include <osg/Geometry>
#if HAVE_POTRACE
#ifdef __cplusplus
extern "C" {
#endif
#  include <potrace/potracelib.h>
#ifdef __cplusplus
}
#endif
#endif
#include <orcapotrace/orcapotrace.h>

using namespace std;

namespace orcaqgui3d {

#if !HAVE_POTRACE
void
convertToPolygons( const hydroogmap::OgMap &ogMap )
{
    throw gbxutilacfr::Exception( ERROR_INFO, "cannot convertToPolygons: potrace not found." );
}
#else

namespace {

void
addLine( const potrace_dpoint_t      &start,
         const potrace_dpoint_t      &end,
         osg::ref_ptr<osg::Geode>    &geode,
         const GridToPolygonParams   &params )
{
    cout << "  line: " << orcapotrace::toString(start) << " -> " << orcapotrace::toString(end) << endl;

    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
    geode->addDrawable( geometry.get() );

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    geometry->setVertexArray( vertices.get() );
    osg::ref_ptr<osg::Vec2Array> texCoords = new osg::Vec2Array;
    geometry->setTexCoordArray( params.textureUnit, texCoords.get() );

    vertices->push_back( osg::Vec3( start.x, start.y, 0.0 ) );
    vertices->push_back( osg::Vec3( end.x, end.y, 0.0 ) );
    vertices->push_back( osg::Vec3( end.x, end.y, params.height ) );
    vertices->push_back( osg::Vec3( start.x, start.y, params.height ) );

    texCoords->push_back( osg::Vec2( 0, 0 ) );
    texCoords->push_back( osg::Vec2( 0, 1 ) );
    texCoords->push_back( osg::Vec2( 1, 1 ) );
    texCoords->push_back( osg::Vec2( 1, 0 ) );

    osg::ref_ptr<osg::DrawElementsUInt> prim = 
        new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS);
    geometry->addPrimitiveSet( prim.get() );
    for ( uint i=0; i < vertices->size(); i++ )
        prim->push_back( i );

    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
    colors->push_back( params.color );
    geometry->setColorArray(colors.get());
    geometry->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);    
}

potrace_dpoint_t
scale( const potrace_dpoint_t &p,
       const hydroogmap::OgMap &ogMap )
{
    potrace_dpoint_t sp;

    sp.x = p.x * ogMap.metresPerCellX();
    sp.y = p.y * ogMap.metresPerCellY();

    cout<<"TRACE(ogmaptopolygons.cpp): p: " << orcapotrace::toString(p) << " -> sp: " << orcapotrace::toString(sp) << endl;
    cout<<"TRACE(ogmaptopolygons.cpp): ogMap.worldSizeX(): " << ogMap.worldSizeX() << endl;

    return sp;
}

void
addPolygons( const potrace_curve_t     &curve, 
             osg::ref_ptr<osg::Geode>  &geode,
             const hydroogmap::OgMap   &ogMap,
             const GridToPolygonParams &params )
{
    const int VERTEX   = 1;
    const int ENDPOINT = 2;

    potrace_dpoint_t *start = &(curve.c[curve.n-1][ENDPOINT]);
    potrace_dpoint_t *vertex, *end;

    for ( int i=0; i < curve.n; i++ )
    {
        int type = curve.tag[i];
        if ( type == POTRACE_CURVETO )
        {
            throw gbxutilacfr::Exception( ERROR_INFO,
                                          "Can't deal with curves, only lines and corners" );
        }
        assert( type == POTRACE_CORNER );

        vertex = &(curve.c[i][VERTEX]);
        end    = &(curve.c[i][ENDPOINT]);

        // Need to scale by the ogmap size
        // (from pixel coords to world coords)
        potrace_dpoint_t scaledStart  = scale( *start, ogMap );
        potrace_dpoint_t scaledVertex = scale( *vertex, ogMap );
        potrace_dpoint_t scaledEnd    = scale( *end, ogMap );

        // Create the individual polygons corresponding to the lines
        addLine( scaledStart,  scaledVertex, geode, params );
        addLine( scaledVertex, scaledEnd, geode, params );

        start = end;
    }
}

}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

osg::ref_ptr<osg::Geode>
convertToPolygons( const hydroogmap::OgMap   &ogMap,
                   const GridToPolygonParams &params )
{
    cout<<"TRACE(gridtopolygons.cpp): Creating tracer" << endl;
    orcapotrace::Potracer      potracer;
    cout<<"TRACE(gridtopolygons.cpp): Creating bitmap" << endl;
    orcapotrace::PotraceBitmap bitmap( ogMap );

    cout<<"TRACE(gridtopolygons.cpp): tracing" << endl;
    potracer.trace( bitmap.bitmap() );
    cout<<"TRACE(gridtopolygons.cpp): done tracing" << endl;

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;

    const potrace_path_t *path = potracer.paths();
    int numPaths=0;
    for ( ; path != NULL; path=path->next )
    {
        numPaths++;
//        cout<<"TRACE(gridtopolygons.cpp): path: " << ((char)path->sign) << path->area << endl;
        addPolygons( path->curve, geode, ogMap, params );
    }
    cout<<"TRACE(gridtopolygons.cpp): numPaths: " << numPaths << endl;

    return geode;
}


#endif // !HAVE_POTRACE

}

