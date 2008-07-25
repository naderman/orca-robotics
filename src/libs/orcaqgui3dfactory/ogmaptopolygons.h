#ifndef ORCAQGUI3D_OGMAPTOPOLYGONS_H
#define ORCAQGUI3D_OGMAPTOPOLYGONS_H

#include <hydroogmap/ogmap.h>
#include <osg/Geode>

namespace orcaqgui3d {

class GridToPolygonParams {
public:

    GridToPolygonParams()
        : textureUnit(0),
          height(3.0),
          color(0.4,0.1,0.3,1.0)
        {}

    int textureUnit;
    double height;
    osg::Vec4 color;
};

// Returns a geode of polygons corresponding to the 
// OgMap.
// Ignores the offset information (you'll need to apply the offset externally)
osg::ref_ptr<osg::Geode>
convertToPolygons( const hydroogmap::OgMap &ogMap,
                   const GridToPolygonParams &params );

}

#endif
