/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "digiclopsgrabber.h"
#include <triclops/pnmutils.h> 

#include <iostream>

using namespace std;

namespace orcaimage {

DigiclopsGrabber::DigiclopsGrabber( const orcaice::Context& context )
    : ImageGrabber( context ) 
{
    // TODO: Maybe should implement this using exceptions that wrap up digiclops errors.

    // open the Digiclops
    error_ = digiclopsCreateContext( &digiclopsContext_ );
    if ( error_ )
    {
        cout << "ERROR(digiclopsgrabber.cpp): Digiclops Error - " << digiclopsErrorToString( error_ ) << endl;
        exit(1);
    }
    else
    {
        cout << "Digiclops context created." << endl;
    }

    cout << "TODO(digiclsopgrabber.cpp): fix constructor so it accepts camera index." << endl; 
    // Initialise the digiclops on the 1394 bus and associate it with
    // the previusly created context.
    // 0 here is the index on the 1394 bus so this should be replaced with the index
    error_ = digiclopsInitialize( digiclopsContext_, 0 );
    if ( error_ )
    {
        cout << "ERROR(digiclopsgrabber.cpp): Digiclops Error - " << digiclopsErrorToString( error_ ) << endl;
        exit(1);
    }
    else
    {
        cout << "Digiclops initialised." << endl;
    }
    
    // start streaming images
    error_ = digiclopsStart( digiclopsContext_ );
    if ( error_ )
    {
        cout << "ERROR(digiclopsgrabber.cpp): Digiclops Error - " << digiclopsErrorToString( error_ ) << endl;
        exit(1);
    }
    else
    {
        cout << "Digiclops is streaming images." << endl;
    }


};

DigiclopsGrabber::~DigiclopsGrabber()
{
    // close the digiclops
    error_ = digiclopsStop( digiclopsContext_ );
    if ( error_ )
    {
        cout << "ERROR(digiclopsgrabber.cpp): Digiclops Error - " << digiclopsErrorToString( error_ ) << endl;
        exit(1);
    }
    else
    {
        cout << "Digiclops is stopped." << endl;
    }


    error_ = digiclopsDestroyContext( digiclopsContext_ ); 
    if ( error_ )
    {
        cout << "ERROR(digiclopsgrabber.cpp): Digiclops Error - " << digiclopsErrorToString( error_ ) << endl;
        exit(1);
    }
    else
    {
        cout << "Digiclops context destroyed." << endl;
    }
};

// grabs frame from camera 
int DigiclopsGrabber::grabFrame()
{
    error_ = digiclopsGrabImage( digiclopsContext_ );
    if ( error_ )
    {
        cout << "ERROR(digiclopsgrabber.cpp): Digiclops Error - " << digiclopsErrorToString( error_ ) << endl;
        return -1;
    }
    else
    {
        return 0;
    }
};


// load the image grabbed from grabFrame() into a TriclopsInput struct
// and return the raw image data
char* DigiclopsGrabber::retrieveFrame()
{
    cout<<"TODO(digiclopsgrabber.cpp): add in support for different types of images" << endl;
    // Need to add in LEFT_IMAGE, RIGHT_IMAGE, TOP_IMAGE, ALL_IMAGES
    // this also effects the size of the data arrays.
    error_ = digiclopsExtractTriclopsInput( digiclopsContext_, STEREO_IMAGE, &triclopsInput_ );
    if ( error_ )
    {
        cout << "ERROR(digiclopsgrabber.cpp): Digiclops Error - " << digiclopsErrorToString( error_ ) << endl;
        exit(1);
    }
    
//     char file[] = "test.ppm";
//     bool res = ppmWriteFromTriclopsInput( file , &triclopsInput_ );
//     cout << "res: " << res << endl;

    int imageSize = triclopsInput_.nrows * triclopsInput_.ncols;

    
    // just return the raw image from TriclopsInput (see
    // <$TRICLOPS_HOME>/src/pnmutils/pnmutils.h -
    // ppmReadToTriclopsInput() to see how to do this)
       if ( triclopsInput_.inputType == TriInp_RGB )
       {
           // the R,G, and B values are stored in separate arrays
           unsigned char* red	= (unsigned char*)triclopsInput_.u.rgb.red;
           unsigned char* green	= (unsigned char*)triclopsInput_.u.rgb.green;
           unsigned char* blue	= (unsigned char*)triclopsInput_.u.rgb.blue;

           // image size = 3 (bayer encoded) images (from each
           // camera) of size nrows*ncols
           // cout << "TODO(digiclopsgrabber.cpp): imageSize must be different if STEREO_IMAGE or ALL_IMAGES is called" << endl;

           // put them into one array as planes and not interleaved
//           cout << "data size: " << data_.size() << endl;
            memcpy ( &data_[0], red, imageSize_/3 );
            memcpy( &data_[0]+imageSize, green, imageSize_/3 );
            memcpy( &data_[0]+2*imageSize, blue, imageSize_/3 );
       }
       else if ( triclopsInput_.inputType == TriInp_RGB_32BIT_PACKED )
       {
           cout<<"TRACE(digiclopsgrabber.cpp): TODO: check that the packed triclops image data is being copied into a single array properly" << endl;
           
           // RGB 32bit packed is in the format [B G R U] [B G R U]...
           unsigned char* dataPacked = (unsigned char*)triclopsInput_.u.rgb32BitPacked.data;
           data_.resize(4*imageSize_);
           memcpy ( &data_[0], dataPacked, 4*imageSize_ );
       }

       return (char*)&data_[0];
};

// grabFrame() and retrieveFrame() in one.
char* DigiclopsGrabber::queryFrame()
{
    bool error = grabFrame();
    if ( error )
    {
        cout << "ERROR(digiclopsgrabber.cpp): grabFrame didn't work" << endl;
        exit(1);
    }
    else
    {
        unsigned char* dataTmp_ = (unsigned char*)retrieveFrame();
        return (char*)dataTmp_;
    }
};

// get and set properties for the grabbed frame
int DigiclopsGrabber::width() const
{
    // structure for enumerated resolutions
    DigiclopsInfo info;

    // declare a new error variable as this is a const member function and
    // therefore can't modify any members of this class such as error_
    DigiclopsError error;

    error = digiclopsGetCameraInformation( digiclopsContext_, &info );
    if ( error )
    {
        cout << "ERROR(digiclopsgrabber.cpp): Digiclops Error - " << digiclopsErrorToString( error_ ) << endl;
        exit(1);
    }
    else
    { 
        if ( info.ImageSize == DIGICLOPS_160x120 )
        {
            return 160;
        }
        else if ( info.ImageSize == DIGICLOPS_320x240 )
        {
            return 320;
        }
        else if ( info.ImageSize == DIGICLOPS_640x480 )
        {
            return 640;
        }
        else 
        {
            // if ( info.ImageSize == DIGICLOPS_1024x768 )
            return 1024;
        }
    }
};

// this function also automatically sets the height since you can only
// set it to full or half resolution
void DigiclopsGrabber::setWidth( int w )
{
    // digiclops only has two settings so the user required width must be either of these two
    if ( w == width() )
    {
        error_ = digiclopsSetImageResolution( digiclopsContext_, DIGICLOPS_FULL);
        if ( error_ )
        {
            cout << "ERROR(digiclopsgrabber.cpp): Digiclops Error - " << digiclopsErrorToString( error_ ) << endl;
            exit(1);
        }
    }
    else if ( w == width()/2 )
    {
        error_ = digiclopsSetImageResolution( digiclopsContext_, DIGICLOPS_HALF);
        if ( error_ )
        {
            cout << "ERROR(digiclopsgrabber.cpp): Digiclops Error - " << digiclopsErrorToString( error_ ) << endl;
            exit(1);
        }
    }
    else
    {
        cout << "ERROR(digiclopsgrabber.cpp): image width can only be set to the actual width or half the actual width of the camera resolution." << endl;
        exit(1);
    }
    return;
};

int DigiclopsGrabber::height() const
{
    // structure for enumerated resolutions
    DigiclopsInfo info;

    // declare a new error variable as this is a const member function and
    // therefore can't modify any members of this class such as error_
    DigiclopsError error;

    error = digiclopsGetCameraInformation( digiclopsContext_, &info );
    if ( error )
    {
        cout << "ERROR(digiclopsgrabber.cpp): Digiclops Error - " << digiclopsErrorToString( error_ ) << endl;
        exit(1);
    }
    else
    {
        if ( info.ImageSize == DIGICLOPS_160x120 )
        {
            return 120;
        }
        else if ( info.ImageSize == DIGICLOPS_320x240 )
        {
            return 240;
        }
        else if ( info.ImageSize == DIGICLOPS_640x480 )
        {
            return 480;
        }
        else 
        {
            // if ( info.ImageSize == DIGICLOPS_1024x768 )
            return 768;
        }
    }
};

// this function also automatically sets the width since there are
// only 2 enumerated resolution types
void DigiclopsGrabber::setHeight( int h )
{
    // digiclops only has two settings so the user required width must be either of these two
    if ( h == height() )
    {
        error_ = digiclopsSetImageResolution( digiclopsContext_, DIGICLOPS_FULL);
        if ( error_ )
        {
            cout << "ERROR(digiclopsgrabber.cpp): Digiclops Error - " << digiclopsErrorToString( error_ ) << endl;
            exit(1);
        }
    }
    else if ( h == height()/2 )
    {
        error_ = digiclopsSetImageResolution( digiclopsContext_, DIGICLOPS_HALF);
        if ( error_ )
        {
            cout << "ERROR(digiclopsgrabber.cpp): Digiclops Error - " << digiclopsErrorToString( error_ ) << endl;
            exit(1);
        }
    }
    else
    {
        cout << "ERROR(digiclopsgrabber.cpp): image height can only be set to the actual height or half the actual height of the camera resolution." << endl;
        exit(1);
    }
    return;
};

int DigiclopsGrabber::size() const
{
    DigiclopsError error;
    DigiclopsImageType imageType;
    int imageSize;
    
    int numPixels = width() * height();
    
    error = digiclopsGetImageTypes( digiclopsContext_, &imageType );
    if ( error )
    {
        cout << "ERROR(digiclopsgrabber.cpp): Digiclops Error - " << digiclopsErrorToString( error_ ) << endl;
        exit(1);
    }
    else
    {
        if ( imageType == STEREO_IMAGE )
        {
            // 3 bayer encoded images * number of pixels
            imageSize = 3 * numPixels;
        }
        else if ( imageType == TOP_IMAGE )
        {
            cout << "ERROR(digiclopsgrabber.cpp): image size for top image not implemented yet" << endl;
            exit(1);
        }
        else if ( imageType == LEFT_IMAGE )
        {
            cout << "ERROR(digiclopsgrabber.cpp): image size for left image not implemented yet" << endl;
            exit(1);
        }
        else if ( imageType == RIGHT_IMAGE )
        {
            cout << "ERROR(digiclopsgrabber.cpp): image size for right image not implemented yet" << endl;
            exit(1);
        }
        else
        {
            // if ( imageType == ALL_IMAGES )
            cout << "ERROR(digiclopsgrabber.cpp): image size for all images not implemented yet" << endl;
            exit(1);
        }
    }
    
    DigiclopsOutputImageResolution res;
    error = digiclopsGetImageResolution( digiclopsContext_, &res );
    if ( error )
    {
        cout << "ERROR(digiclopsgrabber.cpp): Digiclops Error - " << digiclopsErrorToString( error_ ) << endl;
        exit(1);
    }
    else
    {
        if ( res == DIGICLOPS_HALF)
        {
            return imageSize/2;
        }
        else
        {
            // if ( res == DIGICLOPS_FULL)
            return imageSize;
        }
    }
};

// frames per second
double DigiclopsGrabber::fps() const
{
    return -1.0;
};

void DigiclopsGrabber::setFps( double fps )
{
    return;
};

// enumerations of formats and modes exist in libdc1394/dc1394_control.h
int DigiclopsGrabber::format() const
{
    return -1;
};

void DigiclopsGrabber::setFormat( int format )
{
    return;
};

int DigiclopsGrabber::mode() const
{
    return -1;
};

void DigiclopsGrabber::setMode( int mode )
{
    if ( mode == orca::ImageFormatDigiclopsStereo )
    {
        cout<<"TODO(digiclopsgrabber.cpp): add in support for different types of images" << endl;
        // Need to add in LEFT_IMAGE, RIGHT_IMAGE, TOP_IMAGE, ALL_IMAGES
        // this also effects the size of the data arrays.
        error_ = digiclopsSetImageTypes( digiclopsContext_, STEREO_IMAGE );
        if ( error_ )
        {
            cout << "ERROR(digiclopsgrabber.cpp): Digiclops Error - " << digiclopsErrorToString( error_ ) << endl;
            exit(1);
        }
        else
        {
            cout << "Digiclops will send a stereo image (i.e. all three images in the one unpacked array)." << endl;

            // set the raw image size according to the mode
            imageSize_ = size();                 
        }
    }
    else if ( mode == orca::ImageFormatDigiclopsRight )
    {
        error_ = digiclopsSetImageTypes( digiclopsContext_, RIGHT_IMAGE );
        if ( error_ )
        {
            cout << "ERROR(digiclopsgrabber.cpp): Digiclops Error - " << digiclopsErrorToString( error_ ) << endl;
            exit(1);
        }
        else
        {
            cout << "Digiclops will send images from the right camera (in a packed array i.e. rgb interleaved)." << endl;

            // set the raw image size according to the mode
            // multiplied by three as we need to store rgbu
            // from each of the 3 cameras
            cout << "TODO(digiclopsgrabber.cpp): imageSize_ may be incorrect" << endl;
            imageSize_ = width() * height() * 4;
        }
    }
    else if ( mode == orca::ImageFormatDigiclopsBoth )
    {
        error_ = digiclopsSetImageTypes( digiclopsContext_, STEREO_IMAGE | RIGHT_IMAGE );
        if ( error_ )
        {
            cout << "ERROR(digiclopsgrabber.cpp): Digiclops Error - " << digiclopsErrorToString( error_ ) << endl;
            exit(1);
        }
        else
        {
            cout << "Digiclops will send both a stereo image and images from the right camera (not sure of the format for this yet?)." << endl;
            cout << "ERROR(digiclopsgrabber.cpp): grabbing the STEREO_IMAGE and RIGHT_IMAGE at the same time hasn't been implemented yet" << endl;
            exit(1);
        }
    }
    else
    {
        cout << "ERROR(digiclopsgrabber.cpp): Input image mode is not supported" << endl;
        exit(1);
    }

    data_.resize( imageSize_ );
    return;
};

double DigiclopsGrabber::brightness() const
{
    return -1.0;
};

void DigiclopsGrabber::setBrightness( double brightness )
{
    return;
};

double DigiclopsGrabber::contrast() const
{
    return -1.0;
};

void DigiclopsGrabber::setContrast( double contrast )
{
    return;
};

double DigiclopsGrabber::saturation() const
{
    return -1.0;
};

void DigiclopsGrabber::setSaturation( double saturation )
{
    return;
};

double DigiclopsGrabber::hue() const
{
    return -1.0;
};

void DigiclopsGrabber::setHue( double hue )
{
    return;
};

double DigiclopsGrabber::gain() const
{
    return -1.0;
};

void DigiclopsGrabber::setGain( double gain )
{
    return;
};



}
