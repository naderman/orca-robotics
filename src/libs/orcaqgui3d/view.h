#ifndef ORCAQGUI3D_VIEW_H
#define ORCAQGUI3D_VIEW_H


namespace orcaqgui3d {

//!
//! @author Alex Brooks
//!
class View
{

public: 

    virtual ~View() {}

    // All in S.I. units and Orca-style coordinate system
    virtual double cameraX() const=0;
    virtual double cameraX() const=0;
    virtual double cameraZ() const=0;
    virtual double cameraRoll() const=0;
    virtual double cameraPitch() const=0;
    virtual double cameraYaw() const=0;

    virtual bool isAntialiasingEnabled() const=0

private: 

};

}

#endif
