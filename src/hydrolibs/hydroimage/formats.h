/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_IMAGE_FORMATS_H
#define HYDRO_IMAGE_FORMATS_H

#include <string>
#include <iostream>
#include <map>

namespace hydroimage
{
    
    /*!
     * @brief An class that represents a particular image format and all of the information needed by various algorithms to work with it.
     *
     * The class will return the properties (such as depth and number of channels)
     * of an image format given a representative string.
     *   Typical usage:
        @verbatim
#include <hydroimage/formats.h>
// ... other code
string format = "RGB8";
ImageFormat imageFormat = ImageFormat::find( format );
int numChannels = imageFormat.getNumberOfChannels();
        @endverbatim

    */
    class ImageFormat
    {
        public:
            ImageFormat();
            virtual ~ImageFormat() {}
            
            //! @brief Given the input string, the particular image format object will be returned which in turn can be queried about particular properties.
            static ImageFormat find( const std::string& format );

            const std::string getFormatString();
            unsigned int getNumberOfChannels();
            unsigned int getBitsPerPixel();
            unsigned int getBytesPerPixel();
        
        protected:
            //! @brief Instantiates all the classes representing each of the formats
            //! This will add each format to the list of known formats that
            //! can be queried.
            static void initialize();

            //! @brief Each particular image format class will call this constructor which automatically adds this to the list of known formats.
            //!\param[in] formatString string defining the image format
            //!\param[in] channels number of channels eg. RGB has 3 channels
            //!\param[in] bits number of bits per pixel eg. RGB has 8 bits per channel so has 24 bits per pixel
            //!\param[in] bytes number of bytes ber pixel = bits/8
            ///!\return The particular image format object that is defined by the input string
            ImageFormat( std::string formatString, unsigned int channels, unsigned int bits, unsigned int bytes);
            //! @brief add the image format to the list of known formats
            static void add( ImageFormat formatObject );

        private:
            std::string formatString_;
            unsigned int channels_;
            unsigned int bitsPerPixel_;
            unsigned int bytesPerPixel_;
            //! @brief the searchable list of known formats
            static std::map<std::string, ImageFormat> formats_;
    };

    class BayerRG8 : public ImageFormat
    {
        public:
            BayerRG8() : ImageFormat("BayerRG8", 1, 8, 1) {}
    };

    class BayerGR8 : public ImageFormat
    {
        public:
            BayerGR8() : ImageFormat("BayerGR8", 1, 8, 1) {}
    };

    class BayerGB8 : public ImageFormat
    {
        public:
            BayerGB8() : ImageFormat("BayerGB8", 1, 8, 1) {}
    };

    class BayerBG8 : public ImageFormat
    {
        public:
            BayerBG8() : ImageFormat("BayerBG8", 1, 8, 1) {}
    };

    class RGB8 : public ImageFormat
    {
        public:
            RGB8() : ImageFormat("RGB8", 3, 24, 3) {}
    };

    class BGR8 : public ImageFormat
    {
        public:
            BGR8() : ImageFormat("BGR8", 3, 24, 3) {}
    };

    class RGBA8 : public ImageFormat
    {
        public:
            RGBA8() : ImageFormat("RGBA8", 4, 32, 4) {}
    };

    class BGRA8 : public ImageFormat
    {
        public:
            BGRA8() : ImageFormat("BGRA8", 4, 32, 4) {}
    };

    class GRAY8 : public ImageFormat
    {
        public:
            GRAY8() : ImageFormat("GRAY8", 1, 8, 1) {}
    };

    class RAW8 : public ImageFormat
    {
        public:
            RAW8() : ImageFormat("RAW8", 1, 8, 1) {}
    };

}

#endif

