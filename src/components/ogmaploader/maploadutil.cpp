/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <errno.h>
#include <zlib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <iostream>

#include "maploadutil.h"

using namespace std;

namespace maploadutil {
      
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
        cout << "ERROR(maploadutil.cpp): failed to open image file " << filename << endl;
        return(-1);
    }

    numCellsX = gdk_pixbuf_get_width(pixbuf);
    numCellsY = gdk_pixbuf_get_height(pixbuf);
    cells.resize( numCellsX * numCellsY );
    cout<<"TRACE(maploadutil.cpp): Resized map to " << numCellsX << "x" << numCellsY << endl;

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
    cout<<"TRACE(maploadutil.cpp): Resized map to " << numCellsX << "x" << numCellsY << endl;
    
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
