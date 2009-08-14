#ifndef HYDRO_GUIELEMENT2D_H
#define HYDRO_GUIELEMENT2D_H

#include <hydroqguielementutil/guielement.h>

class QPainter;

namespace hydroqguielementutil 
{

//!
//! @author Alex Brooks
//!
class GuiElement2d : public GuiElement 
{

public: 

    GuiElement2d( const GuiElementInfo &guiElementInfo )
        : GuiElement(guiElementInfo)
        {}

    virtual ~GuiElement2d() {};

    //! Paint is called periodically to paint the internal state of the guielement
    virtual void paint( QPainter *p, int z ) {};

    //! Experimental!
    //! Paint part of the canvas inside @c rect.
    virtual void paint( QPainter *p, int z, const QRectF& rect ) {};

    //! Would anything be painted to this layer?
    virtual bool paintThisLayer( int z ) const=0;
    
    //! reports the elements position in the world
    virtual QPointF pos() const { return QPointF(-9999.9999, -9999.9999); };
    
private: 


};

}

#endif
