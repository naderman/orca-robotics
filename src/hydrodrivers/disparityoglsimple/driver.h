/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_DISPARITY_OGL_SIMPLE_H
#define HYDRO_DISPARITY_OGL_SIMPLE_H

#include <vector>

#include <hydroglu/glprocess.h>
#include <hydrostereocamera/gldifference.h>
#include <hydrointerfaces/disparity.h>

#include <QApplication>



namespace disparitysimple {

//
// An extremely simple, error prone, but fast with the right video card, stereo correspondance driver.
//
class Driver : public hydrointerfaces::Disparity
{

public:

    Driver( const Config &config, const hydroutil::Context &context );
    virtual ~Driver();
    virtual void process( const Data &leftImage, const Data &rightImage, Data &outputImage );
    virtual bool hasEventLoop();
    virtual int executeEventLoop();

private:
    QApplication app_;
    QGLWidget* window_;

    std::vector<hydroglu::GLProcessPtr> processVector_;
    
    Config config_;
    hydroutil::Context context_;
    unsigned int numOfShifts_;
};

// Used for dynamically loading driver
class Factory : public hydrointerfaces::DisparityFactory
{
public:
    hydrointerfaces::Disparity *createDriver( const hydrointerfaces::Disparity::Config &config
					                          , const hydroutil::Context &context
					                        ) const
    {
        return new Driver( config, context );
    }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    hydrointerfaces::DisparityFactory *createDriverFactory();
}

#endif
