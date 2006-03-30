/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "fakedriver.h"
#include <orca/camera.h>
#include <orcaice/orcaice.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

namespace imageserver {

FakeDriver::FakeDriver()
{
}

FakeDriver::~FakeDriver()
{
    disable();
}

int 
FakeDriver::read( orca::CameraDataPtr &data )
{
    cout<<"TRACE(fakedriver.cpp): Generating fake camera data..." << endl;

    orcaice::setToNow( data->timeStamp );
    
    // initialise values for the camera object
    data->imageWidth = 640;
    data->imageHeight = 480;
    data->format = orca::MODEBGR;
    data->compression = orca::COMPRESSIONTYPENONE;
    cout << "TODO: resize image properly for different modes" << endl;
    int imageSize = (int)ceil( 3 * data->imageHeight * data->imageWidth );
    data->image.resize( imageSize );

    //! @TODO send a real image
    bool random_data = true;
    if ( random_data )
    {
        // fill the image with a random colour
        int xr=rand()%256, xg=rand()%256, xb=rand()%256;
        fill( data->image, (unsigned char)xr, (unsigned char)xg, (unsigned char)xb );

        cout << "opencv uses BGR format rather than rgb: " << endl; 
        cout<<"B:G:R  ("<<xr<<":"<<xg<<":"<<xb<<")"<<endl;
    }
    else 
    {
        // use a real image from file

        ifstream file;
        // TODO: this should be loaded in from a .def file
        string filename = "/opt/empty-project-0.0.1/images/penguin.jpg";
        file.open(filename.c_str());
        if ( file.is_open() )
        {
            // if file exists close it and load into cv structure
            file.close();
            cvImage_ = cvLoadImage( filename.c_str()  );
            //cout << "image loaded... " << endl;
        }
        else 
        {
            cout << "ERROR(fakedriver.cpp): \""<< filename << "\""<< " does not exist." << endl;
            exit(1);
        }

        // copy image data into ice object
        memcpy( &data->image[0], cvImage_->imageData, cvImage_->imageSize );
        cout << "Image copied into ice object" << endl;
    }

    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    return 0;
}

int 
FakeDriver::setConfig( const orca::CameraConfigPtr &cfg )
{
    isEnabled_ = cfg->isEnabled;
    return 0;
}

int 
FakeDriver::getConfig( orca::CameraConfigPtr &cfg )
{
    return 0;
}

int 
FakeDriver::fill( orca::ByteSequence& image, const unsigned char R, const unsigned char G, const unsigned B )
{
    orca::ByteSequence::iterator i;
    for ( i= image.begin(); i != image.end(); i+=3 )
    {
        *i = B;
        *(i+1) = G;
        *(i+2) = R;
    }
    return 0;
}


}
