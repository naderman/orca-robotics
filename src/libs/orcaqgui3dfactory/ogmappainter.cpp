/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <fstream>
#include <osg/Texture2D>
#include <osg/TexEnv>
#include <osg/Geometry>
#include <hydroqgui/hydroqgui.h>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include <orcaogmap/orcaogmap.h>
#include <orcaqgui3d/osgutil.h>
#include "ogmappainter.h"

using namespace std;

namespace orcaqgui3d {

namespace {

    osg::Image *
    createImage( const hydroogmap::OgMap &ogMap )
    {
        osg::Image *img = new osg::Image;
        img->allocateImage( ogMap.numCellsX(),
                            ogMap.numCellsY(),
                            3,
                            GL_RGB,
                            GL_UNSIGNED_BYTE );

        // copy to the image (seems to be column-major...)
        int n=0;
        for ( int j=0; j < ogMap.numCellsY(); j++ )
            for ( int i=0; i < ogMap.numCellsX(); i++ )
                for ( int k=0; k < 3; k++ )
                    img->data()[n++] = hydroogmap::CELL_OCCUPIED - ogMap.gridCell(i,j);
        
        return img;
    }

}

OgMapPainter::OgMapPainter()
    : root_(new osg::Group)
{    
}


void
OgMapPainter::setData( const orca::OgMapData& data )
{
    hydroogmap::OgMap ogMap;
    orcaogmap::convert( data, ogMap );

    // Remove the old one
    if ( root_->containsNode( offsetNode_.get() ) )
        root_->removeChild( offsetNode_.get() );

    offsetNode_ = orcaqgui3d::getPositionAttitudeTransform( ogMap.offset().p.x,
                                                            ogMap.offset().p.y,
                                                            0.0,
                                                            0.0,
                                                            0.0,
                                                            ogMap.offset().o );
    root_->addChild( offsetNode_.get() );

    groundPlaneGeode_ = drawAsGroundPlane( ogMap );
    offsetNode_->addChild( groundPlaneGeode_.get() );

//    ::IceInternal::ProxyHandle< orca::OgMap > ogMapPrx;


#if 0
    if ( data.offset.o != 0.0 ) 
    {
        stringstream ss;
        ss << "OgMapPainter: Don't know how to display non-axis-aligned map: " << orcaobj::toString( data );
        throw hydroqgui::Exception( ERROR_INFO, ss.str() );
    }
    
    // assemble information to give to pixmapPainter
    PixmapData pixmapData;
    pixmapData.cellSize = QSizeF(data.metresPerCellX,data.metresPerCellY);
    pixmapData.mapSizePix = QSize(data.numCellsX,data.numCellsY);
    pixmapData.offset = QPointF(data.offset.p.x,data.offset.p.y);
    
    if (data.mapType==orca::OgMapHazard) 
    {
        for (int i=0; i<(data.numCellsX*data.numCellsY); i++)
        {   
            // unoccupied: yellow, occupied: red
            pixmapData.rgbR.push_back(255);
            pixmapData.rgbG.push_back(255-data.data[i]);
            pixmapData.rgbB.push_back(0);
        }
    } 
    else 
    {
        for (int i=0; i<(data.numCellsX*data.numCellsY); i++)
        {
            // unoccupied: white, occupied: black
            pixmapData.rgbR.push_back(255-data.data[i]);
            pixmapData.rgbG.push_back(255-data.data[i]);
            pixmapData.rgbB.push_back(255-data.data[i]);
        }
    }
    
    pixmapPainter_->setData( pixmapData );
#endif
}

osg::ref_ptr<osg::Geode>
OgMapPainter::drawAsGroundPlane( const hydroogmap::OgMap &ogMap )
{
    //
    // Create the geode
    //
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;

    //
    // Create the texture
    //
    osg::ref_ptr<osg::Image> image = createImage( ogMap );

    osg::ref_ptr<osg::Texture2D> checkTexture = new osg::Texture2D;
    // protect from being optimized away as static state:
    checkTexture->setDataVariance(osg::Object::DYNAMIC); 
    checkTexture->setImage( image.get() );

//     // Tell the texture to repeat
//     checkTexture->setWrap( osg::Texture::WRAP_S, osg::Texture::REPEAT );
//     checkTexture->setWrap( osg::Texture::WRAP_T, osg::Texture::REPEAT );

    // Create a new StateSet with default settings: 
    osg::ref_ptr<osg::StateSet> groundPlaneStateSet = new osg::StateSet();

    // Assign texture unit n of our new StateSet to the texture 
    // we just created and enable the texture.
    const int unit = 1;
    groundPlaneStateSet->setTextureAttributeAndModes(unit,checkTexture.get(),osg::StateAttribute::ON);

    // Texture mode
    osg::TexEnv* texEnv = new osg::TexEnv;
    texEnv->setMode(osg::TexEnv::DECAL);
    groundPlaneStateSet->setTextureAttribute(unit,texEnv);

    // Associate this state set with our Geode
    geode->setStateSet(groundPlaneStateSet.get());

    //
    // Create the ground plane
    //
    osg::ref_ptr<osg::Geometry> groundPlaneGeometry = new osg::Geometry();
    
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    groundPlaneGeometry->setVertexArray( vertices.get() );
    
    osg::ref_ptr<osg::Vec2Array> texCoords = new osg::Vec2Array;
    groundPlaneGeometry->setTexCoordArray( unit, texCoords.get() );

    vertices->push_back( osg::Vec3d( 0, 0, 0 ) );
    texCoords->push_back( osg::Vec2( 0, 0 ) );
    vertices->push_back( osg::Vec3d( ogMap.worldSizeX(), 0, 0 ) );
    texCoords->push_back( osg::Vec2( 1, 0 ) );
    vertices->push_back( osg::Vec3d( ogMap.worldSizeX(), ogMap.worldSizeY(), 0 ) );
    texCoords->push_back( osg::Vec2( 1, 1 ) );
    vertices->push_back( osg::Vec3d( 0, ogMap.worldSizeY(), 0 ) );
    texCoords->push_back( osg::Vec2( 0, 1 ) );

    osg::ref_ptr<osg::DrawElementsUInt> quad = 
        new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS);
    for ( uint i=0; i < vertices->size(); i++ )
        quad->push_back( i );

    groundPlaneGeometry->addPrimitiveSet( quad.get() );

    geode->addDrawable( groundPlaneGeometry.get() );    
    
    return geode;
}


}
