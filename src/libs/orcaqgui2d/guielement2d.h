#ifndef GUIELEMENT2D_H
#define GUIELEMENT2D_H

#include <QPainter>
#include <orcaqgui/guielement.h>

namespace orcaqgui {

//!
//! @author Alex Brooks
//!
class GuiElement2d : public GuiElement
{

public: 

    virtual ~GuiElement2d() {}

    //! Paint is called periodically to paint the internal state of the guielement
    virtual void paint( QPainter *p, int z ) {};

    //! Would anything be painted to this layer?
    virtual bool paintThisLayer( int z ) const=0;
    
    //! reports the elements position in the world
    virtual QPointF pos() const { return QPointF(-9999.9999, -9999.9999); };
    
private: 


};

}

#endif
