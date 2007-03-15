/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ISENSORMODEL_H
#define ISENSORMODEL_H

namespace localnav {

//!
//! @author Alex Brooks
//!
class ISensorModel
{

public: 

    virtual ~ISensorModel();

    virtual void subscribeForInfo()=0;

    // Allocates memory for ISensorData.
    // Returns NULL if there's a problem.
    virtual ISensorData *getNext( int timeoutMs )=0;

private: 


};

}

#endif
