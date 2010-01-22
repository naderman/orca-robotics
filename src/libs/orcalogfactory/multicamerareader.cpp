/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Alen Alempijevic, Michael Warren
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>

#include <orcaifacelog/multicamera.h>

#include <hydroimage/formats.h>

#include <sstream>  // for auto-generated (ascii) to/fromStream functions

#ifdef OPENCV_FOUND
    #include <hydroimageocvutil/colourconversions.h>
    using namespace hydroimageocvutil;
#endif
    
#include "logstringutils.h"
#include "multicamerareader.h"

using namespace std;
using namespace orcalogfactory;
using namespace hydroimage;

MultiCameraReader::MultiCameraReader()
{
}

MultiCameraReader::~MultiCameraReader()
{
#ifdef OPENCV_FOUND
    // Cleanly release any avi video captures we may have made
    map<std::string,CvCapture*>::iterator it;
    for ( it = cvCapture_.begin() ; it != cvCapture_.end(); it++ )
    {
        if (&((*it).second) != NULL) cvReleaseCapture(&((*it).second));
    }
#endif
}

void
MultiCameraReader::initDataStorage()
{
    // Create new MultiCameraData object
    data_ = new orca::MultiCameraData();
#ifdef OPENCV_FOUND
    // Resize variables appropriately
    cvImage_.resize(descr_->descriptions.size(), NULL);

    for( unsigned int i = 0; i < descr_->descriptions.size(); ++i)
    {
        data_->cameraDataVector.push_back( new orca::ImageData() );
        data_->cameraDataVector[i]->description = descr_->descriptions[i];
        
        // class to search for image format properties
        ImageFormat imageFormat = ImageFormat::find(descr_->descriptions[i]->format);
        int nChannels = imageFormat.getNumberOfChannels();

        // resize object buffer to fit image
        int imageSize = (int)ceil( nChannels * data_->cameraDataVector[i]->description->height *data_->cameraDataVector[i]->description->width );
        data_->cameraDataVector[i]->pixelData.resize( imageSize );
    }
#endif
}

void
MultiCameraReader::readFromFile( std::ifstream *file,
                              const std::string &format,
                              orcaice::Context context,
                              orca::MultiCameraDescriptionPtr &obj )
{
    descr_ = new orca::MultiCameraDescription();
    if ( format=="ice" )
    {
        orcalog::IceReadHelper helper( context.communicator(), file );
        ice_readMultiCameraDescription( helper.stream_, obj );
        helper.read();
    }
    else if ( format=="jpeg" || format=="bmp" || format=="avi")
    {
#ifndef OPENCV_FOUND
        std::stringstream infostring;
        infostring << "Images can only be replayed in '" << format << "' format if you have OpenCV.";
        context.tracer().info( infostring.str() );
        context.tracer().info( "Please have a look at the documentation for installing OpenCV." );
        std::stringstream errorstring;
        errorstring << "Logger: '"<< format <<"' format not supported because OpenCV is not installed.";
        throw orcalog::FormatNotSupportedException( ERROR_INFO, errorstring.str() );
#endif

        // Populate the description with data from the file
        std::string line;
        std::getline( *file, line );

        std::stringstream ss( line );
        fromLogString( ss, descr_ );

        // Point the descriptor pointer to our copy of it
        obj = descr_;
    }
    else
    {
        stringstream ss;
        ss <<  "can't handle format: " << format;
        throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
    }

    // Now that we know the size, initialise the data storage
    initDataStorage();

}

void
MultiCameraReader::readFromFile( std::ifstream *file,
                              const std::string &format,
                              orcaice::Context context,
                              orca::MultiCameraDataPtr &obj )
{
    if ( format=="ice" )
    {
        orcalog::IceReadHelper helper( context.communicator(), file );
        ice_readMultiCameraData( helper.stream_, obj );
        helper.read();
    }
    else if ( format=="jpeg" || format == "bmp" )
    {

#ifndef OPENCV_FOUND
        std::stringstream infostring;
        infostring << "Images can only be replayed in '"<< format <<"' format if you have OpenCV.";
        context.tracer().info( infostring.str() );
        context.tracer().info( "Please have a look at the documentation for installing OpenCV." );
        std::stringstream errorstring;
        errorstring << "Logger: '"<< format << "' format not supported because OpenCV is not installed.";
        throw orcalog::FormatNotSupportedException( ERROR_INFO, errorstring.str() );
#endif

        // Pull the next line from the data log file
        std::string line;
        std::getline( *file, line );

        // Fill the MultiCameraDataObject with the data
        std::stringstream ss( line );
        fromLogString( ss, data_ );

        // Load the correct images using the filenames provided
        loadImageData( ss, data_);

        // Point the descriptor pointer to our copy
        obj = data_;
    }
    else if ( format=="avi" )
    {

#ifndef OPENCV_FOUND
            context.tracer().info( "Data can only be replayed in 'avi' format if you have OpenCV." );
            context.tracer().info( "Please have a look at the documentation for installing OpenCV." );
            throw orcalog::FormatNotSupportedException( ERROR_INFO, "Logger: 'avi' format not supported because OpenCV is not installed." );
#endif

        // Pull the next line from the data log file
        std::string line;
        std::getline( *file, line );

        // Fill the MultiCameraDataObject with the data
        std::stringstream ss( line );
        fromLogString( ss, data_ );

        // Load the correct images using the filenames provided
        loadVideoData( ss, data_ );

        // Point the descriptor pointer to our copy
        obj = data_;
    }
    else
    {
        stringstream ss;
        ss <<  "can't handle format: " << format;
        throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
    }
}

void 
MultiCameraReader::loadImageData( std::stringstream &ss,
                            orca::MultiCameraDataPtr&  obj )
{
#ifdef OPENCV_FOUND
    {
        for (unsigned int i = 0; i < cvImage_.size(); i++) {
            // Get the filename
            std::string filename;
            ss >> filename;
            // load compressed image into opencv struct
            cvImage_[i] = cvLoadImage( filename.c_str(), -1);

            // We have to copy image data across because OpenCV will automatically
            // destroy the data pointed to by cvImage
            
            // load image from opencv struct to orca object
            cout << "pixelData.size()" << obj->cameraDataVector.at(i)->pixelData.size() << endl;
            memcpy( &obj->cameraDataVector.at(i)->pixelData[0], cvImage_[i]->imageData, obj->cameraDataVector.at(i)->pixelData.size() );

        }
    }
#endif
}

void
MultiCameraReader::loadVideoData( std::stringstream &ss,
                            orca::MultiCameraDataPtr& obj )
{
#ifdef OPENCV_FOUND
    {
        for (unsigned int i = 0; i < descr_->descriptions.size(); i++) {
            // Read in the filename of the avi video
            std::string filename;
            ss >> filename;
            // Check if the video file is in the database of opened files
            map<std::string,CvCapture*>::iterator it;
            it = cvCapture_.find(filename);
            if (it == cvCapture_.end())
            {
                // We need to open the appropriate file
                CvCapture* capture = cvCreateFileCapture( filename.c_str() );
                if (capture)
                {
                    cvCapture_.insert(pair<std::string,CvCapture*>(filename,capture));
                }
                else
                {
                    stringstream ss;
                    ss <<  "Could not open video file supplied by log: " << filename;
                    throw orcalog::FileException( ERROR_INFO, ss.str() );
                }
            }
            // 'it' should now point to the right capture pointer
            it = cvCapture_.find(filename);
            cvImage_[i] = cvQueryFrame((*it).second);

            // We have to copy image data across because OpenCV will automatically
            // destroy the data pointed to by cvImage

            // load image from opencv struct to orca object
            memcpy( &obj->cameraDataVector.at(i)->pixelData[0], cvImage_[i]->imageData, obj->cameraDataVector.at(i)->pixelData.size() );
        }
    }
#endif
}




