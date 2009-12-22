/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alen Alempijevic, Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp, Michael Warren
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <cmath>
#include <cstring>
#include <cfloat>
#include <IceUtil/IceUtil.h>
#include <gbxutilacfr/exceptions.h>

#include <hydroutil/hydroutil.h>

#include "driver.h"

using namespace std;

namespace multiimagedc1394camera
{

Driver::Driver(const Config &cfg, const hydroutil::Context &context) :
config_(cfg),
context_(context)
{
    camera.resize(config_.size());
    frame_.resize(config_.size());
    videoMode_.resize(config_.size());
    height_.resize(config_.size());
    width_.resize(config_.size());
    format7_.resize(config_.size());
    isoMode_.resize(config_.size());
    isoSpeed_.resize(config_.size());
    numBufs.resize(config_.size());
    for (unsigned int cam_index = 0; cam_index < camera.size(); ++cam_index)
    {

        //
        //Check the desired format, RGB8 and BGR8 are the same (with channels swapped)
        //GRAY8 is actually MONO8 in DC1394 standards
        //RAW8 is for bayer cameras and will invoke Format7 settings
        //

        dc1394color_coding_t colorCoding;

        if (config_.at(cam_index).format == "RGB8")
        {
            colorCoding = DC1394_COLOR_CODING_RGB8;
        }
        else if (config_.at(cam_index).format == "GRAY8")
        {
            colorCoding = DC1394_COLOR_CODING_MONO8;
        }
        else if (config_.at(cam_index).format == "RAW8")
        {
            colorCoding = DC1394_COLOR_CODING_RAW8;
        }
        else
        {
            std::stringstream sse;
            sse << "The driver can only provide RGB8, GRAY8 or RAW8 data at the moment,";
            sse << " you have requested =" << config_.at(cam_index).format;
            throw gbxutilacfr::Exception(ERROR_INFO, sse.str());
        }
        //
        //Check ISO Mode, The selection of ISO Mode also effects the ISO Speed
        //
        hydroutil::Properties &prop = context_.properties();
        std::string prefix = "DC1394.";
        // "DC1394.IsoMode=..."
        string mode = prop.getPropertyWithDefault(prefix + "IsoMode", "1394A");
        if (mode == "1394B")
        {
            isoMode_.at(cam_index) = DC1394_OPERATION_MODE_1394B;
            isoSpeed_.at(cam_index) = DC1394_ISO_SPEED_800;
        }
        else if (mode == "1394A")
        {
            isoMode_.at(cam_index) = DC1394_OPERATION_MODE_LEGACY;
            isoSpeed_.at(cam_index) = DC1394_ISO_SPEED_400;
        }
        else
            throw gbxutilacfr::Exception(ERROR_INFO, "Unknown image driver Iso mode: '" + mode + "'");

        //
        //Read GUID of camera, used to distinguish cameras on the bus
        //Otherwise first available camera on bus selected
        //
        std::stringstream dc1394_prefix_ss;
        dc1394_prefix_ss << prefix << cam_index << ".";
        std::string dc1394_prefix = dc1394_prefix_ss.str();
        // "DC1394.N.Guid=..."
        string sGuid = prop.getPropertyWithDefault(dc1394_prefix + "Guid", "Any");

        //
        //Copy desired width and height from the config settings
        //
        width_.at(cam_index) = static_cast<uint> (config_.at(cam_index).width);
        height_.at(cam_index) = static_cast<uint> (config_.at(cam_index).height);

        dc1394_t * d;
        dc1394camera_list_t * list;
        d = dc1394_new();

        if (dc1394_camera_enumerate(d, &list) != DC1394_SUCCESS)
        {
            stringstream ss;
            ss << "Unable to look for cameras\n\n"
                    "Please check \n"
                    "  - if the kernel modules `ieee1394',`raw1394' and `ohci1394' are loaded \n"
                    "  - if you have read/write access to /dev/raw1394\n\n";
            throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
        }

        //  get the camera nodes and describe them as we find them
        if (list->num == 0)
        {
            stringstream ss;
            ss << "No cameras found";
            throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
        }
        if (list->num < config_.size())
        {
            stringstream ss;
            ss << "Not enough cameras on bus. " << config_.size() <<
                    " cameras requested, " << list->num << " cameras found, ";
            throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
        }
        int connectToCam = cam_index;

        if (sGuid == "Any")
        {
            stringstream ss;
            ss << "No guid specified, will connect to camera " 
                    << connectToCam << " on bus";
            context_.tracer().info(ss.str());
        }
        else
        {
            //Search for camera with GUID specified
            bool found = false;
            uint64_t test = 0x0LL;
            sscanf(sGuid.c_str(), "0x%Lx", &test);
            for (uint i = 0; i < list->num; i++)
            {
                stringstream ss;
                ss << "Camera " << i << " with guid 0x" << hex << list->ids[i].guid;
                context_.tracer().info(ss.str());
                if (list->ids[i].guid == test)
                {
                    found = true;
                }
            }
            if (!found)
            {
                stringstream ss;
                ss << "Failed to initialize camera with guid: " << sGuid;
                throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
            }
        }

        camera.at(cam_index) = dc1394_camera_new(d, list->ids[connectToCam].guid);
        if (!camera.at(cam_index))
        {
            stringstream ss;
            ss << "Failed to initialize camera with guid: " << hex << list->ids[connectToCam].guid;
            throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
        }
        else
        {
            stringstream ss;
            ss << "There are " << list->num << " camera(s) found attached to your PC";
            context_.tracer().info(ss.str());
            stringstream ss1;
            ss1 << "Working with the camera " << camera.at(cam_index)->vendor << " " << camera.at(cam_index)->model;
            context_.tracer().info(ss1.str());
        }

        // free the other cameras
        dc1394_camera_free_list(list);

        dc1394video_modes_t video_modes;
        if (dc1394_video_get_supported_modes(camera.at(cam_index), &video_modes) != DC1394_SUCCESS)
        {
            cleanup(camera.at(cam_index));
            stringstream ss;
            ss << "Can't get video modes for cam " << cam_index;
            throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
        }
        else
        {
            stringstream ss;
            ss << "Searching for " << config_.at(cam_index).format << " format";
            context_.tracer().info(ss.str());
        }

        bool videoModeOk = false;
        //format7 not set
        format7_.at(cam_index) = false;

        if (config_.at(cam_index).format == "RAW8")
        {
            //Searching for a Format 7 Mode that supports RAW8
            stringstream ss;
            ss << "Searching for Format_7 Mode 0";
            context_.tracer().info(ss.str());
            for (int i = video_modes.num - 1; i >= 0; i--)
            {
                // FORMAT 7 modes (i.e. "scalable")
                if (dc1394_is_video_mode_scalable(video_modes.modes[i]))
                {
                    stringstream ss;
                    ss << "Format_7 Mode " << (video_modes.modes[i] - DC1394_VIDEO_MODE_FORMAT7_MIN) << " detected";
                    context_.tracer().info(ss.str());
                    if (video_modes.modes[i] == DC1394_VIDEO_MODE_FORMAT7_0)
                    {
                        videoMode_.at(cam_index) = video_modes.modes[i];
                        videoModeOk = true;
                        format7_.at(cam_index) = true;
                        break;
                    }
                }
            }
        }
        else
        {
            //
            //If not Format_7 then the height and width are selected by selecting a certain mode
            //that supports the height and width parameters
            //
            dc1394color_coding_t coding;
            for (int i = video_modes.num - 1; i >= 0; i--)
            {
                // don't consider FORMAT 7 modes (i.e. "scalable"), just native camera modes
                if (!dc1394_is_video_mode_scalable(video_modes.modes[i]))
                {
                    dc1394_get_color_coding_from_video_mode(camera.at(cam_index), video_modes.modes[i], &coding);
                    if (coding == colorCoding)
                    {
                        videoMode_.at(cam_index) = video_modes.modes[i];
                        unsigned int width, height;
                        dc1394_get_image_size_from_video_mode(camera.at(cam_index), videoMode_.at(cam_index), &width, &height);

                        stringstream ss;
                        ss << "Supported width:" << width << " height:" << height;
                        context_.tracer().info(ss.str());

                        if ((width == width_.at(cam_index)) && (height == height_.at(cam_index)))
                        {
                            videoModeOk = true;
                            break;
                        }
                    }
                }
            }
        }

        if (!format7_.at(cam_index))
        {
            //Need to check width / height here against config
            unsigned int width, height;
            dc1394_get_image_size_from_video_mode(camera.at(cam_index), videoMode_.at(cam_index), &width, &height);

            if (width_.at(cam_index) != width)
            {
                stringstream ss;
                ss << "config does not match image properties: actual width=" << width << " config width=" << width_.at(cam_index);
                throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
            }

            if (height_.at(cam_index) != height)
            {
                stringstream ss;
                ss << "config does not match image properties: actual height=" << height << " config height=" << height_.at(cam_index);
                throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
            }

            // double check that we found a video mode that is desired color coding
            dc1394color_coding_t coding;
            dc1394_get_color_coding_from_video_mode(camera.at(cam_index), videoMode_.at(cam_index), &coding);
            if ((coding != colorCoding))
            {
                cleanup(camera.at(cam_index));
                stringstream ss;
                ss << "Could not get a valid desired color mode: " << config_.at(cam_index).format;
                throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
            }

            dc1394_video_set_operation_mode(camera.at(cam_index), isoMode_.at(cam_index));
            dc1394_video_set_iso_speed(camera.at(cam_index), isoSpeed_.at(cam_index));

            // get available framerates
            double minDiff = DBL_MAX;
            dc1394framerates_t framerates;
            dc1394framerate_t framerate;
            dc1394framerate_t bestFps = (dc1394framerate_t) - 1;
            
            std::stringstream multicam_prefix_ss;
            multicam_prefix_ss << cam_index << ".";
            std::string multicam_prefix = multicam_prefix_ss.str();
            // "N.FrameRate=..."
            string sfps = prop.getPropertyWithDefault(multicam_prefix + "FrameRate", "30");
            if (sfps == "1.875")
            {
                framerate = DC1394_FRAMERATE_1_875;
            }
            else if (sfps == "3.75")
            {
                framerate = DC1394_FRAMERATE_3_75;
            }
            else if (sfps == "7.5")
            {
                framerate = DC1394_FRAMERATE_7_5;
            }
            else if (sfps == "15")
            {
                framerate = DC1394_FRAMERATE_15;
            }
            else if (sfps == "30")
            {
                framerate = DC1394_FRAMERATE_30;
            }
            else if (sfps == "60")
            {
                framerate = DC1394_FRAMERATE_60;
            }
            else
            {
                std::stringstream sse;
                sse << "The driver can only provide framerates of 1.875, 3.75, ";
                sse << "7.5, 15, 30 or 60. You have requested " << sfps;
                throw gbxutilacfr::Exception(ERROR_INFO, sse.str());
            }
            if (dc1394_video_get_supported_framerates(camera.at(cam_index), videoMode_.at(cam_index), &framerates) != DC1394_SUCCESS)
            {
                cleanup(camera.at(cam_index));
                throw gbxutilacfr::Exception(ERROR_INFO, "Can't get a framerate");
            }

            // Find the closest available framerate to the one requested and set it
            for (int i = 0; i < (int)framerates.num; i++)
            {
                dc1394framerate_t ifps = framerates.framerates[i];
                double diff = fabs(ifps - framerate);
                if (diff < minDiff)
                {
                    minDiff = diff;
                    bestFps = ifps;
                }
            }

            //Check for non-auto shutterval
            // "DC1394.N.ShutterSpeed=..."
            string shutterMode = prop.getPropertyWithDefault(dc1394_prefix + "ShutterSpeed", "Auto");
            if (shutterMode != "Auto") {  // If user has defined a shutter mode
                int shutterVal;
                istringstream shutterStream(shutterMode);
                if (!(shutterStream >> shutterVal)) {
                    stringstream ss;
                    ss << "Unable to set shutter value of \"" <<
                        shutterVal << "\" on camera " << cam_index;
                    throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
                }
                else {
                    stringstream ss;
                    ss << "Setting shutter to \"" << shutterVal << "\" on camera " << cam_index;;
                    context_.tracer().info(ss.str());
                    if (dc1394_feature_set_mode(camera.at(cam_index),DC1394_FEATURE_SHUTTER,DC1394_FEATURE_MODE_MANUAL)!= DC1394_SUCCESS) {
                        stringstream ss;
                        ss << "Could not set SHUTTER to MANUAL on camera "<< cam_index;
                        throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
                    }
                    if (dc1394_feature_set_value(camera.at(cam_index),DC1394_FEATURE_SHUTTER,shutterVal) != DC1394_SUCCESS) {
                        stringstream ss;
                        ss << "Could not set SHUTTER to " << shutterVal << "on camera " << cam_index;
                       throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
                    }

                }
            }
            else { // Ensure that shutter is set to Auto
                stringstream ss;
                ss << "Setting shutter to Auto on camera " << cam_index;
                context_.tracer().info(ss.str());
                if (dc1394_feature_set_mode(camera.at(cam_index),DC1394_FEATURE_SHUTTER,DC1394_FEATURE_MODE_AUTO)!= DC1394_SUCCESS) {
                    stringstream ss;
                    ss << "Could not set SHUTTER to AUTO on camera "<< cam_index;
                    throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
                }
            }

            // Set the number of dc1394 buffers
            // "DC1394.NumBufs=..."
            string numBufsString = prop.getPropertyWithDefault(prefix + "NumBufs", "16");
            numBufs.at(cam_index) = 16;
            istringstream numBufsStream(numBufsString);
            if (!(numBufsStream>>numBufs.at(cam_index))) {
                    stringstream ss;
                    ss << "Unable to set size of dc1394 buffer to \"" <<
                        numBufsStream.str() << ". Defaulting to 16.";
                    throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
            }

            // setup capture
            context_.tracer().info("Setting capture");

            dc1394_video_set_mode(camera.at(cam_index), videoMode_.at(cam_index));
            if ((int)bestFps >= 0)
                dc1394_video_set_framerate(camera.at(cam_index), bestFps);
            if (dc1394_capture_setup(camera.at(cam_index), numBufs.at(cam_index) , DC1394_CAPTURE_FLAGS_DEFAULT) != DC1394_SUCCESS)
            {
                cleanup(camera.at(cam_index));
                stringstream ss;
                ss << "unable to setup camera\n"
                        "check to make sure that the video mode and framerate are supported by your camera\n"
                        "video_mode =" << videoMode_.at(cam_index) << "framerate =" << framerate << "\n"
                        "Check dc1394_control.h for the meanings of these values";
                throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
            }
        }
        else
        {
            //
            //Format7 setup
            //

            // camera status, speed and operationg mode
            int status;
            dc1394speed_t speed;

            // get ISO speed
            if (dc1394_video_get_iso_speed(camera.at(cam_index), &speed) != DC1394_SUCCESS)
            {
                cleanup(camera.at(cam_index));
                throw gbxutilacfr::Exception(ERROR_INFO, "unable to get ISO speed");
            }
            else
            {
                stringstream ss;
                if (speed == DC1394_ISO_SPEED_800)
                    ss << "Current ISO speed 800Mbps";
                else if (speed == DC1394_ISO_SPEED_400)
                    ss << "Current ISO speed 400Mbps";
                context_.tracer().info(ss.str());
            }

            if (dc1394_video_set_operation_mode(camera.at(cam_index), isoMode_.at(cam_index)) != DC1394_SUCCESS)
            {
                cleanup(camera.at(cam_index));
                stringstream ss;
                ss << "unable to set operation mode to ";
                if (isoMode_.at(cam_index) == DC1394_OPERATION_MODE_1394B)
                    ss << "1394B";
                else
                    ss << "Legacy 1394A";
                context_.tracer().info(ss.str());
                throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
            }

            //It appears you can set any ISO speed to the camera and it will never complain here
            //However, when setting the ROI later it will complain, so the ISO speed needs to match
            //that of the camera manual
            if (dc1394_video_set_iso_speed(camera.at(cam_index), isoSpeed_.at(cam_index)) != DC1394_SUCCESS)
            {
                cleanup(camera.at(cam_index));
                stringstream ss;
                ss << "Unable to set ISO speed to ";
                if (isoSpeed_.at(cam_index) == DC1394_ISO_SPEED_800)
                    ss << "800Mbps";
                else
                    ss << "400Mbps";
                context_.tracer().info(ss.str());
                throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
            }

            dc1394video_mode_t mmode;
            if (dc1394_video_get_mode(camera.at(cam_index), &mmode) != DC1394_SUCCESS)
            {
                cleanup(camera.at(cam_index));
                throw gbxutilacfr::Exception(ERROR_INFO, "unable to get current video mode");
            }
            else
            {
                stringstream ss;
                ss << "Current mode:" << mmode;
                context_.tracer().info(ss.str());
            }
            // setup capture
            if (dc1394_video_set_mode(camera.at(cam_index), videoMode_.at(cam_index)) != DC1394_SUCCESS)
            {
                cleanup(camera.at(cam_index));
                throw gbxutilacfr::Exception(ERROR_INFO, "unable to set video mode");
            }

            if (dc1394_video_get_mode(camera.at(cam_index), &mmode) != DC1394_SUCCESS)
            {
                cleanup(camera.at(cam_index));
                throw gbxutilacfr::Exception(ERROR_INFO, "unable to retrieve set mode");
            }

            unsigned int width, height;

            if (dc1394_format7_get_max_image_size(camera.at(cam_index), videoMode_.at(cam_index), &width, &height) != DC1394_SUCCESS)
            {
                cleanup(camera.at(cam_index));
                throw gbxutilacfr::Exception(ERROR_INFO, "unable to query max image size");
            }
            else
            {
                stringstream ss;
                ss << "Supported MAX width:" << width << " height:" << height;
                context_.tracer().info(ss.str());
            }

            if (dc1394_format7_get_image_size(camera.at(cam_index), videoMode_.at(cam_index), &width, &height) != DC1394_SUCCESS)
            {
                cleanup(camera.at(cam_index));
                throw gbxutilacfr::Exception(ERROR_INFO, "unable to query image size");
            }
            else
            {
                stringstream ss;
                ss << "Current image width:" << width << " height:" << height;
                context_.tracer().info(ss.str());
            }

            unsigned int min_bytes, max_bytes;
            if (dc1394_format7_get_packet_parameters(camera.at(cam_index), videoMode_.at(cam_index), &min_bytes, &max_bytes) != DC1394_SUCCESS)
            {
                cleanup(camera.at(cam_index));
                throw gbxutilacfr::Exception(ERROR_INFO, "unable to query packet size");
            }
            else
            {
                stringstream ss;
                ss << "Supported packet size MIN:" << min_bytes << " MAX:" << max_bytes;
                context_.tracer().info(ss.str());
            }

            status = dc1394_format7_set_roi(camera.at(cam_index), videoMode_.at(cam_index), DC1394_COLOR_CODING_RAW8, max_bytes - 1, 0, 0, width_.at(cam_index), height_.at(cam_index)); //9500
            if (status != DC1394_SUCCESS)
            {
                cleanup(camera.at(cam_index));
                throw gbxutilacfr::Exception(ERROR_INFO, "unable to set ROI");
            }

            //    status=dc1394_format7_set_roi(cam, mode,DC1394_COLOR_CODING_MONO8, 2500, 0,0, 1024, 4608);
            //    status=dc1394_format7_set_roi(cam, mode,DC1394_COLOR_CODING_MONO8, 2000, 0,0, 512, 9216);
            //    status=dc1394_format7_set_roi(cam, mode,DC1394_COLOR_CODING_MONO8, 2000, 0,0, 512, 2015);
            //DC1394_USE_MAX_AVAIL

            unsigned int bytes_per_packet;
            unsigned int left, top;
            dc1394color_coding_t color_coding;

            status = dc1394_format7_get_roi(camera.at(cam_index), videoMode_.at(cam_index), &color_coding, &bytes_per_packet, &left, &top, &width, &height);
            if (status != DC1394_SUCCESS)
            {
                cleanup(camera.at(cam_index));
                throw gbxutilacfr::Exception(ERROR_INFO, "error returning ROI");
            }
            else
            {
                stringstream ss;
                ss << "Current setup bytes per packet:" << bytes_per_packet << " ROI (left,top,width,height): (";
                ss << left << "," << top << "," << width << "," << height << ")";
                context_.tracer().info(ss.str());
            }

            // check the image size in bytes
            uint64_t totalbytes;
            status = dc1394_format7_get_total_bytes(camera.at(cam_index), videoMode_.at(cam_index), &totalbytes);
            if (status != DC1394_SUCCESS)
            {
                cleanup(camera.at(cam_index));
                throw gbxutilacfr::Exception(ERROR_INFO, "error checking image size");
            }
            else
            {
                stringstream ss;
                ss << "Bytes per frame:" << totalbytes;
                context_.tracer().info(ss.str());
            }


            // setup capture
            status = dc1394_capture_setup(camera.at(cam_index), 4, DC1394_CAPTURE_FLAGS_DEFAULT);
            if (status != DC1394_SUCCESS)
            {
                cleanup(camera.at(cam_index));
                stringstream ss;
                ss << "unable to setup camera\n"
                        "check to make sure that the video mode and ROI are supported by your camera";
                throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
            }

        }

        context_.tracer().info("Waiting for transmission... ");
    }

    // Start cameras transmitting
    dc1394switch_t is_iso_on;
    for (unsigned int cam_index = 0; cam_index < camera.size(); cam_index++) {
        if (dc1394_video_get_transmission(camera.at(cam_index), &is_iso_on)!=DC1394_SUCCESS) {
            cout << "Could not get ISO status" << endl;
        }
        if (!is_iso_on && dc1394_video_set_transmission(camera.at(cam_index), DC1394_ON) != DC1394_SUCCESS) {
            cout << "Could not start ISO transmission" << endl;
        }
    }
    // Check all cameras are transmitting properly
    usleep(50000);
    for (unsigned int cam_index = 0; cam_index < camera.size(); ++cam_index) {
        dc1394switch_t status = DC1394_OFF;
        if (dc1394_video_get_transmission(camera.at(cam_index), &status) != DC1394_SUCCESS)
        {
            cleanup(camera.at(cam_index));

        }
        if (status != DC1394_OFF)
        {
            continue;
        }
        else {
            cleanup(camera.at(cam_index));
            stringstream ss;
            ss << "camera " << cam_index << " doesn't seem to want to turn on!";
            throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
        }
    }
}

Driver::~Driver()
{
    for (unsigned int cam_index = 0; cam_index < camera.size(); ++cam_index)
        cleanup(camera.at(cam_index));
}

void Driver::read(hydrointerfaces::MultiImage::Data &data)
{
    uint64_t last_stamp = 0;
    //context_.tracer().info("Grabbing frame(s) from camera(s)...");
    for (unsigned int cam_index = 0; cam_index < camera.size(); ++cam_index) {
        if (!grabFrame(cam_index)) {
            // Keep a record of latest timestamp
            if (frame_.at(cam_index)->timestamp > last_stamp) 
                last_stamp = frame_.at(cam_index)->timestamp;
        }
    }
    // Make sure buffers are synchronised
    if (synchroniseBuffers(last_stamp)) {
        stringstream ss;
        ss << "Multiimagedc1394camera Warning: Frames out of sync. Corrected.";
        string err_string = ss.str();
        context_.tracer().info(err_string.c_str());
    }

    for (unsigned int cam_index = 0; cam_index < camera.size(); ++cam_index) {
        memcpy(data.at(cam_index).pixelData, frame_.at(cam_index)->image, frame_.at(cam_index)->image_bytes);

        //Get the time from the frame timestamp
        data.at(cam_index).timeStampSec = (long) (frame_.at(cam_index)->timestamp / 1000000);
        data.at(cam_index).timeStampUsec = (long) (frame_.at(cam_index)->timestamp - ((double) data.at(cam_index).timeStampSec * 1000000));
    }
}

int Driver::synchroniseBuffers(uint64_t last_stamp) {
    int outOfSync = 0;
    // Check timestamps for inconsistencies
    for (unsigned int cam_index = 0; cam_index < frame_.size(); ++cam_index) {
        for (int64_t difference = last_stamp - frame_.at(cam_index)->timestamp;
            difference > 2000; difference =  last_stamp - frame_.at(cam_index)->timestamp) {
            outOfSync = 1;
            if (grabFrame(cam_index)) break;
        } 
    }
    return outOfSync;
}

int Driver::grabFrame(int cam_index) {
    int returnVal = 0;
    err_ = dc1394_capture_dequeue(camera.at(cam_index), DC1394_CAPTURE_POLICY_WAIT, &frame_.at(cam_index));

    if (err_ == -2)
    {
        returnVal = -1;
        context_.tracer().debug("Multiimagedc1394camera Error: No frame in buffer ....");
    }
    else
    {
        if (dc1394_capture_is_frame_corrupt(camera.at(cam_index), frame_.at(cam_index)))
        {
            returnVal = -1;
            context_.tracer().debug("Multiimagedc1394camera Error: Buffer corrupted ");
        }
        else
        {
            // If the buffer is full, frames have potentially been dropped
            if (frame_.at(cam_index)->frames_behind >= numBufs.at(cam_index) - 1) {
                stringstream ss;
                ss << "Multiimagedc1394camera warning: Frames potentially dropped";
                string err_string = ss.str();
                context_.tracer().info(err_string.c_str());
            }
        }
        dc1394_capture_enqueue(camera.at(cam_index), frame_.at(cam_index));
    }
    return returnVal;
}

void Driver::cleanup(dc1394camera_t* camera)
{
    dc1394_capture_stop(camera);
    dc1394_video_set_transmission(camera, DC1394_OFF);
    dc1394_camera_free(camera);
}

} //namespace

extern "C"
{

hydrointerfaces::MultiImageFactory *createMultiImageDriverFactory()
{
    return new multiimagedc1394camera::Factory;
}
}
