/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alen Alempijevic, Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <cmath>
#include <cstring>
#include <gbxutilacfr/exceptions.h>

#include <IceUtil/IceUtil.h>

#include <hydroutil/hydroutil.h>

#include "driver.h"

using namespace std;

using namespace imagedc1394; 

Driver::Driver( const Config &cfg, const hydroutil::Context &context ) :
    config_(cfg),
    context_(context)
{

    //
    //Check the desired format, RGB8 and BGR8 are the same (with channels swapped)
    //GRAY8 is actually MONO8 in DC1394 standards
    //RAW8 is for bayer cameras and will invoke Format7 settings
    //

    dc1394color_coding_t colorCoding;

    if( config_.format == "RGB8"){
        colorCoding=DC1394_COLOR_CODING_RGB8;
    }
    else if (config_.format == "GRAY8" ){
        colorCoding=DC1394_COLOR_CODING_MONO8;
    }
    else if (config_.format == "RAW8"){
        colorCoding=DC1394_COLOR_CODING_RAW8;
    }
    else{
        std::stringstream sse;
        sse << "The driver can only provide RGB8, GRAY8 or RAW8 data at the moment,";
        sse << " you have requisted =" << config_.format;
        throw gbxutilacfr::Exception(ERROR_INFO, sse.str());
    }
    //
    //Check ISO Mode, The selection of ISO Mode also effects the ISO Speed
    //
    hydroutil::Properties &prop = context_.properties();
    std::string prefix = "DC1394.";
    string mode = prop.getPropertyWithDefault( prefix+"IsoMode", "1394A" );
    if ( mode == "1394B" ){
        isoMode_ = DC1394_OPERATION_MODE_1394B;
        isoSpeed_ = DC1394_ISO_SPEED_800;
    }
    else if (mode == "1394A"){ 
        isoMode_ = DC1394_OPERATION_MODE_LEGACY;
        isoSpeed_ = DC1394_ISO_SPEED_400;
    }
    else
        throw gbxutilacfr::Exception( ERROR_INFO, "Unknown image driver Iso mode: '"+mode+"'" );

	//
	//Read GUID of camera, used to distinguish cameras on the bus
	//Otherwise first camera on bus selected
	//
	string sGuid = prop.getPropertyWithDefault( prefix+"Guid", "Any" );

    //
    //Copy desired width and height from the config settings
    //
    width_ = static_cast<uint>(config_.width);
    height_ = static_cast<uint>(config_.height);

    dc1394_t * d;
    dc1394camera_list_t * list;
    d = dc1394_new ();

    if ( dc1394_camera_enumerate (d, &list) != DC1394_SUCCESS ){
        stringstream ss;
        ss << "Unable to look for cameras\n\n"
            "Please check \n"
            "  - if the kernel modules `ieee1394',`raw1394' and `ohci1394' are loaded \n"
            "  - if you have read/write access to /dev/raw1394\n\n";
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }

    //  get the camera nodes and describe them as we find them
    if (list->num == 0) {
        stringstream ss;
        ss << "No cameras found";
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }

	int connectToCam =0;

	if (sGuid == "Any")
	{
        stringstream ss;
        ss << "No guid specified, will connect to first camera on bus";
		context_.tracer().info( ss.str() );
	}
	else
	{
		//Search for camera with GUID specified
		bool found = false;
		uint64_t test = 0x0LL;
		sscanf (sGuid.c_str(), "0x%Lx", &test);
		for (uint i=0;i<list->num;i++){
			stringstream ss;
			ss << "Camera " << i << " with guid 0x" << hex << list->ids[i].guid ;
			context_.tracer().info( ss.str() );
			if (list->ids[i].guid == test  )
			{
				found = true;
			}
		}
		if (!found)
		{
			stringstream ss;
			ss << "Failed to initialize camera with guid" << sGuid ;
			throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
		}
	}

    camera = dc1394_camera_new (d, list->ids[connectToCam].guid);
    if (!camera) {
        stringstream ss;
        ss << "Failed to initialize camera with guid" << hex << list->ids[connectToCam].guid ;
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
    else{
        stringstream ss;
        ss << "There are " << list->num << " camera(s) found attached to your PC";
        context_.tracer().info( ss.str() );
        stringstream ss1;
        ss1 << "Working with the camera " << camera->vendor << " " << camera->model;
        context_.tracer().info( ss1.str() );
    }


    // free the other cameras
    dc1394_camera_free_list (list);

    dc1394video_modes_t 	video_modes;
    if ( dc1394_video_get_supported_modes( camera, &video_modes ) != DC1394_SUCCESS ) {
        cleanup( camera );
        stringstream ss;
        ss << "Can't get video modes";
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
    else {
        stringstream ss;
        ss << "Searching for " << config_.format << " format";
        context_.tracer().info( ss.str());
    }

    bool videoModeOk = false;
    //format7 not set
    format7_ = false;


    if (config_.format == "RAW8"){
        //Searching for a Format 7 Mode that supports RAW8
        stringstream ss;
        ss << "Searching for Format_7 Mode 0";
        context_.tracer().info( ss.str());
        for ( int i = video_modes.num-1; i >= 0; i-- ) 
        {
            // FORMAT 7 modes (i.e. "scalable")
            if ( dc1394_is_video_mode_scalable( video_modes.modes[i] ) ){
                stringstream ss;
                ss <<  "Format_7 Mode " << (video_modes.modes[i]-DC1394_VIDEO_MODE_FORMAT7_MIN) << " detected";
                context_.tracer().info( ss.str() ); 
                if ( video_modes.modes[i] == DC1394_VIDEO_MODE_FORMAT7_0 ){
                    videoMode_ = video_modes.modes[i];
                    videoModeOk = true;
                    format7_ = true;
                    break;
                }
            }
        }
    }
    else{
        //
        //If not Format_7 then the height and width are selected by selecting a certain mode 
        //that supports the height and width parameters
        // 
        dc1394color_coding_t coding;
        for ( int i = video_modes.num-1; i >= 0; i-- ){
            // don't consider FORMAT 7 modes (i.e. "scalable"), just native camera modes
            if ( !dc1394_is_video_mode_scalable( video_modes.modes[i] ) ) {
                dc1394_get_color_coding_from_video_mode( camera, video_modes.modes[i], &coding );
                if ( coding == colorCoding ) {
                    videoMode_ = video_modes.modes[i];
                    unsigned int width, height;
                    dc1394_get_image_size_from_video_mode( camera,videoMode_,&width,&height );

                    stringstream ss;
                    ss <<  "Supported width:" << width << " height:" << height ;
                    context_.tracer().info( ss.str() );

                    if ( (width == width_) && (height == height_) ){
                        videoModeOk = true;
                        break;
                    }
                }
            }
        }
    }

    if (!format7_)
    {
        //Need to check width / height here against config
        unsigned int width, height;
        dc1394_get_image_size_from_video_mode( camera,videoMode_,&width,&height );

        if( width_ != width ){
            stringstream ss;
            ss << "config does not match image properties: actual width=" << width << " config width=" << width_;
            throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
        }

        if( height_ != height ){
            stringstream ss;
            ss << "config does not match image properties: actual height=" << height << " config height=" << height_;
            throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
        }

        // double check that we found a video mode that is desired color coding
        dc1394color_coding_t coding;
        dc1394_get_color_coding_from_video_mode( camera, videoMode_, &coding );
        if ( ( coding != colorCoding ) ){
            cleanup( camera );
            stringstream ss;
            ss << "Could not get a valid desired color mode: " << config_.format ;
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }

        dc1394_video_set_operation_mode( camera, isoMode_ );
        dc1394_video_set_iso_speed( camera, isoSpeed_ );

        // get highest framerate
        dc1394framerates_t 	framerates;
        dc1394framerate_t 	framerate;
        if ( dc1394_video_get_supported_framerates( camera, videoMode_, &framerates ) != DC1394_SUCCESS ){
            cleanup( camera );
            throw gbxutilacfr::Exception( ERROR_INFO, "Can't get a framerate" );
        }
        framerate = framerates.framerates[ framerates.num-1 ];
        //The framerate is avaibale here and can be passed into the config
        //config_.framerate=framerate;

        // setup capture
        context_.tracer().info( "Setting capture" );  

        dc1394_video_set_mode( camera, videoMode_ );
        dc1394_video_set_framerate( camera, framerate );
        if ( dc1394_capture_setup( camera, 8, DC1394_CAPTURE_FLAGS_AUTO_ISO ) != DC1394_SUCCESS ){
            cleanup( camera );
            stringstream ss;
            ss << "unable to setup camera\n"
                "check to make sure that the video mode and framerate are supported by your camera\n"
                "video_mode =" << videoMode_ << "framerate =" << framerate << "\n"
                "Check dc1394_control.h for the meanings of these values";
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }
    }
    else{
        //
        //Formta7 setup
        //

        // camera status, speed and operationg mode
        int status;
        dc1394speed_t speed;

        // get ISO speed 
        if (dc1394_video_get_iso_speed(camera,&speed) != DC1394_SUCCESS){
            cleanup(camera);
            throw gbxutilacfr::Exception( ERROR_INFO, "unable to get ISO speed" );
        }
        else{
            stringstream ss;
            if (speed == DC1394_ISO_SPEED_800)
                ss <<  "Current ISO speed 800Mbps";
            else if (speed == DC1394_ISO_SPEED_400)
                ss <<  "Current ISO speed 400Mbps";
            context_.tracer().info( ss.str() );
        } 

        if (dc1394_video_set_operation_mode( camera, isoMode_ ) != DC1394_SUCCESS){
            cleanup(camera);
            stringstream ss;
            ss << "unable to set operation mode to ";
            if (isoMode_ == DC1394_OPERATION_MODE_1394B)
                ss <<  "1394B";
            else 
                ss <<  "Legacy 1394A";
            context_.tracer().info( ss.str() );
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }

        //It appears you can set any ISO speed to the camera and it will never complain here
        //However, when setting the ROI later it will complain, so the ISO speed needs to match 
        //that of the camera manual
        if (dc1394_video_set_iso_speed(camera,isoSpeed_) != DC1394_SUCCESS){
            cleanup(camera);
            stringstream ss;
            ss << "Unable to set ISO speed to ";
            if (isoSpeed_ == DC1394_ISO_SPEED_800)
                ss <<  "800Mbps";
            else 
                ss <<  "400Mbps";
            context_.tracer().info( ss.str() );
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }

        dc1394video_mode_t mmode;
        if (dc1394_video_get_mode(camera, &mmode) != DC1394_SUCCESS){
            cleanup(camera);
            throw gbxutilacfr::Exception( ERROR_INFO, "unable to get current video mode" );
        }
        else{
            stringstream ss;
            ss <<  "Current mode:" << mmode ;
            context_.tracer().info( ss.str() );
        }
        // setup capture
        if (dc1394_video_set_mode(camera, videoMode_) != DC1394_SUCCESS){
            cleanup(camera);
            throw gbxutilacfr::Exception( ERROR_INFO, "unable to set video mode" );
        }

        if (dc1394_video_get_mode(camera, &mmode) != DC1394_SUCCESS){
            cleanup(camera);
            throw gbxutilacfr::Exception( ERROR_INFO, "unable to retrieve set mode" );
        }

        unsigned int width,height;

        if (dc1394_format7_get_max_image_size(camera, videoMode_, &width, &height) != DC1394_SUCCESS){
            cleanup(camera);
            throw gbxutilacfr::Exception( ERROR_INFO, "unable to query max image size" );
        }
        else{
            stringstream ss;
            ss <<  "Supported MAX width:" << width << " height:" << height ;
            context_.tracer().info( ss.str() );
        }

        if (dc1394_format7_get_image_size(camera, videoMode_, &width, &height) != DC1394_SUCCESS){
            cleanup(camera);
            throw gbxutilacfr::Exception( ERROR_INFO, "unable to query image size" );
        }
        else{
            stringstream ss;
            ss <<  "Current image width:" << width << " height:" << height ;
            context_.tracer().info( ss.str() );
        }

        unsigned int min_bytes, max_bytes;
        if (dc1394_format7_get_packet_parameters(camera,videoMode_,&min_bytes,&max_bytes) != DC1394_SUCCESS){
            cleanup(camera);
            throw gbxutilacfr::Exception( ERROR_INFO, "unable to query packet size" );
        }
        else{
            stringstream ss;
            ss <<  "Supported packet size MIN:" << min_bytes << " MAX:" << max_bytes ;
            context_.tracer().info( ss.str() );
        }

        status=dc1394_format7_set_roi(camera, videoMode_,DC1394_COLOR_CODING_RAW8, max_bytes-1, 0,0, width_, height_);//9500
        if (status != DC1394_SUCCESS){
            cleanup(camera);
            throw gbxutilacfr::Exception( ERROR_INFO, "unable to set ROI" );
        }

        //    status=dc1394_format7_set_roi(cam, mode,DC1394_COLOR_CODING_MONO8, 2500, 0,0, 1024, 4608);
        //    status=dc1394_format7_set_roi(cam, mode,DC1394_COLOR_CODING_MONO8, 2000, 0,0, 512, 9216);
        //    status=dc1394_format7_set_roi(cam, mode,DC1394_COLOR_CODING_MONO8, 2000, 0,0, 512, 2015);
        //DC1394_USE_MAX_AVAIL

        unsigned int bytes_per_packet;
        unsigned int left, top;
        dc1394color_coding_t color_coding;

        status = dc1394_format7_get_roi(camera, videoMode_,&color_coding, &bytes_per_packet, &left, &top, &width, &height );
        if (status != DC1394_SUCCESS){
            cleanup(camera);
            throw gbxutilacfr::Exception( ERROR_INFO, "error returning ROI" );
        }
        else{
            stringstream ss;
            ss <<  "Current setup bytes per packet:" <<  bytes_per_packet << " ROI (left,top,width,height): (";
            ss <<   left << "," << top << "," <<  width << "," <<  height <<")";
            context_.tracer().info( ss.str() );
        }

        // check the image size in bytes
        uint64_t totalbytes;
        status = dc1394_format7_get_total_bytes(camera, videoMode_, &totalbytes);
        if (status != DC1394_SUCCESS){
            cleanup(camera);
            throw gbxutilacfr::Exception( ERROR_INFO, "error checking image size" );
        }
        else{
            stringstream ss;
            ss <<  "Bytes per frame:" <<  totalbytes;
            context_.tracer().info( ss.str() );
        }

        // setup capture
        status=dc1394_capture_setup(camera, 4, DC1394_CAPTURE_FLAGS_DEFAULT);
        if (status != DC1394_SUCCESS){
            cleanup( camera );
            stringstream ss;
            ss << "unable to setup camera\n"
                "check to make sure that the video mode and ROI are supported by your camera";
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }

    }
    // have the camera start sending us data
    context_.tracer().info("Start transmission");
    if ( dc1394_video_set_transmission( camera, DC1394_ON ) != DC1394_SUCCESS ){
        cleanup( camera );
        throw gbxutilacfr::Exception( ERROR_INFO, "Unable to start camera iso transmission" );
    }

    context_.tracer().info("Waiting for transmission... ");

    //  Sleep untill the camera has a transmission
    dc1394switch_t status = DC1394_OFF;

    for ( int i = 0; i <= 5; i++ )
    {
        usleep(50000);
        if ( dc1394_video_get_transmission( camera, &status ) != DC1394_SUCCESS ){
            cleanup( camera );

        }
        if ( status != DC1394_OFF )
            break;
        if( i == 5 ){
            cleanup( camera );
            throw gbxutilacfr::Exception( ERROR_INFO, "Camera doesn't seem to want to turn on!" );
        }
    }

}

Driver::~Driver()
{
    cleanup(camera);
}

    void
Driver::read( hydrointerfaces::Image::Data &data )
{
    context_.tracer().info( "Grabbing frame(s) from camera(s)..." );

    err_=dc1394_capture_dequeue(camera, DC1394_CAPTURE_POLICY_WAIT, &frame_);

    if (err_ < 0 ){
        context_.tracer().debug( "No frame in buffer ....");
    }
    else
    {
        if (dc1394_capture_is_frame_corrupt (camera, frame_)){
            context_.tracer().debug( "Buffer corrupted ");
        }
        else{
            //Notes how many frames are still left in buffer
            //std::cout <<  "Frames behind " << frame_->frames_behind << std::endl;

            memcpy(data.pixelData,frame_->image, frame_->image_bytes);

            //Get the time from the frame timestamp
            data.timeStampSec = (long)(frame_->timestamp/1000000);
            data.timeStampUsec = (long)(frame_->timestamp - ((double)data.timeStampSec*1000000));

        }
        dc1394_capture_enqueue(camera, frame_);
    }
}

    void 
Driver::cleanup( dc1394camera_t* camera ) 
{
    dc1394_capture_stop( camera );
    dc1394_video_set_transmission( camera, DC1394_OFF );
    dc1394_camera_free( camera );
}



extern "C" {
    hydrointerfaces::ImageFactory *createImageDriverFactory()
    { return new imagedc1394::Factory; }
}
