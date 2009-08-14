/*
 * List cameras and their modes using libdc1394
 *
 * Written by Damien Douxchamps <ddouxchamps@users.sf.net>
 * Modified by Thomas Burdick <thomas.burdick@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


#include <stdio.h>
#include <stdint.h>
#include <dc1394/dc1394.h>
#include <stdlib.h>
#include <inttypes.h>


#define IMAGE_FILE_NAME "ImageRGB.ppm"

void print_encoding( uint32_t encoding )
{
#define print_encoding_case(A) case A: printf("%s", #A); break;
    switch( encoding) {
        print_encoding_case(DC1394_COLOR_FILTER_RGGB);
        print_encoding_case(DC1394_COLOR_FILTER_GBRG);
        print_encoding_case(DC1394_COLOR_FILTER_GRBG);
        print_encoding_case(DC1394_COLOR_FILTER_BGGR);
    default:
        break;
    }
}

/*-----------------------------------------------------------------------
 *  Prints the type of format to standard out
 *-----------------------------------------------------------------------*/
void print_format( uint32_t format )
{
#define print_case(A) case A: printf("%s", #A); break;

    switch( format ) {
        print_case(DC1394_VIDEO_MODE_160x120_YUV444);
        print_case(DC1394_VIDEO_MODE_320x240_YUV422);
        print_case(DC1394_VIDEO_MODE_640x480_YUV411);
        print_case(DC1394_VIDEO_MODE_640x480_YUV422);
        print_case(DC1394_VIDEO_MODE_640x480_RGB8);
        print_case(DC1394_VIDEO_MODE_640x480_MONO8);
        print_case(DC1394_VIDEO_MODE_640x480_MONO16);
        print_case(DC1394_VIDEO_MODE_800x600_YUV422);
        print_case(DC1394_VIDEO_MODE_800x600_RGB8);
        print_case(DC1394_VIDEO_MODE_800x600_MONO8);
        print_case(DC1394_VIDEO_MODE_1024x768_YUV422);
        print_case(DC1394_VIDEO_MODE_1024x768_RGB8);
        print_case(DC1394_VIDEO_MODE_1024x768_MONO8);
        print_case(DC1394_VIDEO_MODE_800x600_MONO16);
        print_case(DC1394_VIDEO_MODE_1024x768_MONO16);
        print_case(DC1394_VIDEO_MODE_1280x960_YUV422);
        print_case(DC1394_VIDEO_MODE_1280x960_RGB8);
        print_case(DC1394_VIDEO_MODE_1280x960_MONO8);
        print_case(DC1394_VIDEO_MODE_1600x1200_YUV422);
        print_case(DC1394_VIDEO_MODE_1600x1200_RGB8);
        print_case(DC1394_VIDEO_MODE_1600x1200_MONO8);
        print_case(DC1394_VIDEO_MODE_1280x960_MONO16);
        print_case(DC1394_VIDEO_MODE_1600x1200_MONO16);
        print_case(DC1394_VIDEO_MODE_EXIF);
        print_case(DC1394_VIDEO_MODE_FORMAT7_0);
        print_case(DC1394_VIDEO_MODE_FORMAT7_1);
        print_case(DC1394_VIDEO_MODE_FORMAT7_2);
        print_case(DC1394_VIDEO_MODE_FORMAT7_3);
        print_case(DC1394_VIDEO_MODE_FORMAT7_4);
        print_case(DC1394_VIDEO_MODE_FORMAT7_5);
        print_case(DC1394_VIDEO_MODE_FORMAT7_6);
        print_case(DC1394_VIDEO_MODE_FORMAT7_7);

    default:
        dc1394_log_error("Unknown format\n");
        exit(1);
    }

}

/*-----------------------------------------------------------------------
 *  Returns the number of pixels in the image based upon the format
 *-----------------------------------------------------------------------*/
uint32_t get_num_pixels(dc1394camera_t *camera, uint32_t format ) {
    uint32_t w,h;

    dc1394_get_image_size_from_video_mode(camera, format,&w,&h);

    return w*h;
}

/*-----------------------------------------------------------------------
 *  Prints the type of color encoding
 *-----------------------------------------------------------------------*/
void print_color_coding( uint32_t color_id )
{
    switch( color_id ) {
    case DC1394_COLOR_CODING_MONO8:
        printf("MONO8");
        break;
    case DC1394_COLOR_CODING_YUV411:
        printf("YUV411");
        break;
    case DC1394_COLOR_CODING_YUV422:
        printf("YUV422");
        break;
    case DC1394_COLOR_CODING_YUV444:
        printf("YUV444");
        break;
    case DC1394_COLOR_CODING_RGB8:
        printf("RGB8");
        break;
    case DC1394_COLOR_CODING_MONO16:
        printf("MONO16");
        break;
    case DC1394_COLOR_CODING_RGB16:
        printf("RGB16");
        break;
    case DC1394_COLOR_CODING_MONO16S:
        printf("MONO16S");
        break;
    case DC1394_COLOR_CODING_RGB16S:
        printf("RGB16S");
        break;
    case DC1394_COLOR_CODING_RAW8:
        printf("RAW8");
        break;
    case DC1394_COLOR_CODING_RAW16:
        printf("RAW16");
        break;
    default:
        dc1394_log_error("Unknown color coding = %d\n",color_id);
        exit(1);
    }
}

/*-----------------------------------------------------------------------
 *  Prints various information about the mode the camera is in
 *-----------------------------------------------------------------------*/
void print_mode_info( dc1394camera_t *camera , uint32_t mode )
{
    int j;

    printf("\tMode: ");
    print_format(mode);
    printf("\n");


    if(mode >= DC1394_VIDEO_MODE_FORMAT7_0 && mode <= DC1394_VIDEO_MODE_FORMAT7_7)
    {
        dc1394error_t err=0;
        dc1394format7mode_t info;
        dc1394_format7_get_mode_info(camera, mode, &info);
        DC1394_ERR(err,"Could not get format 7 info");

        printf("\t\tMax Size: %dx%d\n", info.size_x, info.size_y);
        /*printf("\tColor Codings:\n");
        for( int i = 0; i < info.color_codings.num; ++i)
        {
            printf("\t\t[%d]: ");
            print_color_coding(info.color_codings.modes[i]);
            printf("\n");
        }
        */

        printf("\t\tBayer Encoding: ");
        print_encoding(info.color_filter);
        printf("\n");

    }
    else
    {
        dc1394framerates_t framerates;
        dc1394error_t err;
        err=dc1394_video_get_supported_framerates(camera,mode,&framerates);
        DC1394_ERR(err,"Could not get frame rates");

        printf("\t\tFrame Rates:\n");
        for( j = 0; j < framerates.num; j++ ) {
            uint32_t rate = framerates.framerates[j];
            float f_rate;
            dc1394_framerate_as_float(rate,&f_rate);
            printf("\t\t\t[%d] rate = %f\n",j,f_rate );

        }
    }
}

/*-----------------------------------------------------------------------
 *  Releases the cameras and exits
 *-----------------------------------------------------------------------*/
void cleanup_and_exit(dc1394camera_t *camera)
{
    dc1394_video_set_transmission(camera, DC1394_OFF);
    dc1394_capture_stop(camera);
    dc1394_camera_free(camera);
    exit(1);
}

int main(int argc, char *argv[])
{
    dc1394camera_t *camera;
    dc1394_t * d;
    dc1394camera_list_t * list;

    dc1394error_t err;

    d = dc1394_new ();
    err=dc1394_camera_enumerate (d, &list);
    DC1394_ERR_RTN(err,"Failed to enumerate cameras");

    if (list->num == 0) {
        dc1394_log_error("No cameras found");
        return 1;
    }

    printf("Found %d cameras\n", list->num);

    for(int i = 0; i < list->num; ++i)
    {
        camera = dc1394_camera_new (d, list->ids[i].guid);
        if (!camera) {
            dc1394_log_error("Failed to initialize camera with guid %llx", list->ids[0].guid);
            return 1;
        }

        printf("Camera %d\n", i);
        printf("Vendor: %s \n", camera->vendor);
        printf("Model: %s \n", camera->model);
        printf("GUID: %"PRIx64"\n", camera->guid);

    
        dc1394video_modes_t modes;
        
        /*-----------------------------------------------------------------------
         *  list Capture Modes
         *-----------------------------------------------------------------------*/
        err=dc1394_video_get_supported_modes(camera, &modes);
        DC1394_ERR_RTN(err,"Could not get list of modes");

        for(int j = 0; j < modes.num; ++j)
        {
            print_mode_info(camera, modes.modes[j]);
        }

        dc1394_camera_free(camera);
    }

    dc1394_camera_free_list(list);

    /*-----------------------------------------------------------------------
     *  close camera
     *-----------------------------------------------------------------------*/
    dc1394_free(d);

    return 0;
}
