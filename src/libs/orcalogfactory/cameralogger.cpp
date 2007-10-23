// /*
//  * Orca Project: Components for robotics 
//  *               http://orca-robotics.sf.net/
//  * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
//  *
//  * This copy of Orca is licensed to you under the terms described in the
//  * ORCA_LICENSE file included in this distribution.
//  *
//  */

// #include <iostream>
// #include <iomanip>

// #include <orcaice/orcaice.h>
// #include <orcalog/orcalog.h>
// #include <orcaimage/colourconversions.h>
// #include <orcaimage/imageutils.h>

// #include "cameralogger.h"

// #ifdef OPENCV_FOUND
// 	#include <highgui.h>
// //alen - Added the below to create a directory to save images
// 	#include <sys/stat.h>
// 	#include <sys/types.h>
// #endif

// using namespace std;
// using namespace orcalogfactory;


// CameraLogger::CameraLogger( orcalog::LogMaster *master, 
//                         const std::string & typeSuffix,
//                         const std::string & format,
//                         const std::string & filenamePrefix,
//                         const orcaice::Context & context )
//     : orcalog::Logger( master, 
//              "Camera",
//              "Camera"+typeSuffix,
//              format,
//              filenamePrefix+"camera"+typeSuffix+".log",
//              context ),
// 	directoryPrefix_(filenamePrefix)
// {
// #ifndef OPENCV_FOUND
//     // check that we support this format
//     if ( format_=="ice" )
//     {
//         // this one is supported
//     }
//     else if ( format_=="jpeg" )
//     {
//         context_.tracer()->info( "Images can only be logged in 'jpeg' format if you have OpenCV." );
//         context_.tracer()->info( "Please have a look at the documentation for ImageServer component for installing OpenCv." );
//         throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: 'jpeg' format not supported because OpenCV is not installed." );
//     }
//     else
//     {
//         context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
//         throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
//     }
// #endif


// #ifdef OPENCV_FOUND
//     // Initialise the iplimage pointer to NULL.
//     // Note that we can't do this in the normal variable initialisation stage of the
//     // constructor since it needs OPENCV_FOUND to be defined
//     cvImage_ = 0;

//     // check that we support this format
//     if ( format_!="ice" && format_!="jpeg" )
//     {
//         context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
//         throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
//     }
// #endif

//     // alexm: hack! the idea was for this virtual function to be called from
//     // Logger::Logger() but at that point the derived class is not instantiated yet
//     // so the CameraLogger::createLogFile() is not executed.
//     // benu: createLogFile() is already called for "ice" so only need to call this here
//     // for "jpeg" 
//     if (format_ == "jpeg" )
//     {
// 		//alen: obtain directory name from file prefix
// 		directoryPrefixLength_ = directoryPrefix_.length();
// 		if (directoryPrefixLength_>0)
// 		{
// 			directoryPrefix_=directoryPrefix_.substr(0,--directoryPrefixLength_);
// 		}
//         createLogFile();
//     }
// }

// CameraLogger::~CameraLogger()
// {
// }

// // reimplement from Logger because we have a custom log format
// void
// CameraLogger::createLogFile()
// {
//     //
//     // create log file, may throw and it will kill us
//     //
//     if ( format_ == "ice" ) {
//         createBinaryFile();
//     }
//     else if ( format_ == "jpeg" ) {
//         createBinaryFile();
//     }
// }

// void 
// CameraLogger::init()
// {   
//     // throws orcaice::NetworkException but it's ok to quit here
//     orca::CameraPrx objectPrx;
//     orcaice::connectToInterfaceWithTag<orca::CameraPrx>( context_, objectPrx, interfaceTag_ );
    
//     // get description. should do try/catch
//     orca::CameraDescription obj = objectPrx->getDescription();
//     writeDescription( obj );

//     // consumer
//     Ice::ObjectPtr consumer = this;
//     orca::CameraConsumerPrx callbackPrx = orcaice::createConsumerInterface<orca::CameraConsumerPrx>( context_, consumer );

//     context_.tracer()->debug("Subscribing to IceStorm now.",5);
//     objectPrx->subscribe( callbackPrx );
// }

// void 
// CameraLogger::setData(const orca::CameraData& data, const Ice::Current&)
// {
//     // Write reference to master file
//     appendMasterFile();

//     if ( format_ == "ice" )
//     {
//         orcalog::IceWriteHelper helper( context_.communicator() );
//         ice_writeCameraData( helper.stream_, data );
//         helper.write( file_ );
//     }
//     else if ( format_ == "jpeg" )
//     {        
//         // image filename (different file for each image)
//         std::ostringstream filename;
// 		if (directoryPrefixLength_>0)
// 		{
//         	filename << ".//" << directoryPrefix_ << "//";
// 		}
// 		filename << "image" << std::setw(5) << std::setfill('0') << dataCounter_ << ".jpg";

//         // write object meta data into a binary file
//         orcalog::IceWriteHelper helper( context_.communicator() );
//         this->orca_writeCameraData( helper.stream_, data, filename.str() );
//         helper.write( file_ );

//         // the image itself is saved as jpeg into a separate file
// // ##########################################
// //
// // alexm: 'data' cannot be const because we are changing it's format
// //
//          writeCameraDataAsJpeg( data, filename.str() );
// // ##########################################
//     }
//     else
//     {
//         context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
//         throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
//     }
// }

// void 
// CameraLogger::writeDescription( const orca::CameraDescription& obj )
// {
//     context_.tracer()->debug( "CameraLogger: Writing description to file", 5 );
    
//     if ( format_ == "ice" || format_ == "jpeg" )
//     {
//         orcalog::IceWriteHelper helper( context_.communicator() );
//         ice_writeCameraDescription( helper.stream_, obj );
//         helper.write( file_ );  
//     }
//     else
//     {
//         context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
//         throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
//     }
// } 


// // Write image object information and image filename to our personal file 
// // (this is the file that the master file looks at)
// // Note: need to do our own marshalling as we don't want the whole object in the personal file
// void 
// CameraLogger::orca_writeCameraData( Ice::OutputStreamPtr outStreamPtr, 
//         const orca::CameraData& data, const std::string & filename )
// {
//     outStreamPtr->writeInt(data.imageWidth);
//     outStreamPtr->writeInt(data.imageHeight);
//     if ( format_ == "jpeg" )
//     {
//         // if we use opencv to compress to jpeg, the format must be BGR
//         outStreamPtr->writeInt(orca::ImageFormatModeBgr);
//     }
//     else
//     {
//         outStreamPtr->writeInt(data.format);
//     }
//     outStreamPtr->writeInt(data.compression);
//     outStreamPtr->writeDouble(orcaice::timeAsDouble(data.timeStamp));
//     outStreamPtr->writeString( filename );
// }    

// // Write image to file after compressing to jpeg format using opencv
// void 
// CameraLogger::writeCameraDataAsJpeg( const orca::CameraData& data, const std::string & filename )
// {
// #ifdef OPENCV_FOUND
//     if ( dataCounter_ == 1 )
//     {    
//         nChannels_ = orcaimage::numChannels( data.format );
        
//         // setup opencv image
//         cvImage_  = cvCreateImage( cvSize( data.imageWidth, data.imageHeight ),  8, nChannels_ );

// 		//alen: create directory to store images
// 		if (directoryPrefixLength_>0)
// 		{
// 			int ret= mkdir(directoryPrefix_.c_str(),0755);
// 			if (ret != 0)
// 			{	
// 					std::string s = "Camera Logger: Could not create directory " + directoryPrefix_;
// 					context_.tracer()->warning( s );
// 					throw orcalog::FileException( ERROR_INFO, s );
// 			}
// 		}
//     }
    
//     // cout << "Image Format: " << orcaimage::formatName( data.format ) << endl;
    
//     // copy image in orca object into opencv struct
//     // and make sure the image is BGR format before compression
//     // cout << "nChannels_: " << nChannels_ << endl;       
//     if ( nChannels_ == 3 )
//     {       
//         orcaimage::cvtToBgr( cvImage_, cvImage_, data );
//         // data.format = orca::ImageFormatModeBgr;
//     }
//     else
//     {
//         memcpy( cvImage_->imageData, &data.image[0], data.image.size() );
//     }                     
    
//     // Save image to file
//     // This assumes that the file has been stored in bgr format (if the image is colour) which is 
//     // the default for opencv
//     cvSaveImage( filename.c_str(), cvImage_ );
// #endif

// }     
