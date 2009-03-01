/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <cmath>
#include <sstream>
#include <orcaqgui3d/glutil.h>
#include <orcaqgui3d/osgutil.h>
#include "gridelement.h"

#include <osg/Node>
#include <osg/Group>
#include <osg/BlendFunc>

#include <osgDB/ReadFile>

using namespace std;

namespace orcaqgui3d {

namespace {

    void makeCheckImage64x64x3( GLubyte img[64][64][3],
                                int numSquaresPerEdge,
                                int lowVal, 
                                int highVal )
    {
        const int widthInPixels=64;
        const int heightInPixels=64;
        assert( lowVal >= 0 && lowVal <= 255 );
        assert( highVal >= 0 && highVal <= 255 );

        int wOn=0;
        int hOn=0;
        for (int i = 0; i < widthInPixels; i++) 
        {
            if ( (i % (widthInPixels/numSquaresPerEdge)) == 0 )
                wOn = wOn ? 0 : 1;

            for (int j = 0; j < heightInPixels; j++) 
            {
                if ( (j % (heightInPixels/numSquaresPerEdge)) == 0 )
                    hOn = hOn ? 0 : 1;

                int c = (wOn^hOn);
                if ( c==0 ) c = lowVal;
                else c = highVal;
                // cout<<"TRACE(glutil.cpp): hOn: " << hOn << ", wOn: " << wOn << ", c: " << c << endl;
                img[i][j][0] = (GLubyte) c;
                img[i][j][1] = (GLubyte) c;
                img[i][j][2] = (GLubyte) c;
            }
        }
    }

    osg::Image *
    createCheckImage()
    {
        osg::Image *img = new osg::Image;
        img->allocateImage( 64, 64, 3, GL_RGB, GL_UNSIGNED_BYTE );

        GLubyte checkImage[64][64][3];

        // Draw the chess-board in memory
        makeCheckImage64x64x3( checkImage, 2, 120, 130 );
        
        // copy to the image
        int n=0;
        for ( uint i=0; i < 64; i++ )
            for ( uint j=0; j < 64; j++ )
                for ( uint k=0; k < 3; k++ )
                    img->data()[n++] = checkImage[i][j][k];
        
        return img;
    }

}

GridElement::GridElement( double wireGridSpacing,
                          double groundPlaneSquareSpacing )
    : wireGridSpacing_(wireGridSpacing),
      groundPlaneSquareSpacing_(groundPlaneSquareSpacing)
{
    root_ = new osg::Group();

    drawGroundPlane();    
    root_->addChild( groundPlaneGeode_.get() );
    
    drawOrigin();
    root_->addChild( originGeode_.get() );

    viewOffset_ = new osg::PositionAttitudeTransform();
    root_->addChild( viewOffset_.get() );

    drawWireGrid();
    viewOffset_->addChild( wireGridGeode_.get() );

//    drawLabels();
}

void
GridElement::setCameraPose( const orcaqgui3d::CoordinateFrame &cameraPose )
{
    const int xOffset = (int)floor(cameraPose.pos().x());
    const int yOffset = (int)floor(cameraPose.pos().y());

    viewOffset_->setPosition( osg::Vec3( xOffset, yOffset, 0 ) );
}

void
GridElement::drawGroundPlane()
{
    //
    // Create the geode
    //
    groundPlaneGeode_ = new osg::Geode;

    //
    // Create the texture
    //
    osg::ref_ptr<osg::Image> checkImage = createCheckImage();

    osg::ref_ptr<osg::Texture2D> checkTexture = new osg::Texture2D;
    // protect from being optimized away as static state:
    checkTexture->setDataVariance(osg::Object::DYNAMIC); 
    checkTexture->setImage( checkImage.get() );

    // Tell the texture to repeat
    checkTexture->setWrap( osg::Texture::WRAP_S, osg::Texture::REPEAT );
    checkTexture->setWrap( osg::Texture::WRAP_T, osg::Texture::REPEAT );

    // Create a new StateSet with default settings: 
    osg::ref_ptr<osg::StateSet> groundPlaneStateSet = new osg::StateSet();

    // Assign texture unit 0 of our new StateSet to the texture 
    // we just created and enable the texture.
    groundPlaneStateSet->setTextureAttributeAndModes(0,checkTexture.get(),osg::StateAttribute::ON);

    // Texture mode
    osg::TexEnv* texEnv = new osg::TexEnv;
    texEnv->setMode(osg::TexEnv::DECAL); // (osg::TexEnv::MODULATE);
    groundPlaneStateSet->setTextureAttribute(0,texEnv);

    // Associate this state set with our Geode
    groundPlaneGeode_->setStateSet(groundPlaneStateSet.get());

    //
    // Create the ground plane
    //
    osg::ref_ptr<osg::Geometry> groundPlaneGeometry = new osg::Geometry();
    
    const double infty=1000;

    const double metresPerTile=2*groundPlaneSquareSpacing_;
    const double texCoordExtreme=2*infty/metresPerTile;

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    groundPlaneGeometry->setVertexArray( vertices.get() );
    
    osg::ref_ptr<osg::Vec2Array> texCoords = new osg::Vec2Array;
    groundPlaneGeometry->setTexCoordArray( 0, texCoords.get() );

    vertices->push_back( osg::Vec3d( -infty, -infty, 0 ) );
    texCoords->push_back( osg::Vec2( 0, 0 ) );
    vertices->push_back( osg::Vec3d(  infty, -infty, 0 ) );
    texCoords->push_back( osg::Vec2( texCoordExtreme, 0 ) );
    vertices->push_back( osg::Vec3d(  infty,  infty, 0 ) );
    texCoords->push_back( osg::Vec2( texCoordExtreme, texCoordExtreme ) );
    vertices->push_back( osg::Vec3d( -infty,  infty, 0 ) );
    texCoords->push_back( osg::Vec2( 0, texCoordExtreme ) );

//     osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
//     colors->push_back(osg::Vec4(0.2, 0.2, 0.2, 1.0) );
//     // colors->push_back(osg::Vec4(0.0, 0.5, 0.0, 1.0) );
//     groundPlaneGeometry->setColorArray(colors.get());
//     groundPlaneGeometry->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);

    osg::ref_ptr<osg::DrawElementsUInt> quad = 
        new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS);
    for ( uint i=0; i < vertices->size(); i++ )
        quad->push_back( i );

    groundPlaneGeometry->addPrimitiveSet( quad.get() );

    groundPlaneGeode_->addDrawable( groundPlaneGeometry.get() );
}

// void
// GridElement::drawLabels()
// {
// //     const int xOffset = view.cameraX();
// //     const int yOffset = view.cameraY();
//     const int xOffset = 0;
//     const int yOffset = 0;
    
//     const int gridLinesEachDirn = 8;
//     const float gridWidth = 2*gridLinesEachDirn*wireGridSpacing_; // [m]

//     for ( int i=-gridLinesEachDirn; i <= gridLinesEachDirn; i++ )
//     {
//         const float d = gridWidth*(float)i/(float)(2*gridLinesEachDirn);
//         if ( i != 0 )
//         {
//             std::stringstream ssX, ssY;
//             ssX << (int)d+xOffset;
//             ssY << (int)d+yOffset;
            
//             osg::ref_ptr<osgText::Text> labelX = new osgText::Text;
//             osg::ref_ptr<osgText::Text> labelY = new osgText::Text;

//             labelX->setText( ssX.str() );
//             labelY->setText( ssY.str() );

//             labelX->setPosition( osg::Vec3(d, 0, 0) );
//             labelY->setPosition( osg::Vec3(0, d, 0) );

//             geode_->addDrawable( labelX.get() );
//             geode_->addDrawable( labelY.get() );
            
//             labels_.push_back( labelX );
//             labels_.push_back( labelY );
//         }
//     }
// }

void
GridElement::drawWireGrid()
{
    osg::ref_ptr<osg::Geometry> wireGridGeometry = new osg::Geometry();

    osg::ref_ptr<osg::Vec3Array> wireGridVertices = new osg::Vec3Array;
    wireGridGeometry->setVertexArray( wireGridVertices.get() );

    osg::ref_ptr<osg::DrawElementsUInt> prim = 
        new osg::DrawElementsUInt(osg::PrimitiveSet::LINES);
    wireGridGeometry->addPrimitiveSet(prim.get());

    const int gridLinesEachDirn = 8;
    const float gridWidth = 2*gridLinesEachDirn*wireGridSpacing_; // [m]
    const float w = gridWidth/2.0;

    for ( int i=-gridLinesEachDirn; i <= gridLinesEachDirn; i++ )
    {
        const float d = gridWidth*(float)i/(float)(2*gridLinesEachDirn);

        // Lines in one direction
        wireGridVertices->push_back( osg::Vec3(-w, d, 0) );
        wireGridVertices->push_back( osg::Vec3( w, d, 0) );

        // Lines at 90deg
        wireGridVertices->push_back( osg::Vec3( d,-w, 0) );
        wireGridVertices->push_back( osg::Vec3( d, w, 0) );
    }
    
    for ( uint i=0; i < wireGridVertices->size(); i++ )
        prim->push_back(i);

    wireGridGeode_ = new osg::Geode();
    wireGridGeode_->addDrawable( wireGridGeometry.get() );

    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(0.0, 0.0, 0.0, 1.0) );
    wireGridGeometry->setColorArray(colors.get());
    wireGridGeometry->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);

//     const int xOffset = view.cameraX();
//     const int yOffset = view.cameraY();

//     glPushMatrix();
//     {
//         // Centre near the camera, slightly off the ground so it's visible
//         // glTranslatef( xOffset, yOffset, 0.001 );
//         glTranslatef( xOffset, yOffset, 0.0 );

//         // Major line every N lines (others are minor)
//         const int majorEveryN=2;
//         const int numMajor=4;
//         const float gridWidth=numMajor*majorEveryN*wireGridSpacing_; // [m]

//         int gridLinesEachDirn=numMajor*majorEveryN;

//         float w = gridWidth/2.0;

//         for(int i=-gridLinesEachDirn; i <= gridLinesEachDirn; i++)
//         {
//             float d = gridWidth*(float)i/(float)(2*gridLinesEachDirn);
//             if(i%(majorEveryN)==0)
//             {
//                 glColor4f(0,0,0,0.5);

//                 if ( isDisplayLabels_ )
//                 {
//                     if ( i != 0 )
//                     {
//                         std::stringstream ssX, ssY;
//                         ssX << (int)d+xOffset;
//                         ssY << (int)d+yOffset;
//                         p.renderText( d, 0, 0, ssX.str().c_str() );
//                         p.renderText( 0, d, 0, ssY.str().c_str() );
//                     }
//                 }
//             }
//             else 
//             {
//                 glColor4f(0.5,0.5,0.5,0.5);
//             }

//             glBegin(GL_LINES);
//             glVertex3f(-w, d, 0);
//             glVertex3f(w,  d, 0);
//             glVertex3f(d, -w, 0);
//             glVertex3f(d,  w, 0);
//             glEnd();
//         }
//     }
//     glPopMatrix();
}

void
GridElement::drawOrigin()
{
    const float scaleAxes=2.0;
    const float d = 0.2*scaleAxes;
    
    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    geometry->setVertexArray( vertices.get() );

    // Axes
    vertices->push_back( osg::Vec3(0,0,0) );
    vertices->push_back( osg::Vec3(d,0,0) );
    vertices->push_back( osg::Vec3(0,0,0) );
    vertices->push_back( osg::Vec3(0,d,0) );
    vertices->push_back( osg::Vec3(0,0,0) );
    vertices->push_back( osg::Vec3(0,0,d) );
    
    const float p=0.1*scaleAxes;
    // x
    vertices->push_back( osg::Vec3(d+p  ,p/2.0,0) );
    vertices->push_back( osg::Vec3(d+2*p,-p/2.0,0) );
    vertices->push_back( osg::Vec3(d+p  ,-p/2.0,0) );
    vertices->push_back( osg::Vec3(d+2*p,p/2.0,0) );

    // y
    vertices->push_back( osg::Vec3(-p/2.0,d+p,0) );
    vertices->push_back( osg::Vec3(p/2.0 ,d+2*p,0) );
    vertices->push_back( osg::Vec3(-p/2.0,d+2*p,0) );
    vertices->push_back( osg::Vec3(0     ,d+1.5*p,0) );

    // z
    vertices->push_back( osg::Vec3(-p/2.0,0,d+p) );
    vertices->push_back( osg::Vec3(p/2.0, 0,d+p) );
    vertices->push_back( osg::Vec3(-p/2.0,0,d+p) );
    vertices->push_back( osg::Vec3(p/2.0, 0,d+2*p) );
    vertices->push_back( osg::Vec3(-p/2.0,0,d+2*p) );
    vertices->push_back( osg::Vec3(p/2.0, 0,d+2*p) );

    osg::ref_ptr<osg::DrawElementsUInt> prim = 
        new osg::DrawElementsUInt(osg::PrimitiveSet::LINES);

    for ( uint i=0; i < vertices->size(); i++ )
        prim->push_back( i );
    geometry->addPrimitiveSet( prim.get() );

    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(0.0, 0.0, 1.0, 1.0) );
    geometry->setColorArray(colors.get());
    geometry->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);

    originGeode_ = new osg::Geode();
    originGeode_->addDrawable( geometry.get() );
}

QStringList
GridElement::contextMenu()
{
    QStringList s;
    s<<"Toggle Grid"<<"Toggle Origin"<<"Toggle Labels"<<"Toggle Ground Plane";
    return s;
}

void
GridElement::execute( int action )
{
    switch ( action )
    {
    case 0 :
    {
        // toggle grid
        if ( viewOffset_->containsNode( wireGridGeode_.get() ) )
            viewOffset_->removeChild( wireGridGeode_.get() );
        else
            viewOffset_->addChild( wireGridGeode_.get() );
        break;
    }
    case 1 :
    {
        // toggle origin
        if ( root_->containsNode( wireGridGeode_.get() ) )
            root_->removeChild( wireGridGeode_.get() );
        else
            root_->addChild( wireGridGeode_.get() );
        break;
    }
    case 2 :
    {
        // toggle labels
        break;
    }
    case 3 :
    {
        // toggle ground plane
        if ( root_->containsNode( groundPlaneGeode_.get() ) )
            root_->removeChild( groundPlaneGeode_.get() );
        else
            root_->addChild( groundPlaneGeode_.get() );
        break;
        break;
    }
    }
}

}
