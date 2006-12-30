/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <QPainter>
#include <orcaice/orcaice.h>

#include "ogmapscombinedpainter.h"

using namespace orcaqgui;
using namespace std;
using namespace orca;


OgMapsCombinedPainter::OgMapsCombinedPainter( const orcaice::Context &context )
{
    Ice::PropertiesPtr prop = context.properties();
    std::string prefix = context.tag();
    prefix += ".Config.";
    Ice::StringSeq strIn; strIn.push_back("ogHazardLookupTable.png"); Ice::StringSeq strOut;
    strOut = orcaice::getPropertyAsStringSeqWithDefault( prop, prefix+"OgMapsCombined.LookupTableFileName", strIn );
    
    lookupTable_ = new QImage( QString(strOut[0].c_str()) );
    assert( !lookupTable_->isNull() && "Lookup table file not found" );
    
    pixmapPainter_ = new PixmapPainter();
}


OgMapsCombinedPainter::~OgMapsCombinedPainter()
{
}

void 
OgMapsCombinedPainter::paint( QPainter *p, int z )
{
    if (z!=Z_OG_MAP) return;
    pixmapPainter_->paint(p); 
}

void
OgMapsCombinedPainter::setData( const orca::OgMapData& data0, const orca::OgMapData& data1 )
{
    // both maps currently need to have the same dimensions
    assert( data0.origin == data1.origin );
    assert( data0.numCellsX == data1.numCellsX );
    assert( data0.numCellsY == data1.numCellsY );
    assert( data0.metresPerCellX == data1.metresPerCellX );
    assert( data0.metresPerCellY == data1.metresPerCellY );
    assert( (data0.mapType==ogmaptype::OCCUPANCY || data0.mapType==ogmaptype::HAZARD)
             && (data1.mapType==ogmaptype::OCCUPANCY  || data1.mapType==ogmaptype::HAZARD) );
    
    if ( data0.origin.o != 0.0 ) {
        cout << "ERROR(ogmapscombinedpainter.cpp): Don't know how to display a non-axis-aligned map." << endl;
        return;
    }
    
    const orca::OgMapData *occMap;
    const orca::OgMapData *hazMap;
    if (data0.mapType==ogmaptype::OCCUPANCY)
    {
        occMap = &data0;
        hazMap = &data1;
    }
    else
    {
        occMap = &data1;
        hazMap = &data0;
    }
    // if both maps are of type occMap then it doesn't matter which one is occMap/hazMap    
    
    PixmapData pixmapData;
    pixmapData.cellSize = QSizeF(occMap->metresPerCellX,occMap->metresPerCellY);
    pixmapData.mapSizePix = QSize(occMap->numCellsX,occMap->numCellsY);
    pixmapData.origin = QPointF(occMap->origin.p.x,occMap->origin.p.y);
    
    QRgb rgb;
    
    for (int i=0; i<occMap->numCellsX*occMap->numCellsY; i++)
    {
        rgb = lookupTable_->pixel(QPoint((int)hazMap->data[i],(int)occMap->data[i]));
        pixmapData.rgbR.push_back( qRed(rgb) );
        pixmapData.rgbG.push_back( qGreen(rgb) );
        pixmapData.rgbB.push_back( qBlue(rgb) );
    }
    
    pixmapPainter_->setData( pixmapData );
}

// void
// OgMapsCombinedPainter::setData( const orca::OgMapData& data0, const orca::OgMapData& data1 )
// {
//     // both maps currently need to have the same dimensions
//     assert( data0.origin == data1.origin );
//     assert( data0.numCellsX == data1.numCellsX );
//     assert( data0.numCellsY == data1.numCellsY );
//     assert( data0.metresPerCellX == data1.metresPerCellX );
//     assert( data0.metresPerCellY == data1.metresPerCellY );
//     assert( (data0.mapType==ogmaptype::OCCUPANCY || data0.mapType==ogmaptype::HAZARD)
//              && (data1.mapType==ogmaptype::OCCUPANCY  || data1.mapType==ogmaptype::HAZARD) );
//     
//     if ( data0.origin.o != 0.0 ) {
//         cout << "ERROR(ogmappainter.cpp): Don't know how to display a non-axis-aligned map." << endl;
//         return;
//     }
//     
//     const orca::OgMapData *occMap;
//     const orca::OgMapData *hazMap;
//     if (data0.mapType==ogmaptype::OCCUPANCY)
//     {
//         occMap = &data0;
//         hazMap = &data1;
//     }
//     else
//     {
//         occMap = &data1;
//         hazMap = &data0;
//     }
//     // if both maps are of type occMap then it doesn't matter which one is occMap/hazMap    
//         
//     
//     vector<int> occR;
//     vector<int> occG;
//     vector<int> occB;
//     vector<int> hazR;
//     vector<int> hazG;
//     vector<int> hazB;
//     
//     for (int i=0; i<(occMap->numCellsX*occMap->numCellsY); i++)
//     {   
//         int r=255-(int)occMap->data[i];
//         int g=255-(int)occMap->data[i];
//         int b=255-(int)occMap->data[i];
//         
//         occR.push_back(r);
//         occG.push_back(g);
//         occB.push_back(b);
//         //cout << "occ: rgb is " << r << " " << g << " " << b << endl;
//         
//         r = 255;
//         g = (255-(int)hazMap->data[i]);
//         b = (255-(int)hazMap->data[i]);
//         hazR.push_back(r);
//         hazG.push_back(g);
//         hazB.push_back(b);
//         //cout << "haz: rgb is " << r << " " << g << " " << b << endl << endl;
//     }
//     
//     PixmapData pixmapData;
//         
//     for (int i=0; i<(occMap->numCellsX*occMap->numCellsY); i++)
//     {
//         pixmapData.rgbR.push_back( (occR[i]+hazR[i])/2 );
//         pixmapData.rgbG.push_back( (occG[i]+hazG[i])/2 );
//         pixmapData.rgbB.push_back( (occB[i]+hazB[i])/2 );    
//     }
//     
//     // assemble information to give to pixmapPainter
//     pixmapData.cellSize = QSizeF(occMap->metresPerCellX,occMap->metresPerCellY);
//     pixmapData.mapSizePix = QSize(occMap->numCellsX,occMap->numCellsY);
//     pixmapData.origin = QPointF(occMap->origin.p.x,occMap->origin.p.y);
//     
//     pixmapPainter_->setData( pixmapData );
// }


