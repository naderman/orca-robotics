#ifndef OBJECTMODEL_H
#define OBJECTMODEL_H

#if HAVE_LIB3DS
#include <lib3ds/file.h>
#include <lib3ds/node.h>                        
#include <lib3ds/mesh.h>
#include <lib3ds/material.h>
#include <lib3ds/camera.h>
#include <lib3ds/light.h>
#include <lib3ds/matrix.h>
#include <lib3ds/vector.h>
#endif

namespace orcaqgui3d {

//!
//! @brief A 3-D model of an object, loaded from a .3ds model
//!
//! @author Alex Brooks
//!
class ObjectModel
{

public: 

    ObjectModel( const char *filename );
    ~ObjectModel();

    //! Draw the thing.
    void render();

private: 

#if HAVE_LIB3DS
    void renderNode(Lib3dsNode *node);
    Lib3dsFile *file_;
#endif

};

}

#endif
