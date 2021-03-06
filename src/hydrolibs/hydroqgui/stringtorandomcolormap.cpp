/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "stringtorandomcolormap.h"
#include <cstdlib>
#include <cmath>

using namespace std;

namespace hydroqgui 
{
    
StringToRandomColorMap::StringToRandomColorMap() 
    : colorCounter_(0)
{
//    double timeInSec = orcaice::timeAsDouble( orcaice::getNow() );
//    seed_ = (unsigned int)floor(timeInSec);
    
    colorVector_.push_back(Qt::black);
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

QColor
StringToRandomColorMap::getColor( const QString &str )
{
    QColor color;

    std::map<QString,QColor>::iterator it = colorMap_.find( str );

    if ( it == colorMap_.end() )
    {
        // Allocate a new color
        if ( colorCounter_>=colorVector_.size() ) {
            color = generateRandomColor();
        } else {
            color = colorVector_[colorCounter_];
            colorCounter_++;
        }
        // save for the future
        colorMap_[str] = color;
    }
    else
    {
        // find the old color
        color = it->second;
    }

    return color;
}

QColor
StringToRandomColorMap::generateRandomColor()
{    
//    seed_ = seed_ + 10;
//    srand(seed_);
    
    QColor colour;
    int r = (int)floor( (double)rand()/((double)(RAND_MAX)+1.0)*256 );
    int g = (int)floor( (double)rand()/((double)(RAND_MAX)+1.0)*256 );
    int b = (int)floor( (double)rand()/((double)(RAND_MAX)+1.0)*256 );       
    colour.setRgb(r,g,b);
    //cout << "random color is: " << r << " " << g << " " << b << endl;
    return colour;    
}

}

