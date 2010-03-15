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

#define DC1394_FEATURE_INC          (DC1394_FEATURE_MAX - DC1394_FEATURE_MAX + 1)


const char *featuresStr[]={ 
    "Brightness" ,
    "Exposure",
    "Sharpness", 
    "White Balance", 
    "Hue", 
    "Saturation",
    "Gamma",
    "Shutter",
    "Gain",
    "Iris",
    "Focus",
    "Temperature",
    "Trigger",
    "TriggerDelay",
    "WhiteShading",
    "FrameRate",
    "Zoom",
    "Pan",
    "Tilt",
    "OpticalFilter",
    "CaptureSize",
    "CaptureQuality"
};

//const dc1394feature_id_t& operator++(dc1394feature_id_t& value) { 
  //  return (eValue = (dc1394feature_id_t)(value+1)); 
//} 

using namespace std;

namespace multiimagedc1394camera
{

Driver::Driver(const Config &cfg, const hydroutil::Context &context) :
config_(cfg),
context_(context)
{
    camera_.resize(config_.size());
    frame_.resize(config_.size());
    videoMode_.resize(config_.size());
    height_.resize(config_.size());
    width_.resize(config_.size());
    format7_.resize(config_.size());
    isoMode_.resize(config_.size());
    isoSpeed_.resize(config_.size());
    numBufs_.resize(config_.size());
    for (unsigned int camIndex = 0; camIndex < camera_.size(); ++camIndex)
    {

        //
        //Check the desired format, RGB8 and BGR8 are the same (with channels swapped)
        //GRAY8 is actually MONO8 in DC1394 standards
        //RAW8 is for bayer cameras and will invoke Format7 settings
        //

        dc1394color_coding_t colorCoding;

        if (config_.at(camIndex).format == "RGB8")
        {
            colorCoding = DC1394_COLOR_CODING_RGB8;
        }
        else if (config_.at(camIndex).format == "GRAY8")
        {
            colorCoding = DC1394_COLOR_CODING_MONO8;
        }
        else if (config_.at(camIndex).format == "RAW8"            
            || config_.at(camIndex).format == "BayerRG8" 
            || config_.at(camIndex).format == "BayerGR8" 
            || config_.at(camIndex).format == "BayerGB8" 
            || config_.at(camIndex).format == "BayerBG8")
        {
            colorCoding = DC1394_COLOR_CODING_RAW8;
        }
        else
        {
            std::stringstream sse;
            sse << "The driver can only provide RGB8, GRAY8 or RAW8 data at the moment,";
            sse << " you have requested =" << config_.at(camIndex).format;
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
            isoMode_.at(camIndex) = DC1394_OPERATION_MODE_1394B;
            isoSpeed_.at(camIndex) = DC1394_ISO_SPEED_800;
        }
        else if (mode == "1394A")
        {
            isoMode_.at(camIndex) = DC1394_OPERATION_MODE_LEGACY;
            isoSpeed_.at(camIndex) = DC1394_ISO_SPEED_400;
        }
        else
            throw gbxutilacfr::Exception(ERROR_INFO, "Unknown image driver Iso mode: '" + mode + "'");

        //
        //Read GUID of camera, used to distinguish cameras on the bus
        //Otherwise first available camera on bus selected
        //
        std::stringstream dc1394PrefixSs;
        dc1394PrefixSs << prefix << camIndex << ".";
        std::string dc1394prefix = dc1394PrefixSs.str();
        // "DC1394.N.Guid=..."
        string sGuid = prop.getPropertyWithDefault(dc1394prefix + "Guid", "Any");

        //
        //Copy desired width and height from the config settings
        //
        width_.at(camIndex) = static_cast<uint> (config_.at(camIndex).width);
        height_.at(camIndex) = static_cast<uint> (config_.at(camIndex).height);

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
        int connectToCam = camIndex;

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

        camera_.at(camIndex) = dc1394_camera_new(d, list->ids[connectToCam].guid);
        if (!camera_.at(camIndex))
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
            ss1 << "Working with the camera " << camera_.at(camIndex)->vendor << " " << camera_.at(camIndex)->model;
            context_.tracer().info(ss1.str());
        }

        // free the other cameras
        dc1394_camera_free_list(list);

        dc1394video_modes_t video_modes;
        if (dc1394_video_get_supported_modes(camera_.at(camIndex), &video_modes) != DC1394_SUCCESS)
        {
            cleanup(camera_.at(camIndex));
            stringstream ss;
            ss << "Can't get video modes for cam " << camIndex;
            throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
        }
        else
        {
            stringstream ss;
            ss << "Searching for " << config_.at(camIndex).format << " format";
            context_.tracer().info(ss.str());
        }

        bool videoModeOk = false;
        //format7 not set
        format7_.at(camIndex) = false;
        
        //Check for non-default feature settings in config file
        int featureIndex = 0;
        for(dc1394feature_t feature = DC1394_FEATURE_MIN; feature <= DC1394_FEATURE_MAX; feature = (dc1394feature_t)(feature+1)) { 
        // For all dc1394 features
            // Is there a setting enabled for this feature in the config file?
            // e.g. "DC1394.N.Shutter=..."
            string featureString;
            if (!prop.getProperty(dc1394prefix + featuresStr[featureIndex], featureString)) {
                unsigned int value;
                istringstream numberStream(featureString);
                if (!(numberStream >> value)) {
                    stringstream ss;
                    ss << "Unable to set value for feature " << featuresStr[featureIndex] << "\" on camera " << camIndex;
                    throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
                }
                setFeature(camIndex, feature, featuresStr[featureIndex], DC1394_FEATURE_MODE_MANUAL, value);
            // Is there a setting enabled for the absolute value of this feature in the config file?
            // e.g. "DC1394.N.ShutterAbs=..."
            } else if (!prop.getProperty(dc1394prefix + featuresStr[featureIndex] + "Abs", featureString)) {
                float value;
                istringstream numberStream(featureString);

                if (!(numberStream >> value)) {
                    stringstream ss;
                    ss << "Unable to set value for feature " << feature << "\" on camera " << camIndex;
                    throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
                }
                cout << "Setting absolute value: " << dc1394prefix + featuresStr[featureIndex] + "Abs: " << value << endl;
                setFeatureAbs(camIndex, feature, featuresStr[featureIndex], DC1394_FEATURE_MODE_MANUAL, value);
            } else {
                setFeature(camIndex, feature, featuresStr[featureIndex], DC1394_FEATURE_MODE_AUTO, 0);
            }
            featureIndex++;
        }
        
        // Set the number of dc1394 buffers
        // "DC1394.NumBufs=..."
        string numBufsString = prop.getPropertyWithDefault(prefix + "NumBufs", "16");
        numBufs_.at(camIndex) = 16;
        istringstream numBufsStream(numBufsString);
        if (!(numBufsStream>>numBufs_.at(camIndex))) {
                stringstream ss;
                ss << "Unable to set size of dc1394 buffer to \"" <<
                    numBufsStream.str() << ". Defaulting to 16.";
                throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
        }
        
        if (colorCoding == DC1394_COLOR_CODING_RAW8)
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
                        videoMode_.at(camIndex) = video_modes.modes[i];
                        videoModeOk = true;
                        format7_.at(camIndex) = true;
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
                    dc1394_get_color_coding_from_video_mode(camera_.at(camIndex), video_modes.modes[i], &coding);
                    if (coding == colorCoding)
                    {
                        videoMode_.at(camIndex) = video_modes.modes[i];
                        unsigned int width, height;
                        dc1394_get_image_size_from_video_mode(camera_.at(camIndex), videoMode_.at(camIndex), &width, &height);

                        stringstream ss;
                        ss << "Supported width:" << width << " height:" << height;
                        context_.tracer().info(ss.str());

                        if ((width == width_.at(camIndex)) && (height == height_.at(camIndex)))
                        {
                            videoModeOk = true;
                            break;
                        }
                    }
                }
            }
        }

        if (!format7_.at(camIndex))
        {
            //Need to check width / height here against config
            unsigned int width, height;
            dc1394_get_image_size_from_video_mode(camera_.at(camIndex), videoMode_.at(camIndex), &width, &height);

            if (width_.at(camIndex) != width)
            {
                stringstream ss;
                ss << "config does not match image properties: actual width=" << width << " config width=" << width_.at(camIndex);
                throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
            }

            if (height_.at(camIndex) != height)
            {
                stringstream ss;
                ss << "config does not match image properties: actual height=" << height << " config height=" << height_.at(camIndex);
                throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
            }

            // double check that we found a video mode that is desired color coding
            dc1394color_coding_t coding;
            dc1394_get_color_coding_from_video_mode(camera_.at(camIndex), videoMode_.at(camIndex), &coding);
            if ((coding != colorCoding))
            {
                cleanup(camera_.at(camIndex));
                stringstream ss;
                ss << "Could not get a valid desired color mode: " << config_.at(camIndex).format;
                throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
            }

            dc1394_video_set_operation_mode(camera_.at(camIndex), isoMode_.at(camIndex));
            dc1394_video_set_iso_speed(camera_.at(camIndex), isoSpeed_.at(camIndex));

            // get available frameRates
            double minDiff = DBL_MAX;
            dc1394framerates_t frameRates;
            dc1394framerate_t frameRate;
            dc1394framerate_t bestFps = (dc1394framerate_t) - 1;
            
            std::stringstream MulticamPrefixSs;
            MulticamPrefixSs << camIndex << ".";
            std::string multicam_prefix = MulticamPrefixSs.str();
            // "N.FrameRate=..."
            string sFps = prop.getPropertyWithDefault(multicam_prefix + "FrameRate", "30");
            if (sFps == "1.875")
            {
                frameRate = DC1394_FRAMERATE_1_875;
            }
            else if (sFps == "3.75")
            {
                frameRate = DC1394_FRAMERATE_3_75;
            }
            else if (sFps == "7.5")
            {
                frameRate = DC1394_FRAMERATE_7_5;
            }
            else if (sFps == "15")
            {
                frameRate = DC1394_FRAMERATE_15;
            }
            else if (sFps == "30")
            {
                frameRate = DC1394_FRAMERATE_30;
            }
            else if (sFps == "60")
            {
                frameRate = DC1394_FRAMERATE_60;
            }
            else
            {
                std::stringstream sse;
                sse << "The driver can only provide frameRates of 1.875, 3.75, ";
                sse << "7.5, 15, 30 or 60. You have requested " << sFps;
                throw gbxutilacfr::Exception(ERROR_INFO, sse.str());
            }
            if (dc1394_video_get_supported_framerates(camera_.at(camIndex), videoMode_.at(camIndex), &frameRates) != DC1394_SUCCESS)
            {
                cleanup(camera_.at(camIndex));
                throw gbxutilacfr::Exception(ERROR_INFO, "Can't get a frameRate");
            }

            // Find the closest available frameRate to the one requested and set it
            for (int i = 0; i < (int)frameRates.num; i++)
            {
                dc1394framerate_t ifps = frameRates.framerates[i];
                double diff = fabs(ifps - frameRate);
                if (diff < minDiff)
                {
                    minDiff = diff;
                    bestFps = ifps;
                }
            }

            // setup capture
            context_.tracer().info("Setting capture");

            dc1394_video_set_mode(camera_.at(camIndex), videoMode_.at(camIndex));
            if ((int)bestFps >= 0)
                dc1394_video_set_framerate(camera_.at(camIndex), bestFps);
            if (dc1394_capture_setup(camera_.at(camIndex), numBufs_.at(camIndex) , DC1394_CAPTURE_FLAGS_DEFAULT) != DC1394_SUCCESS)
            {
                cleanup(camera_.at(camIndex));
                stringstream ss;
                ss << "unable to setup camera\n"
                        "check to make sure that the video mode and frameRate are supported by your camera\n"
                        "video_mode =" << videoMode_.at(camIndex) << "frameRate =" << frameRate << "\n"
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
            if (dc1394_video_get_iso_speed(camera_.at(camIndex), &speed) != DC1394_SUCCESS)
            {
                cleanup(camera_.at(camIndex));
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

            if (dc1394_video_set_operation_mode(camera_.at(camIndex), isoMode_.at(camIndex)) != DC1394_SUCCESS)
            {
                cleanup(camera_.at(camIndex));
                stringstream ss;
                ss << "unable to set operation mode to ";
                if (isoMode_.at(camIndex) == DC1394_OPERATION_MODE_1394B)
                    ss << "1394B";
                else
                    ss << "Legacy 1394A";
                context_.tracer().info(ss.str());
                throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
            }

            //It appears you can set any ISO speed to the camera and it will never complain here
            //However, when setting the ROI later it will complain, so the ISO speed needs to match
            //that of the camera manual
            if (dc1394_video_set_iso_speed(camera_.at(camIndex), isoSpeed_.at(camIndex)) != DC1394_SUCCESS)
            {
                cleanup(camera_.at(camIndex));
                stringstream ss;
                ss << "Unable to set ISO speed to ";
                if (isoSpeed_.at(camIndex) == DC1394_ISO_SPEED_800)
                    ss << "800Mbps";
                else
                    ss << "400Mbps";
                context_.tracer().info(ss.str());
                throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
            }

            dc1394video_mode_t mmode;
            if (dc1394_video_get_mode(camera_.at(camIndex), &mmode) != DC1394_SUCCESS)
            {
                cleanup(camera_.at(camIndex));
                throw gbxutilacfr::Exception(ERROR_INFO, "unable to get current video mode");
            }
            else
            {
                stringstream ss;
                ss << "Current mode:" << mmode;
                context_.tracer().info(ss.str());
            }
            // setup capture
            if (dc1394_video_set_mode(camera_.at(camIndex), videoMode_.at(camIndex)) != DC1394_SUCCESS)
            {
                cleanup(camera_.at(camIndex));
                throw gbxutilacfr::Exception(ERROR_INFO, "unable to set video mode");
            }

            if (dc1394_video_get_mode(camera_.at(camIndex), &mmode) != DC1394_SUCCESS)
            {
                cleanup(camera_.at(camIndex));
                throw gbxutilacfr::Exception(ERROR_INFO, "unable to retrieve set mode");
            }

            unsigned int width, height;

            if (dc1394_format7_get_max_image_size(camera_.at(camIndex), videoMode_.at(camIndex), &width, &height) != DC1394_SUCCESS)
            {
                cleanup(camera_.at(camIndex));
                throw gbxutilacfr::Exception(ERROR_INFO, "unable to query max image size");
            }
            else
            {
                stringstream ss;
                ss << "Supported MAX width:" << width << " height:" << height;
                context_.tracer().info(ss.str());
            }

            if (dc1394_format7_get_image_size(camera_.at(camIndex), videoMode_.at(camIndex), &width, &height) != DC1394_SUCCESS)
            {
                cleanup(camera_.at(camIndex));
                throw gbxutilacfr::Exception(ERROR_INFO, "unable to query image size");
            }
            else
            {
                stringstream ss;
                ss << "Current image width:" << width << " height:" << height;
                context_.tracer().info(ss.str());
            }

            unsigned int min_bytes, max_bytes;
            if (dc1394_format7_get_packet_parameters(camera_.at(camIndex), videoMode_.at(camIndex), &min_bytes, &max_bytes) != DC1394_SUCCESS)
            {
                cleanup(camera_.at(camIndex));
                throw gbxutilacfr::Exception(ERROR_INFO, "unable to query packet size");
            }
            else
            {
                stringstream ss;
                ss << "Supported packet size MIN:" << min_bytes << " MAX:" << max_bytes;
                context_.tracer().info(ss.str());
            }

            status = dc1394_format7_set_roi(camera_.at(camIndex), videoMode_.at(camIndex), DC1394_COLOR_CODING_RAW8, max_bytes - 1, 0, 0, width_.at(camIndex), height_.at(camIndex)); //9500
            if (status != DC1394_SUCCESS)
            {
                cleanup(camera_.at(camIndex));
                throw gbxutilacfr::Exception(ERROR_INFO, "unable to set ROI");
            }

            //    status=dc1394_format7_set_roi(cam, mode,DC1394_COLOR_CODING_MONO8, 2500, 0,0, 1024, 4608);
            //    status=dc1394_format7_set_roi(cam, mode,DC1394_COLOR_CODING_MONO8, 2000, 0,0, 512, 9216);
            //    status=dc1394_format7_set_roi(cam, mode,DC1394_COLOR_CODING_MONO8, 2000, 0,0, 512, 2015);
            //DC1394_USE_MAX_AVAIL

            unsigned int bytes_per_packet;
            unsigned int left, top;
            dc1394color_coding_t color_coding;

            status = dc1394_format7_get_roi(camera_.at(camIndex), videoMode_.at(camIndex), &color_coding, &bytes_per_packet, &left, &top, &width, &height);
            if (status != DC1394_SUCCESS)
            {
                cleanup(camera_.at(camIndex));
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
            status = dc1394_format7_get_total_bytes(camera_.at(camIndex), videoMode_.at(camIndex), &totalbytes);
            if (status != DC1394_SUCCESS)
            {
                cleanup(camera_.at(camIndex));
                throw gbxutilacfr::Exception(ERROR_INFO, "error checking image size");
            }
            else
            {
                stringstream ss;
                ss << "Bytes per frame:" << totalbytes;
                context_.tracer().info(ss.str());
            }


            // setup capture
            status = dc1394_capture_setup(camera_.at(camIndex), 4, DC1394_CAPTURE_FLAGS_DEFAULT);
            if (status != DC1394_SUCCESS)
            {
                cleanup(camera_.at(camIndex));
                stringstream ss;
                ss << "unable to setup camera\n"
                        "check to make sure that the video mode and ROI are supported by your camera";
                throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
            }

        }

        context_.tracer().info("Waiting for transmission... ");
    }

    // Start cameras transmitting
    dc1394switch_t isIsoOn;
    for (unsigned int camIndex = 0; camIndex < camera_.size(); camIndex++) {
        if (dc1394_video_get_transmission(camera_.at(camIndex), &isIsoOn)!=DC1394_SUCCESS) {
            cout << "Could not get ISO status" << endl;
        }
        if (!isIsoOn && dc1394_video_set_transmission(camera_.at(camIndex), DC1394_ON) != DC1394_SUCCESS) {
            cout << "Could not start ISO transmission" << endl;
        }
    }
    // Check all cameras are transmitting properly
    usleep(50000);
    for (unsigned int camIndex = 0; camIndex < camera_.size(); ++camIndex) {
        dc1394switch_t status = DC1394_OFF;
        if (dc1394_video_get_transmission(camera_.at(camIndex), &status) != DC1394_SUCCESS)
        {
            cleanup(camera_.at(camIndex));

        }
        if (status != DC1394_OFF)
        {
            continue;
        }
        else {
            cleanup(camera_.at(camIndex));
            stringstream ss;
            ss << "camera " << camIndex << " doesn't seem to want to turn on!";
            throw gbxutilacfr::Exception(ERROR_INFO, ss.str());
        }
    }
}

Driver::~Driver()
{
    for (unsigned int camIndex = 0; camIndex < camera_.size(); ++camIndex)
        cleanup(camera_.at(camIndex));
}

void Driver::read(hydrointerfaces::MultiImage::Data &data)
{
    uint64_t lastStamp = 0;
    //context_.tracer().info("Grabbing frame(s) from camera(s)...");
    for (unsigned int camIndex = 0; camIndex < camera_.size(); ++camIndex) {
        if (!grabFrame(camIndex)) {
            // Keep a record of latest timestamp
            if (frame_.at(camIndex)->timestamp > lastStamp) 
                lastStamp = frame_.at(camIndex)->timestamp;
        }
    }
    // Make sure buffers are synchronised
    if (synchroniseBuffers(lastStamp)) {
        stringstream ss;
        ss << "Multiimagedc1394camera Warning: Frames out of sync. Corrected by dropping out of sync frames.";
        string err_string = ss.str();
        context_.tracer().info(err_string.c_str());
    }

    for (unsigned int camIndex = 0; camIndex < camera_.size(); ++camIndex) {
        memcpy(data.at(camIndex).pixelData, frame_.at(camIndex)->image, frame_.at(camIndex)->image_bytes);

        //Get the time from the frame timestamp
        data.at(camIndex).timeStampSec = (long) (frame_.at(camIndex)->timestamp / 1000000);
        data.at(camIndex).timeStampUsec = (long) (frame_.at(camIndex)->timestamp - ((double) data.at(camIndex).timeStampSec * 1000000));
    }
}

bool Driver::synchroniseBuffers(uint64_t lastStamp) {
    bool outOfSync = false, outOfSyncFlag = false;
    do { // Check timestamps for inconsistencies
        for (unsigned int camIndex = 0; camIndex < frame_.size(); ++camIndex) {
            int64_t difference = lastStamp - frame_.at(camIndex)->timestamp;
            if (difference > 2000) {
                outOfSync = true;
                outOfSyncFlag = true;
                // If something went wrong grabbing the frame, break out
                if (grabFrame(camIndex)) break;
                // Make sure that we still have the newest timestamp
                if (frame_.at(camIndex)->timestamp > lastStamp)
                    lastStamp = frame_.at(camIndex)->timestamp;
            } 
        }
    } while (outOfSync);
    return outOfSyncFlag;
}

int Driver::grabFrame(int camIndex) {
    int returnVal = 0;
    err_ = dc1394_capture_dequeue(camera_.at(camIndex), DC1394_CAPTURE_POLICY_WAIT, &frame_.at(camIndex));

    if (err_ == -2)
    {
        returnVal = -1;
        context_.tracer().debug("Multiimagedc1394camera Error: No frame in buffer ....");
    }
    else
    {
        if (dc1394_capture_is_frame_corrupt(camera_.at(camIndex), frame_.at(camIndex)))
        {
            returnVal = -1;
            context_.tracer().debug("Multiimagedc1394camera Error: Buffer corrupted ");
        }
        else
        {
            // If the buffer is full, frames have potentially been dropped
            if (frame_.at(camIndex)->frames_behind >= numBufs_.at(camIndex) - 1) {
                stringstream ss;
                ss << "Multiimagedc1394camera warning: Frames potentially dropped";
                string err_string = ss.str();
                context_.tracer().info(err_string.c_str());
            }
        }
        dc1394_capture_enqueue(camera_.at(camIndex), frame_.at(camIndex));
    }
    return returnVal;
}


// Set a feature
bool Driver::setFeature(unsigned int camIndex, dc1394feature_t feature, const char*& featureString, dc1394feature_mode_t mode, unsigned int value) {
    dc1394bool_t present;
    dc1394_feature_is_present(camera_.at(camIndex) , feature, &present); // Make sure the feature is present
    if (!present && mode == DC1394_FEATURE_MODE_MANUAL) {
      stringstream ss;
      ss << featureString << " is not available on camera " << camIndex << ", cannot set manual value.";
      return false;
    }
    // Make sure it's within the bounds
    unsigned int minVal, maxVal;
    dc1394_feature_get_boundaries(camera_.at(camIndex), feature, &minVal, &maxVal); 
    if (mode == DC1394_FEATURE_MODE_MANUAL && (value < minVal || value > maxVal)) {
      stringstream ss;
      ss << "Requested " << featureString << " value of " << value << " is outside available range of "
          << minVal << " to " << maxVal << " on camera " << camIndex;
      context_.tracer().info(ss.str());
      return false;
    }
    // Make sure it has manual mode
    //dc1394_feature_has_manual_mode(camera_.at(camIndex),feature,present);
    if (mode == DC1394_FEATURE_MODE_MANUAL) {
        stringstream ss;
        ss << "Setting " << featureString << " to \"" << value << "\" on camera " << camIndex;
        context_.tracer().info(ss.str());
        // Set the feature to Manual and set the value
        if (dc1394_feature_set_mode(camera_.at(camIndex),feature,mode) != DC1394_SUCCESS) {
            stringstream ss;
            ss << "Could not set " << featureString << " to manual on camera. Unknown error"<< camIndex;
            return false;
        }
        if (dc1394_feature_set_value(camera_.at(camIndex),feature,value) != DC1394_SUCCESS) {
            stringstream ss;
            ss << "Could not set " << featureString << " to value " << value << " on camera "<< camIndex;
           return false;
        }
    } else if (mode == DC1394_FEATURE_MODE_AUTO) {
        // Ensure that shutter is set to Auto
        stringstream ss;
        ss << "Setting " << featureString << " to auto on camera " << camIndex;
        context_.tracer().info(ss.str(),10);
        if (dc1394_feature_set_mode(camera_.at(camIndex),feature,mode)!= DC1394_SUCCESS) {
            stringstream ss;
            ss << "Could not set " << featureString << " to auto on camera "<< camIndex;
            return false;
        }
    } else {
        stringstream ss;
        ss << "Could not set " << featureString << " to manual on camera "<< camIndex
                << ". It is not supported";
        return false;
    }
    return true;
}


// Set a feature with absolute value
bool Driver::setFeatureAbs(unsigned int camIndex, dc1394feature_t feature, const char*& featureString, dc1394feature_mode_t mode, float value) {
    dc1394bool_t present;
    dc1394_feature_is_present(camera_.at(camIndex) , feature, &present); // Make sure the feature is present
    if (!present && mode == DC1394_FEATURE_MODE_MANUAL) {
      stringstream ss;
      ss << featureString << " is not available on camera " << camIndex << ", cannot set manual value.";
      return false;
    }
    // Make sure it has absolute control
    dc1394_feature_has_absolute_control(camera_.at(camIndex), feature, &present); 
    if (mode == DC1394_FEATURE_MODE_MANUAL && !present) {
      stringstream ss;
      ss << "Abolute value for " << featureString << " is not available on camera " << camIndex << ", cannot set manual value.";
      return false;
    }
    else if (!present) {
        return false;
    }
    // Make sure it's within the bounds
    float minVal, maxVal;
    dc1394_feature_get_absolute_boundaries(camera_.at(camIndex), feature, &minVal, &maxVal); 
    if (mode == DC1394_FEATURE_MODE_MANUAL && (value < minVal || value > maxVal)) {
      stringstream ss;
      ss << "Requested absolute " << featureString << " value of " << value << " is outside available range of "
          << minVal << " to " << maxVal << " on camera " << camIndex;
      context_.tracer().info(ss.str());
      return false;
    }
    cout << "minVal: " << minVal << ", maxVal: " << maxVal << endl;
    // Make sure it has manual mode
    //dc1394_feature_has_manual_mode(camera_.at(camIndex), feature, present);
    if (mode == DC1394_FEATURE_MODE_MANUAL) {
        stringstream ss;
        ss << "Setting " << featureString << " to \"" << value << "\" on camera " << camIndex;
        context_.tracer().info(ss.str());
        if (dc1394_feature_set_mode(camera_.at(camIndex),feature,mode) != DC1394_SUCCESS) {
            stringstream ss;
            ss << "Could not set " << featureString << " to manual on camera "<< camIndex;
            return false;
        }
        if (dc1394_feature_set_absolute_control(camera_.at(camIndex),feature,DC1394_ON) != DC1394_SUCCESS) {
            stringstream ss;
            ss << "Could not set absolute control on feature" << featureString << " to 'on' on camera "<< camIndex;
            return false;
        }
        if (dc1394_feature_set_absolute_value(camera_.at(camIndex),feature,value) != DC1394_SUCCESS) {
            stringstream ss;
            ss << "Could not set " << featureString << " to value " << value << " on camera "<< camIndex;
            return false;
        }
    } else if (mode == DC1394_FEATURE_MODE_AUTO) {
        // Ensure that feature is set to auto
        stringstream ss;
        ss << "Setting " << featureString << " to auto on camera " << camIndex;
        context_.tracer().info(ss.str(),10);
        if (dc1394_feature_set_absolute_control(camera_.at(camIndex),feature,DC1394_OFF) != DC1394_SUCCESS) {
            stringstream ss;
            ss << "Could not set absolute control on feature" << featureString << " to 'off' on camera "<< camIndex;
            return false;
        }
        if (dc1394_feature_set_mode(camera_.at(camIndex),feature,mode)!= DC1394_SUCCESS) {
            stringstream ss;
            ss << "Could not set " << featureString << " to auto on camera "<< camIndex;
            return false;
        }
    } else {
        stringstream ss;
        ss << "Could not set " << featureString << " to manual on camera "<< camIndex
                << ". It is not supported";
        return false;
    }
    return true;
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
