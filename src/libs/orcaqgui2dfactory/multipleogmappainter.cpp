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

#include "multipleogmappainter.h"

using namespace orcaqgui;
using namespace std;


MultipleOgMapPainter::MultipleOgMapPainter()
{
    pixmapPainter_ = new PixmapPainter();
}


MultipleOgMapPainter::~MultipleOgMapPainter()
{
}

void 
MultipleOgMapPainter::paint( QPainter *p, int z )
{
    if (z!=Z_OG_MAP) return;
    pixmapPainter_->paint(p); 
}

void
MultipleOgMapPainter::setData( const orca::OgMapDataPtr & data0, const orca::OgMapDataPtr & data1 )
{

//     cout << orcaice::toVerboseString(data);

    if ( data0->origin.o != 0.0 ) {
        cout << "ERROR(ogmappainter.cpp): Don't know how to display a non-axis-aligned map." << endl;
        return;
    }
    
    vector<int> occR;
    vector<int> occG;
    vector<int> occB;
    vector<int> hazR;
    vector<int> hazG;
    vector<int> hazB;
    
    for (int i=0; i<(data0->numCellsX*data0->numCellsY); i++)
    {   
        int r=255-(int)data0->data[i];
        int g=255-(int)data0->data[i];
        int b=255-(int)data0->data[i];
        
        occR.push_back(r);
        occG.push_back(g);
        occB.push_back(b);
        //cout << "occ: rgb is " << r << " " << g << " " << b << endl;
        
        r = 255;
        g = (255-(int)data1->data[i]);
        b = (255-(int)data1->data[i]);
        hazR.push_back(r);
        hazG.push_back(g);
        hazB.push_back(b);
        //cout << "haz: rgb is " << r << " " << g << " " << b << endl << endl;
    }
    
    PixmapData pixmapData;
        
    for (int i=0; i<(data0->numCellsX*data0->numCellsY); i++)
    {
        pixmapData.rgbR.push_back( (occR[i]+hazR[i])/2 );
        pixmapData.rgbG.push_back( (occG[i]+hazG[i])/2 );
        pixmapData.rgbB.push_back( (occB[i]+hazB[i])/2 );    
    }
    
    
    // assemble information to give to pixmapPainter
    pixmapData.cellSize = QSizeF(data0->metresPerCellX,data0->metresPerCellY);
    pixmapData.mapSizePix = QSize(data0->numCellsX,data0->numCellsY);
    pixmapData.origin = QPointF(data0->origin.p.x,data0->origin.p.y);
    
    pixmapPainter_->setData( pixmapData );
}

// void
// MultipleOgMapPainter::setData( const orca::OgMapDataPtr & data0, const orca::OgMapDataPtr & data1 )
// {
// 
// //     cout << orcaice::toVerboseString(data);
// 
//     if ( data0->origin.o != 0.0 ) {
//         cout << "ERROR(ogmappainter.cpp): Don't know how to display a non-axis-aligned map." << endl;
//         return;
//     }
//     
//     vector<int> occR;
//     vector<int> occG;
//     vector<int> occB;
//     vector<int> hazR;
//     vector<int> hazG;
//     vector<int> hazB;
//     
//     
// //     occR = data0->data; occG = data0->data; occB = data0->data;
//     
//     for (int i=0; i<(data1->numCellsX*data1->numCellsY); i++)
//     {   
//         int r=255-(int)data0->data[i];
//         int g=255-(int)data0->data[i];
//         int b=255-(int)data0->data[i];
//         
//         occR.push_back(r);
//         occG.push_back(g);
//         occB.push_back(b);
//         cout << "occ: rgb is " << r << " " << g << " " << b << endl;
//         
//         r = 255 - ( ((int)data1->data[i]) + (255-(int)data1->data[i]) );
//         g = (255-(int)data1->data[i]);
//         b = (255-(int)data1->data[i]);
//         hazR.push_back(r);
//         hazG.push_back(g);
//         hazB.push_back(b);
//         cout << "haz: rgb is " << r << " " << g << " " << b << endl << endl;
//     }
// //     hazG = data1->data;
// //     hazB = data1->data;
//     
//     PixmapData pixmapData;
//         
//     for (int i=0; i<(data1->numCellsX*data1->numCellsY); i++)
//     {
// /*        pixmapData.rgbR.push_back( (unsigned char)((int)occR[i]+(int)hazR[i])/2 );
//         pixmapData.rgbG.push_back( (unsigned char)((int)occG[i]+(int)hazG[i])/2 );
//         pixmapData.rgbB.push_back( (unsigned char)((int)occB[i]+(int)hazB[i])/2 );  */  
//         pixmapData.rgbR.push_back( (occR[i]+hazR[i])/2 );
//         pixmapData.rgbG.push_back( (occG[i]+hazG[i])/2 );
//         pixmapData.rgbB.push_back( (occB[i]+hazB[i])/2 );  
//     }
//     
//     
//     // assemble information to give to pixmapPainter
// //     PixmapData pixmapData;
//     pixmapData.cellSize = QSizeF(data0->metresPerCellX,data0->metresPerCellY);
//     pixmapData.mapSizePix = QSize(data0->numCellsX,data0->numCellsY);
//     pixmapData.origin = QPointF(data0->origin.p.x,data0->origin.p.y);
// //     pixmapData.rgbR = data0->data;
// //     pixmapData.rgbG = data1->data;
// //     pixmapData.rgbB = data1->data;
//     
//     pixmapPainter_->setData( pixmapData );
// }

