/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <stdint.h>
#include <string>
#include <sstream>

#include <orcaice/orcaice.h>
#include <orcaobj/camera.h>
#include <orcaobj/bros1.h>
#include <orcaifaceutil/camera.h>

#include "propertyutils.h"

namespace orcaimage
{

    /**
     * Read in the orca::ImageDescription properties from a config file using the prefix given
     * 
     * @param orca context
     * @param config prefix
     * @param image description structure
     */
    void 
    getImageProperties( const orcaice::Context& context
        , const std::string& prefix
        , orca::ImageDescriptionPtr description
        )
    {
        Ice::PropertiesPtr prop = context.properties();

        description->width = (uint32_t)orcaice::getPropertyAsIntWithDefault( prop, prefix + "ImageWidth", 320);
        description->height = (uint32_t)orcaice::getPropertyAsIntWithDefault( prop, prefix + "ImageHeight", 240);
        description->format = orcaice::getPropertyWithDefault( prop, prefix + "ImageFormat", "BGR8" );
        description->size = (uint32_t)orcaice::getPropertyAsIntWithDefault( prop, prefix + "ImageSize", description->width*description->height*3);
    }

    /**
     * Read in the orca::CameraDescription properties from a config file using the prefix given
     * 
     * @param orca context
     * @param config prefix
     * @param image description structure
     */
    void
    getCameraProperties( const orcaice::Context& context
        , const std::string& prefix
        , orca::CameraDescriptionPtr description
        )
    {
        getImageProperties( context, prefix, description );

        Ice::PropertiesPtr prop = context.properties();

        // offset from the robot coordinate system
        ifaceutil::zeroAndClear( description->offset );
        description->offset = orcaobj::getPropertyAsFrame3dWithDefault( prop, prefix + "Offset", description->offset );

        // read size
        ifaceutil::zeroAndClear( description->caseSize );
        description->caseSize = orcaobj::getPropertyAsSize3dWithDefault( prop, prefix + "CaseSize", description->caseSize );

        // frame rate
        description->frameRate = orcaice::getPropertyAsDoubleWithDefault( prop, prefix + "FrameRate", 0.0 );

        // Focal Length
        std::vector<double> defaultFocalLength;
        defaultFocalLength.push_back(0.0);
        defaultFocalLength.push_back(0.0);
        defaultFocalLength = orcaice::getPropertyAsDoubleVectorWithDefault( prop, prefix+"FocalLength", defaultFocalLength );
        description->focalLength.x =defaultFocalLength[0];
        description->focalLength.y =defaultFocalLength[1];

        // Principle Point
        std::vector<double> defaultPrinciplePoint;
        defaultPrinciplePoint.push_back(0.0);
        defaultPrinciplePoint.push_back(0.0);
        defaultPrinciplePoint = orcaice::getPropertyAsDoubleVectorWithDefault( prop, prefix+"PrinciplePoint", defaultPrinciplePoint );
        description->principlePoint.x =defaultPrinciplePoint[0];
        description->principlePoint.y =defaultPrinciplePoint[1];


        // Distortion Parameters
        std::vector<double> defaultDistortionParameters;
        defaultDistortionParameters.push_back(0.0);
        defaultDistortionParameters.push_back(0.0);
        defaultDistortionParameters.push_back(0.0);
        defaultDistortionParameters.push_back(0.0);
        defaultDistortionParameters = orcaice::getPropertyAsDoubleVectorWithDefault( prop, prefix+"DistortionParameters", defaultDistortionParameters );
        description->k1 =defaultDistortionParameters[0];
        description->k2 =defaultDistortionParameters[1];
        description->p1 =defaultDistortionParameters[0];
        description->p2 =defaultDistortionParameters[1];
    }

    /**
     * Read in the orca::MultiCameraDescription properties from a config file using the prefix given
     * 
     * @param orca context
     * @param config prefix
     * @param image description structure
     */
    void
    getMultiCameraProperties( const orcaice::Context& context
        , const std::string& prefix
        , orca::MultiCameraDescriptionPtr description
        )
    {

        Ice::PropertiesPtr prop = context.properties();
        
        unsigned int numberOfCameras = (unsigned int)orcaice::getPropertyAsIntWithDefault( prop, prefix + "NumberOfCameras", 1);

        for( unsigned int i = 0; i < numberOfCameras; ++i )
        {
            std::stringstream prefixSS;
            prefixSS << prefix <<  i << ".";
            description->descriptions.push_back(new orca::CameraDescription());
            getCameraProperties( context, prefixSS.str(), description->descriptions[i] );
        }

        std::stringstream debugSS;
        debugSS << "descriptions size: " << description->descriptions.size() << std::endl;
        std::string debug = debugSS.str();
        context.tracer().debug( debug );
    }

    /**
     * @brief copy orca::ImageDescriptionPtr to hydroimage::Config
     * Copies all values from the ImageDescription structure to 
     * the hydroimage::Config structure
     *
     * @param hydro image config structure
     * @param orca image description structure
     */
    void 
    copy( hydroimage::Config& config, const orca::ImageDescriptionPtr description )
    {
        config.width = description->width;
        config.height = description->height;
        config.size = description->size;
        config.format = description->format;
    }

    /**
     * @brief copy hydroimage::Config to orca::ImageDescriptionPtr
     * Copies all values from hydroimage::Config the structure to 
     * the orca::ImageDescription structure
     *
     * @param orca image description structure
     * @param hydro image config structure
     */
    void
    copy( orca::ImageDescriptionPtr description, const hydroimage::Config& config)
    {
        description->width = config.width;
        description->height = config.height;
        description->size = config.size;
        description->format = config.format;
    }

}
