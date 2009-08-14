/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <hydroqguielementutil/iknowsplatformposition2d.h>
#include "platformcsfinder.h"

using namespace std;

namespace hydroqgui {

bool 
PlatformCSFinder::findPlatformCS( const QList< ::hydroqguielementutil::GuiElement* > &elements,
                                  const QString                                       &platform,
                                  float                                               &x,
                                  float                                               &y,
                                  float                                               &theta ) const
{
    //
    // Find the most reliable element that reckons it knows where we are.
    //
    int mostReliable = -1;
    hydroqguielementutil::IKnowsPlatformPosition2d *mostReliablePosElement = NULL;

    for ( int i=0; i < elements.size(); ++i )
    {
        //cout<<endl<<elements_[i]->id.toStdString()<<" on "<<elements_[i]->platformatrix.toStdString();

        hydroqguielementutil::IKnowsPlatformPosition2d *posElem = 
                dynamic_cast<hydroqguielementutil::IKnowsPlatformPosition2d*>(elements[i]);
        if ( posElem != NULL )
        {
            // Ignore if it's the wrong platform.
            if ( elements[i]->platform() != platform ) continue;

            if ( posElem->platformKnowledgeReliability() > mostReliable )
            {
                mostReliablePosElement = posElem;
                mostReliable = posElem->platformKnowledgeReliability();
            }
        }
    }

    if ( mostReliablePosElement == NULL )
    {
        // No-one knows where we are.
        return false;
    }

    x     = mostReliablePosElement->x();
    y     = mostReliablePosElement->y();
    theta = mostReliablePosElement->theta();

//     cout<<"TRACE(platformcsfinder.cpp): x,y,theta: " << x<<", "<<y<<", "<<theta*180.0/3.14159265 << endl;

    return true;
}


}
