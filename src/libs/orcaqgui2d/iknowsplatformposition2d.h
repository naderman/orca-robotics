#ifndef IKNOWSPLATFORMPOSITION2D_H
#define IKNOWSPLATFORMPOSITION2D_H


namespace orcaqgui2d {

//!
//! @brief GUIs need to paint some things centred on a platform's position.
//!        Inheriting from this interface means that the GUI element knows
//!        where the platform is located.
//!
//! @author Alex Brooks
//!
class IKnowsPlatformPosition2d
{

public: 

    virtual ~IKnowsPlatformPosition2d() {};

    // Returns a number from 1 to 10 indicating the extent to which this
    // GUI element should be trusted.
    // This is used in the case that multiple GUI elements on a single platform 
    // all purport to know the position.
    virtual int platformKnowledgeReliability() const=0;

    // coordinates in GUI CS
    virtual double x() const=0;
    virtual double y() const=0;
    virtual double theta() const=0;

private: 


};

}

#endif
