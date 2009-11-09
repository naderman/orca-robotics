/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_DRIVERS_IMAGECONVERTER_BAYER_H
#define HYDRO_DRIVERS_IMAGECONVERTER_BAYER_H

#include <string>
#include <vector>
#include <hydroutil/context.h>

namespace imageconverterbayer
{

class Driver : public hydrointerfaces::ImageConverter
{

public:
    Driver( const hydroutil::Context &context );
    virtual ~Driver();

    virtual void convert( Data &inputData, Data &outputData );
   
    virtual void getOutputBufferSize(const Config& config, const std::string& outputFormat);
    virtual const std::vector<std::string>& getInputFormats();
    virtual const std::vector<std::string>& getOutputFormats( const std::string &inputFormat);

private:
    std::vector<hydroimage::ImageFormat> inputFormats_;
    std::vector<hydroimage::ImageFormat> outputFormats_;

};

}
#endif

