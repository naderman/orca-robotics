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
#include <QPainter>

#include <hydroqgui/hydroqgui.h>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include "ogmappainter.h"

using namespace std;

namespace orcaqgui2d {

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
//     cout << orcaobj::toVerboseString(data);
    data_ = data;

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
}

ImageFileType
OgMapPainter::checkFileExtension( QString &fe, hydroqgui::IHumanManager *humanManager )
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
        humanManager->showDialogMsg(hydroqgui::IHumanManager::Error, "File extension not supported" );
        return NOT_SUPPORTED;
    }
}

int 
OgMapPainter::saveMap( const orcaice::Context   &context,
                       const QString            &fileName,
                       hydroqgui::IHumanManager *humanManager )
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
            humanManager->showDialogMsg(hydroqgui::IHumanManager::Error, "Could not create ICE_STREAM file " + fileName); 
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
        humanManager->showStatusMsg(hydroqgui::IHumanManager::Information, "Successfully saved ogMap to file: " + fileName);
    }
    
    return 0;
}

}
