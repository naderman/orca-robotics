/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_IMAGE_STRUCTURES_H
#define HYDRO_IMAGE_STRUCTURES_H

#include <string>
#include <sstream>

namespace hydroimage
{

    //! Image Configuration Structure
    class Config
    {
        public:
            Config();
            bool validate() const;
            std::string toString() const;
            bool operator==( const Config & other );
            bool operator!=( const Config & other );

            //! image width [pixels]
            size_t width;
            //! image height [pixels]
            size_t height;
            //! image byte size [bytes]
            size_t size;
            //! image format string
            std::string format;
    };

    //! Image Data Structure
    class Data
    {
        public:
            Data()
                : haveWarnings(false)
            {}

            unsigned char* pixelData;
            int            timeStampSec;
            int            timeStampUsec;
            bool           haveWarnings;
            //! if 'haveWarnings' is set, 'warnings' will contain diagnostic information.
            std::string    warnings;

    };
}

#endif
