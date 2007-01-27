/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <fstream>
#include <QPainter>

#include <orcaqgui/ihumanmanager.h>
#include <orcaice/orcaice.h>
#include "ogmappainter.h"
#include <orcaqgui/exceptions.h>

using namespace orcaqgui;
using namespace std;


OgMapPainter::OgMapPainter()
{    
    pixmapPainter_ = new PixmapPainter();
}


OgMapPainter::~OgMapPainter()
{
    delete pixmapPainter_;
}

void 
OgMapPainter::paint( QPainter *p, int z )
{
    if (z!=Z_OG_MAP) return;
    pixmapPainter_->paint(p); 
}

void
OgMapPainter::setData( const orca::OgMapData& data )
{
//     cout << orcaice::toVerboseString(data);
    // alexm: why do we save this big mother?
    // tobi: to be able to save to binary file (see below)
    data_ = data;

    if ( data.origin.o != 0.0 ) 
    {
        stringstream ss;
        ss << "OgMapPainter: Don't know how to display non-axis-aligned map: " << orcaice::toString( data );
        throw Exception( ss.str() );
    }
    
    // assemble information to give to pixmapPainter
    PixmapData pixmapData;
    pixmapData.cellSize = QSizeF(data.metresPerCellX,data.metresPerCellY);
    pixmapData.mapSizePix = QSize(data.numCellsX,data.numCellsY);
    pixmapData.origin = QPointF(data.origin.p.x,data.origin.p.y);
    
    for (int i=0; i<(data.numCellsX*data.numCellsY); i++)
    {
        pixmapData.rgbR.push_back(255-data.data[i]);
        pixmapData.rgbG.push_back(255-data.data[i]);
        pixmapData.rgbB.push_back(255-data.data[i]);
    }
    
    pixmapPainter_->setData( pixmapData );
}

ImageFileType
OgMapPainter::checkFileExtension( QString &fe, IHumanManager *humanManager )
{
    if ( fe.isEmpty() ) 
    {
        fe="png";
        return BITMAP;
    }
    else if (fe=="png" || fe=="bmp" || fe=="jpg" || fe=="jpeg" || fe=="ppm" || fe=="xbm" || fe=="xpm")
    {
        return BITMAP;    
    }
    else if (fe=="bin")
    {
        return ICE_STREAM;
    }
    else
    {
        cout << "ERROR(ogmappainter.cpp): File extension not supported" << endl;
        humanManager->showBoxMsg(Error, "File extension not supported" );
        return NOT_SUPPORTED;
    }
}

int 
OgMapPainter::saveMap( const orcaice::Context & context, const QString fileName, IHumanManager *humanManager )
{
    QString fileExtension = fileName.section('.',-1,-1);
    ImageFileType type = checkFileExtension( fileExtension, humanManager );
    
    if ( type == NOT_SUPPORTED )
    {
        return -1;
    }
    else if ( type == BITMAP )
    {
        pixmapPainter_->saveMap( fileName, fileExtension, humanManager );
    }
    else if ( type == ICE_STREAM )
    {
        // create data file
        std::ofstream *dataFile = new ofstream( fileName.toStdString().c_str(),ios::binary );
        if ( !dataFile->is_open() ) 
        {
            cout << "ERROR(ogmappainter.cpp): Could not create data file " << fileName.toStdString() << endl;
            humanManager->showBoxMsg(Error, "Could not create ICE_STREAM file " + fileName); 
            return -1;
        }
                
        // create stream
        vector<Ice::Byte> byteData;
        Ice::OutputStreamPtr outStreamPtr = Ice::createOutputStream( context.communicator() );
        ice_writeOgMapData(outStreamPtr, data_);
        outStreamPtr->writePendingObjects();
        outStreamPtr->finished(byteData);
                
        // write stream to binary file
        size_t length = byteData.size();
        dataFile->write( (char*)&length, sizeof(size_t) );
        dataFile->flush();
        dataFile->write( (char*)&byteData[0], length);
        dataFile->flush();
        dataFile->close();
        delete dataFile;
        cout << "INFO(ogmappainter.cpp): Successfully saved map to file " << fileName.toStdString() << endl;
        humanManager->showStatusMsg(Information, "Successfully saved ogMap to file: " + fileName);
    }
    
    return 0;
}
