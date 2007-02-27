/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include "platformcolor.h"

using namespace std;

namespace orcaqgui 
{
    
PlatformColor::PlatformColor() 
    : colorCounter_(0)
{
    double timeInSec = orcaice::timeAsDouble( orcaice::getNow() );
    seed_ = (unsigned int)floor(timeInSec);
    
    colorVector_.push_back(Qt::black);  // this one gets assigned to "global", no display
    colorVector_.push_back(Qt::red);
    colorVector_.push_back(Qt::blue);
    colorVector_.push_back(Qt::cyan);
    colorVector_.push_back(Qt::magenta);
    colorVector_.push_back(Qt::darkYellow);    
    colorVector_.push_back(Qt::darkRed);
    colorVector_.push_back(Qt::darkBlue);
    colorVector_.push_back(Qt::darkGreen);
    colorVector_.push_back(Qt::darkCyan);
    colorVector_.push_back(Qt::darkMagenta);
    
}

void 
PlatformColor::setNewPlatform( QString &platform )
{
    QColor platformColor;
    // assign a new colour
    if ( colorCounter_>=colorVector_.size() ) {
        platformColor = generateRandomColor();
    } else {
        platformColor = colorVector_[colorCounter_];
        colorCounter_++;
    }
    // save for the future
    colorMap_[platform] = platformColor;
    //cout<<"TRACE(guielementmodel.cpp): emit newPlatform signal" << endl;
}    

bool 
PlatformColor::getColor( const QString &platform, QColor &color )
{
    if (colorMap_[platform]==QColor()) return false;
    
    color = colorMap_[platform];
    return true;
}

QColor
PlatformColor::generateRandomColor()
{    
    seed_ = seed_ + 10;
    srand(seed_);
    
    QColor colour;
    int r = (int)floor( (double)rand()/((double)(RAND_MAX)+1.0)*256 );
    int g = (int)floor( (double)rand()/((double)(RAND_MAX)+1.0)*256 );
    int b = (int)floor( (double)rand()/((double)(RAND_MAX)+1.0)*256 );       
    colour.setRgb(r,g,b);
    //cout << "random color is: " << r << " " << g << " " << b << endl;
    return colour;    
}

}

