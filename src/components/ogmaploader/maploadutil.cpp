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
#include <iostream>
#include <sstream>

#include "maploadutil.h"

using namespace std;

namespace maploadutil {

// Load a gzipped pnm image.
int
loadPnmGz( const char *filename, 
           bool negate,
           int &numCellsX,
           int &numCellsY,
           std::vector<unsigned char> &cells );

int
loadBitmap( const char *filename,
            bool negate,
            int &numCellsX,
            int &numCellsY,
            std::vector<unsigned char> &cells );


void
loadMap( const std::string &filename,
         bool negate,
         int &numCellsX,
         int &numCellsY,
         std::vector<unsigned char> &cells )
{
    int len = strlen( filename.c_str() );
    
    // pnm.gz files
    if ( strcmp( &filename.c_str()[len - 7], ".pnm.gz" ) == 0 )
    {
        cout << "TRACE(ogmaploader.cpp): loading " << filename << " as a gzipped PNM image..." << endl;
        if ( loadPnmGz( filename.c_str(), negate, numCellsX, numCellsY, cells ) != 0 )
        {
            stringstream ss;
            ss << "ERROR(ogmaploader.cpp): failed to open " << filename;
            throw( ss.str() );
        }
    }
    else // all other non-zipped bitmap files are loaded using the gdk library
    {
        cout << "TRACE(ogmaploader.cpp): loading " << filename << endl;        
        if ( loadBitmap( filename.c_str(), negate, numCellsX, numCellsY, cells ) != 0 )
        {
            stringstream ss;
            ss << "ERROR(ogmaploader.cpp): failed to open " << filename;
            throw ss.str();
        }
    }
}
      
int
loadBitmap(const char *filename,
           bool negate,
           int &numCellsX,
           int &numCellsY,
           std::vector<unsigned char> &cells )
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

    numCellsX = gdk_pixbuf_get_width(pixbuf);
    numCellsY = gdk_pixbuf_get_height(pixbuf);
    cells.resize( numCellsX * numCellsY );
    cout<<"TRACE(ogmaploader.cpp): Resized map to " << numCellsX << "x" << numCellsY << endl;

    rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    bps = gdk_pixbuf_get_bits_per_sample(pixbuf)/8;
    n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    if(gdk_pixbuf_get_has_alpha(pixbuf))
    {
        n_channels++;
    }
    
    // Read data
    pixels = gdk_pixbuf_get_pixels(pixbuf);
    for(j = 0; j < numCellsX; j++)
    {
        for (i = 0; i < numCellsY; i++)
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
            int yi = numCellsY-1-i;
            int index = numCellsX*(yi) + xi;
            cells[index] = cellVal;
        }
    }
    
    gdk_pixbuf_unref(pixbuf);
    
    return 0;
    
}

    
// Load a gzipped pnm image.
int
loadPnmGz(const char *filename, 
          bool negate,           
          int &numCellsX,
          int &numCellsY,
          std::vector<unsigned char> &cells )
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
    numCellsX = width;
    numCellsY = height;
    cells.resize( numCellsX * numCellsY );
    cout<<"TRACE(ogmaploader.cpp): Resized map to " << numCellsX << "x" << numCellsY << endl;
    
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
            int yi = numCellsY-1-j;
            int index = numCellsX*(yi) + xi;
            cells[index] = cellVal;
        }
    }
    
    gzclose( file );
    
    return 0;    
}

}
