/*
 *  Orca-Components: Components for robotics.
 *  
 *  Copyright (C) 2004
 *  
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <errno.h>
#include <zlib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "ogmaploader.h"

using namespace std;
using namespace orca;

OgMapLoader::OgMapLoader( std::string filename,
                          float worldSizeX,
                          float worldSizeY,
                          float xOrigin,
                          float yOrigin,
                          float originTheta,
                          bool  negate )
    : ogMap_(new OgMapData),
      hasMap_(false)
{
    ogMap_->origin.p.x = xOrigin;
    ogMap_->origin.p.y = xOrigin;
    ogMap_->origin.o   = originTheta;
    
    if ( loadMap( filename, negate ) == 0 )
    {
        hasMap_ = true;
    }

    // now we know that map size in pixels, we can calculate the cell size
    ogMap_->metresPerCellX = worldSizeX / (float)ogMap_->numCellsX;
    ogMap_->metresPerCellY = worldSizeY / (float)ogMap_->numCellsY;
}

OgMapLoader::~OgMapLoader()
{
}

void
OgMapLoader::getMap(orca::OgMapDataPtr &toMap)
{
    toMap = ogMap_;
}

// Use Glib library to load map
int
OgMapLoader::loadMap( std::string filename,
                          bool  negate )
{
    int len = strlen( filename.c_str() );
    
    // pnm.gz files
    if ( strcmp( &filename.c_str()[len - 7], ".pnm.gz" ) == 0 )
    {
        cout << "TRACE(ogmaploader.cpp): loading " << filename << " as a gzipped PNM image..." << endl;
        if ( this->loadPnmGz( filename.c_str(), negate ) != 0 )
        {
            cout << "ERROR(ogmaploader.cpp): failed to open " << filename << endl;
            return -1;
        }
    }
    else // all other non-zipped bitmap files are loaded using the gdk library
    {
        cout << "TRACE(ogmaploader.cpp): loading " << filename << endl;        
        if (this->loadBitmap( filename.c_str(), negate) != 0 )
        {
            cout << "ERROR(ogmaploader.cpp): failed to open " << filename << endl;
            return -1;
        }
    }
    
    return(0);        
}
      
int
OgMapLoader::loadBitmap(const char *filename, bool negate)
{
    // from Player mapfile.cc
    GdkPixbuf* pixbuf;
    guchar* pixels;
    guchar* p;
    int rowstride, n_channels, bps;
    GError* error = NULL;
    int i,j,k;
    double occ;
    int color_sum;
    double color_avg;

    // Initialize glib
    g_type_init();
    
    // Read the image
    if(!(pixbuf = gdk_pixbuf_new_from_file(filename, &error)))
    {
        cout << "ERROR(ogmaploader.cpp): failed to open image file " << filename << endl;
        return(-1);
    }

    ogMap_->numCellsX = gdk_pixbuf_get_width(pixbuf);
    ogMap_->numCellsY = gdk_pixbuf_get_height(pixbuf);
    ogMap_->data.resize( ogMap_->numCellsX * ogMap_->numCellsY );
    cout<<"TRACE(ogmaploader.cpp): Resized map to " << ogMap_->numCellsX << "x" << ogMap_->numCellsY << endl;

    rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    bps = gdk_pixbuf_get_bits_per_sample(pixbuf)/8;
    n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    if(gdk_pixbuf_get_has_alpha(pixbuf))
    {
        n_channels++;
    }
    
    // Read data
    pixels = gdk_pixbuf_get_pixels(pixbuf);
    for(j = 0; j < ogMap_->numCellsX; j++)
    {
        for (i = 0; i <ogMap_->numCellsY; i++)
        {
            p = pixels + i*rowstride + j*n_channels*bps;
            color_sum = 0;
            for(k=0;k<n_channels;k++)
            {
                color_sum += *(p + (k * bps));
            }
            color_avg = color_sum / (double)n_channels;

            occ = color_avg / 255.0;
        
            if(!negate)
            {
                occ = 1-occ;
            }

            unsigned char cellVal = (unsigned char) (occ*254.0);

            int xi = j;
            int yi = ogMap_->numCellsY-1-i;
            int index = ogMap_->numCellsX*(yi) + xi;
            ogMap_->data[index] = cellVal;
        }
    }
    
    gdk_pixbuf_unref(pixbuf);
    
    return 0;
    
}

    
// Load a gzipped pnm image.
int
OgMapLoader::loadPnmGz(const char *filename, bool negate)
{   
    gzFile file;
    char magic[3];
    char line[1024];
    int i, j;
    int ch;
    int width, height, depth;
    
    // Open file
    file = gzopen( filename, "r" );
    
    if ( file == NULL )
    {
        fprintf( stderr, "%s: %s\n", strerror(errno), filename );
        return -1;
    }
    
    // Read ppm header
    gzgets( file, line, sizeof( line ) );
    sscanf( line, "%10s \n", magic );
    
    if ( strcmp( magic, "P5" ) != 0 )
    {
        fprintf( stderr, "incorrect image format; must be PGM/binary\n" );
        return -1;
    }
    
    // Ignore comments
    while ( ( ch = gzgetc( file ) ) == '#')
    {
        while ( gzgetc( file ) != '\n' );
    }
    
    gzungetc( ch, file );
    
    // Read image dimensions
    gzgets( file, line, sizeof( line ) );
    sscanf( line, " %d %d \n", &width, &height );
    gzgets( file, line, sizeof( line ) );
    sscanf (line, " %d \n", &depth );
    
    // Allocate space in the map
    ogMap_->numCellsX = width;
    ogMap_->numCellsY = height;
    ogMap_->data.resize( ogMap_->numCellsX * ogMap_->numCellsY );
    cout<<"TRACE(ogmaploader.cpp): Resized map to " << ogMap_->numCellsX << "x" << ogMap_->numCellsY << endl;
    
    // Read in the image
    for ( j = 0; j < height; j++ )
    {
        for ( i = 0; i < width; i++ )
        { 
            ch = gzgetc( file );

            double occProb = (double)ch/(double)depth;

            if ( negate )
                occProb = 1-occProb;

            unsigned char cellVal = (unsigned char) (occProb*254.0);
            int xi = i;
            int yi = ogMap_->numCellsY-1-j;
            int index = ogMap_->numCellsX*(yi) + xi;
            ogMap_->data[index] = cellVal;
        }
    }
    
    gzclose( file );
    
    return 0;    
}
