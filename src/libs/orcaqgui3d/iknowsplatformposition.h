/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI3D_IKNOWSPLATFORMPOSITION_H
#define ORCAGUI3D_IKNOWSPLATFORMPOSITION_H

namespace orcaqgui3d {

//!
//! @author Alex Brooks
//!
class IKnowsPlatformPosition
{

public: 

    virtual ~IKnowsPlatformPosition() {}

    // Returns a number from 1 to 10 indicating the extent to which this
    // GUI element should be trusted.
    // This is used in the case that multiple GUI elements on a single platform 
    // all purport to know the position.
    virtual int platformKnowledgeReliability() const=0;

    virtual float x() const=0;
    virtual float y() const=0;
    virtual float z() const=0;
    virtual float roll()  const=0;
    virtual float pitch() const=0;
    virtual float yaw()   const=0;

private: 


};

}

#endif
