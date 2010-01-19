// /*
//  * Orca-Robotics Project: Components for robotics
//  *               http://orca-robotics.sf.net/
//  * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Alen Alempijevic, Michael Warren
//  *
//  * This copy of Orca is licensed to you under the terms described in
//  * the LICENSE file included in this distribution.
//  *
//  */

#include <iostream>
#include <iomanip>
#include <fstream>

#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>
#include <orca/multicamera.h>
#include "multicamerawriter.h"

#include <hydroimage/formats.h>
#include <orcaifacelog/multicamera.h>

// these depend on OpenCV
#ifdef OPENCV_FOUND
    #include <hydroimageocvutil/colourconversions.h>
    using namespace hydroimageocvutil;
#endif

//alen - currently missing conversion utilities
//#include <orcaimage/colourconversions.h>
//#include <orcaimage/imageutils.h>

//alen - Added the below to create a directory to save images
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;
using namespace orcalogfactory;
using namespace hydroimage;

MultiCameraWriter::MultiCameraWriter()
{
#ifdef OPENCV_FOUND
    dataCounter_=1;
#endif
}

MultiCameraWriter::~MultiCameraWriter()
{
 #ifdef OPENCV_FOUND
  	for (unsigned int i = 0; i < cvVideoWriters.size(); ++i) {
            cvReleaseVideoWriter(&cvVideoWriters[i]);
            // Clean up openCV data space
            if (isPadded_[i]) cvReleaseData(&cvImage_[i]);
            cvReleaseImageHeader(&cvImage_[i]);
            cvReleaseImage(&cvBayer_[i]);
        }

 #endif
}

void
MultiCameraWriter::init(const orca::MultiCameraDescriptionPtr &descr, const std::string &format)
{
#ifdef OPENCV_FOUND
    // Resize vectors to accomodate each camera
    cvImage_.resize(descr->descriptions.size(), NULL);
    cvBayer_.resize(descr->descriptions.size(), NULL);
    orcaByteWidth_.resize(descr->descriptions.size(), 0);
    isPadded_.resize(descr->descriptions.size(), false);
#endif
    if (format == "jpeg")
    {
#ifndef OPENCV_FOUND
        throw orcalog::FormatNotSupportedException(ERROR_INFO, "Logger: 'jpeg' format not supported because OpenCV is not installed.");
#else
        initMultiImageLogWriter(descr);
#endif
    }
    else if (format == "bmp")
    {
#ifndef OPENCV_FOUND
        throw orcalog::FormatNotSupportedException(ERROR_INFO, "Logger: 'bmp' format not supported because OpenCV is not installed.");
#else
        initMultiImageLogWriter(descr);
#endif
    }
    else if (format == "avi")
    {
#ifndef OPENCV_FOUND
        throw orcalog::FormatNotSupportedException(ERROR_INFO, "Logger: 'avi' format not supported because OpenCV is not installed.");
#else
        initMultiImageLogWriter(descr);
        initVideoLogWriter(descr);
#endif
    }
}

void
MultiCameraWriter::initMultiImageLogWriter(const orca::MultiCameraDescriptionPtr &descr)
{
#ifdef OPENCV_FOUND
    for( unsigned int i=0; i<descr->descriptions.size(); ++i ) {
	isPadded_[i] = false;
	// Get the properties of each camera
	std::string format = descr->descriptions[i]->format;
	int width = descr->descriptions[i]->width;
        int height = descr->descriptions[i]->height;
        // class to search for image format properties
        ImageFormat imageFormat = ImageFormat::find(format);
        int numChannels = imageFormat.getNumberOfChannels();
        int depth = imageFormat.getBitsPerPixel() / numChannels;
        // check if opencv has padded the byte array so that the width is a multiple of 4 or 8 bytes
        orcaByteWidth_[i] = width*numChannels;
        // Don't allocate image space yet
        cvImage_[i] = cvCreateImageHeader(cvSize(width, height), depth, numChannels);
        if (orcaByteWidth_[i] != cvImage_[i]->widthStep)
        {
            isPadded_[i] = true;
        }
        if (isPadded_[i])
            // Allocate space, we will need to copy image data properly
            cvCreateData(&cvImage_[i]);
        // Allocate memory for bayer image conversion
        if (format == "BayerBG8" ||
            format == "BayerGB8"||
            format == "BayerRG8" ||
            format == "BayerGR8")
                cvBayer_[i] = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
    }
#endif
}


void
MultiCameraWriter::initVideoLogWriter(const orca::MultiCameraDescriptionPtr &descr)
{
#ifdef OPENCV_FOUND
    cvVideoWriters.resize(descr->descriptions.size(), NULL);
    for (unsigned int i = 0; i < descr->descriptions.size(); ++i)
    {
        // Need to initialize video writer
        double fps = descr->descriptions[i]->frameRate;
        int width = descr->descriptions[i]->width;
        int height = descr->descriptions[i]->height;
        std::stringstream filename;
        filename << ".//" << directoryPrefix_ << "//";
        filename << "cam" << i << "_avi_video.avi";
        std::string outputname = filename.str();
        int isColor;
        if (descr->descriptions[i]->format == "GRAY8")
            isColor = 0;
        else
            isColor = 1;
        // Create video writer
        cvVideoWriters[i] = cvCreateVideoWriter(outputname.c_str(),FOURCC_RAWRGB, (int) fps, cvSize(width, height), isColor);
        
    }
#endif
}

void
MultiCameraWriter::createLogDirectory(const std::string & filename)
{
    int directoryPrefixLength = filename.length();
    //Removing the '.log' (total of 4 characters) will give the directory name
    directoryPrefix_= filename.substr(0,directoryPrefixLength-4);
    int ret= mkdir(directoryPrefix_.c_str(),0755);
    if (ret != 0){
        stringstream ss;
        ss << "MultiCamera LogRecorder: Could not create directory " << directoryPrefix_;
        throw orcalog::FileException( ERROR_INFO, ss.str() );
    }
}

void
MultiCameraWriter::logToFile( std::ofstream *file, const std::string &format, orcaice::Context context, const orca::MultiCameraDescriptionPtr &obj)
{
    if ( format=="ice" ){
        orcalog::IceWriteHelper helper( context.communicator() );
        ice_writeMultiCameraDescription( helper.stream_, obj );
        helper.write( file );
    }
    else if ( format=="jpeg" || format == "bmp" ||  format=="avi") {
        //Log the MultiCamera Description to an ascii file
        (*file) << toLogString(obj) << endl;
    }
    else{
        stringstream ss;
        ss << "can't handle format: " << format;
        throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
    }

}

void
MultiCameraWriter::logToFile( std::ofstream *file, const std::string &format, orcaice::Context context, const orca::MultiCameraDataPtr &obj)
{
        if ( format == "ice" ){
            orcalog::IceWriteHelper helper( context.communicator() );
            ice_writeMultiCameraData( helper.stream_, obj );
            helper.write( file );
        }
        else if ( format=="jpeg" ){
             // write object meta data into a ascii file
             (*file) << toLogString(obj);

             // For each image in MultiCameraDatPtr, write it to disk
             for (unsigned int i = 0; i < obj->cameraDataVector.size(); ++i)
             {
                 // generate image filename (different file for each image)
                 std::stringstream filename;
                 filename << ".//" << directoryPrefix_ << "//" << "cam" << i << "_image"
                        << std::setw(5) << std::setfill('0') << dataCounter_ << ".jpeg";
                 std::string outputname;
                 outputname = filename.str();

                 // Write the outputted filename to the logfile as well
                 (*file) << " " << outputname;

                // the images are saved as individual jpegs
                writeCameraDataAsFile(obj, outputname, i);
            }
            // Make sure we put in a newline
            (*file) << endl;

        }
        else if ( format=="bmp" ){
            // write object meta data into a ascii file
            (*file) << toLogString(obj);

            // For each image in MultiCameraDatPtr, write it to disk
            for (unsigned int i = 0; i < obj->cameraDataVector.size(); ++i)
            {
                // generate image filename (different file for each image)
                std::stringstream filename;
                filename << ".//" << directoryPrefix_ << "//" << "cam" << i << "_image"
                        << std::setw(5) << std::setfill('0') << dataCounter_ << ".bmp";
                std::string outputname;
                outputname = filename.str();

                // Write the outputted filename to the logfile as well
                (*file) << " " <<outputname;
                
                // the images are saved as individual bitmaps
                writeCameraDataAsFile(obj, outputname, i);
            }

            // Make sure we put in a newline
            (*file) << endl;
        }
        else if (format =="avi") {
            // write object meta data into a ascii file
            (*file) << toLogString(obj);

            // For each image in MultiCameraDatPtr, write it to disk
            for (unsigned int i = 0; i < obj->cameraDataVector.size(); ++i)
            {
                // generate image filename. This will be the same for each video,
                // but is important for the logplayer to know what the filename is.
                // Also allows for future enhancements to split the video file into
                // manageable pieces
                std::stringstream filename;
                filename << ".//" << directoryPrefix_ << "//" << "cam" << i << "_avi_video.avi";
                std::string outputname = filename.str();

                // Write the outputted filename to the logfile as well
                (*file) << " " << outputname;

                 // the images are saved as uncompressed video into separate files
                 writeCameraDataAsVideo(obj, i);
            }
            // Make sure we put in a newline
            (*file) << endl;
        }
        else{
            stringstream ss;
            ss << "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
        // Increment output image number
        dataCounter_++;
}

// Write image to file after compressing to jpeg format using opencv
void
MultiCameraWriter::writeCameraDataAsFile(const orca::MultiCameraDataPtr& data, const std::string &filename, int i)
{
#ifdef OPENCV_FOUND
        // check if the opencv image data structure is padded
        if (!isPadded_[i])
        {
            // Point the opencv image data structure to the orca image data
            // Don't want to memcpy to reduce the total copies that occur
            cvImage_[i]->imageData = (char*) (&data->cameraDataVector.at(i)->pixelData[0]);
        }
        else
        {
            // if the opencv image data structure is padded, we have to
            // copy the data line by line into a new area of memory
            // Note that this is less efficient than if the data is not padded

            for (uint32_t j = 0; j < static_cast<uint32_t> (data->cameraDataVector.at(i)->description->height); ++j)
            {
                char* destLine = cvImage_[i]->imageData + j * cvImage_[i]->widthStep;
                unsigned char* srcLine = (&data->cameraDataVector.at(i)->pixelData[0]) + j*orcaByteWidth_[i];
                memcpy(destLine, srcLine, orcaByteWidth_[i]);
            }
        }
        // Perform proper Bayer colour conversions
        std::string format = data->cameraDataVector.at(i)->description->format;
        if (format == "BayerBG8" ||
            format == "BayerGB8" ||
            format == "BayerRG8" ||
            format == "BayerGR8")
        {
            cvtToBgr(cvImage_[i], cvBayer_[i], data->cameraDataVector.at(i)->description->format);
            cvSaveImage(filename.c_str(), cvBayer_[i]);
        }
        else
        {
            cvSaveImage(filename.c_str(), cvImage_[i]);
        }
#endif
}


// Write image to video file using opencv
void
MultiCameraWriter::writeCameraDataAsVideo (const orca::MultiCameraDataPtr& data, int i)
{
#ifdef OPENCV_FOUND
      // check if the opencv image data structure is padded
      if (!isPadded_[i])
        {
          // Point the opencv image data structure to the orca image data
          // Don't want to memcpy to reduce the total copies that occur
          cvImage_[i]->imageData = (char*) (&data->cameraDataVector.at (i)->pixelData[0]);
        }
      else
        {
          // if the opencv image data structure is padded, we have to
          // copy the data line by line into a new area of memory
          // Note that this is less efficient than if the data is not padded

            for (uint32_t j = 0; j < static_cast<uint32_t> (data->cameraDataVector.at(i)->description->height); ++j)
            {
                char* destLine = cvImage_[i]->imageData + j * cvImage_[i]->widthStep;
                unsigned char* srcLine = (&data->cameraDataVector.at(i)->pixelData[0]) + j * orcaByteWidth_[i];
                memcpy(destLine, srcLine, orcaByteWidth_[i]);
            }
        }
        // Perform proper Bayer colour conversions
        std::string format = data->cameraDataVector.at(i)->description->format;
        if (format == "BayerBG8" ||
            format == "BayerGB8" ||
            format == "BayerRG8" ||
            format == "BayerGR8")
        {
            cvtToBgr(cvImage_[i], cvBayer_[i], data->cameraDataVector.at(i)->description->format);
            cvWriteFrame(cvVideoWriters[i], cvBayer_[i]);
        }
        else cvWriteFrame(cvVideoWriters[i], cvImage_[i]);
#endif
}
