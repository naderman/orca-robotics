#include "opencvviewer.h" 

#include <orcaifaceutil/datetime.h>

using namespace imageview;

OpenCVViewer::OpenCVViewer( const orcaice::Context& context ) :
    Viewer( context )
{
}

OpenCVViewer::~OpenCVViewer()
{
    cvDestroyWindow( name_ );
    cvReleaseImage( &cvImage_ );
}

void OpenCVViewer::initialise()
{
    // cout << "TRACE(imageviewer.cpp):Image Format - " << orcaimage::formatName( descr_.format ) << endl;
    
    // should this be done at the imageserver level and depend on the mode and format?
    // maybe nChannels should be in the Camera object
    // TODO: put this nChannel calculation into imageutils as a separate function 
    
    // default number of channels for a colour image
    int nChannels = 3;
//     int nBayerChannels = 1;   
//     if( descr_.format == orca::ImageFormatBayerBg  || descr_.format == orca::ImageFormatBayerGb 
//             || descr_.format == orca::ImageFormatBayerRg || descr_.format == orca::ImageFormatBayerGr )
//     {
//         // set up an IplImage struct for the Greyscale bayer encoded data
//         bayerImage_  = cvCreateImage( cvSize( descr_.imageWidth, descr_.imageHeight ),  8, nBayerChannels );
//         cout << "Image is Bayer encoded: " << endl;
//         // cout << "bayer encoding: " << format << endl;
//     }
//     else if ( descr_.format == orca::ImageFormatModeGray )
//     {
//         // display image is greyscale therefore only 1 channel      
//         nChannels = 1;
//     }      

    // opencv gear here
    int imageWidth = 640;
    int imageHeight = 480;
    int depth = 8;
    // cvImage_ = cvCreateImage( cvSize( descr_.imageWidth, descr_.imageHeight ),  8, nChannels );
    cvImage_ = cvCreateImage( cvSize( imageWidth, imageHeight ),  depth, nChannels );
    // dodgy opencv needs this so it has time to resize
    cvWaitKey(100);
    
    name_ = "ImageViewer";
    cvNamedWindow( name_ );
    // context_.tracer()->debug("opencv window created",5);
}

void OpenCVViewer::display( orca::ImageDataPtr image )
{

    // check the user hasn't closed the window
    if( cvGetWindowHandle( name_ ) != 0 )
    {
        // copy the image data into the IplImage variable from the orca image variable
        memcpy( cvImage_->imageData, &(image->data[0]), image->data.size() );
        
        // display the image in an opencv window
        cvShowImage( name_, cvImage_ );
    
        // Without this, the window doesn't display propery... what the hell?
        cvWaitKey(5);
    }
    else
    {
        // no window to display in so shutdown the component
        context_.communicator()->shutdown();
    }
    
}
