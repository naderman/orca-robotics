#ifndef FEATUREMAP2DELEMENT_H
#define FEATUREMAP2DELEMENT_H

#include <orcaqgui3d/icestormelement3d.h>
#include <hydroqgui/hydroqgui.h>
#include <orcaqgui3dfactory/featuremap2dpainter.h>
#include <orcaqgui2dfactory/connectutils.h>

namespace orcaqgui3d
{

//!
//! @author Alex Brooks
//!
class FeatureMap2dElement
    : public orcaqgui3d::IceStormElement3d<FeatureMap2dPainter,
                             orca::FeatureMap2dData,
                             orca::FeatureMap2dPrx,
                             orca::FeatureMap2dConsumer,
                             orca::FeatureMap2dConsumerPrx>
{

public: 

    FeatureMap2dElement( const orcaice::Context  &context,
                         const std::string       &proxyString,
                         hydroqguielementutil::IHumanManager *humanManager );

    virtual bool isInGlobalCS() { return true; }
    virtual void actionOnConnection() 
    {
        orcaqgui2d::paintInitialData<orca::FeatureMap2dPrx, FeatureMap2dPainter>
            ( context_, listener_.interfaceName(), painter_ );
    }
    virtual QStringList contextMenu();
    virtual void execute( int action );

    osg::Node *osgNode() const { return painter_.osgNode(); }

public:
    void saveFeatureMapAs();
    void saveFeatureMap();

private:
    FeatureMap2dPainter painter_;
    hydroqguielementutil::IHumanManager *humanManager_;
    QString featureMapFileName_;
    bool featureMapFileNameSet_;
};

}

#endif
