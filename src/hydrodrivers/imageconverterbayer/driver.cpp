#include "driver.h"

Driver::Driver( const hydroutil::Context &context)
: context_(context)
{
    inputFormats_.push_back(hydroimage::BayerRG8);
    inputFormats_.push_back(hydroimage::BayerGB8);
    inputFormats_.push_back(hydroimage::BayerGR8);
    inputFormats_.push_back(hydroimage::BayerBG8);
    outputFormats_.push_back(hydroimage::RGBA8);
    outputFormats_.push_back(hydroimage::BGRA8);
    outputFormats_.push_back(hydroimage::RGB8);
    outputFormats_.push_back(hydroimage::BGR8);
    outputFormats_.push_back(hydroimage::GRAY8);
}

Driver::~Driver()
{
}

void Driver::convert( Data &inputData, Data &outputData )
{
    hydroimage::ImageFormat inputFormat = hydroimage::ImageFormat::find(inputData.format);
    hydroimage::ImageFormat outputFormat = hydroimage::ImageFormat::find(outputData.format);
}

size_t Driver::getOutputBufferSize(const Config &config, const std::string &outputFormat)
{
    hydroimage::ImageFormat outputFormat = hydroimage::ImageFormat::find( outputFormat );

    return config.width*config.height*outputFormat.getBytesPerPixel();
}

const std::vector<std::string>& Driver::getInputFormats() const
{
    std::vector<std::string> inputStrings;

    std::vector<hydroimage::ImageFormats>::const_iterator iter = inputFormats_.begin();
    while( iter != inputFormats_.end() )
    {
        inputStrings.push_back(iter->getFormatString());
    }

    return inputFormats_;
}

const std::vector<std::string>& Driver::getOutputFormats(const std::string& inputFormat) const
{
    std::vector<std::string> outputStrings;

    std::vector<hydroimage::ImageFormats>::const_iterator iter = outputFormats_.begin();
    while( iter != inputFormats_.end() )
    {
        outputStrings.push_back(iter->getFormatString());
    }

    return outputStrings;

}

