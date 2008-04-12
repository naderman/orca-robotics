#ifndef FEATUREMAP2DELEMENT_H
#define FEATUREMAP2DELEMENT_H

#include <orcaqguielementutil/icestormelement.h>
#include <hydroqgui/hydroqgui.h>
#include <orcaqgui2dfactory/featuremap2dpainter.h>
#include <orcaqgui2dfactory/connectutils.h>

namespace orcaqgui2d
{

//!
//! @author Alex Brooks
//!
class FeatureMap2dElement
    : public orcaqguielementutil::IceStormElement<FeatureMap2dPainter,
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
        paintInitialData<orca::FeatureMap2dPrx, FeatureMap2dPainter>
            ( context_, listener_.interfaceName(), painter_ );
    }
    virtual QStringList contextMenu();
    virtual void execute( int action );
    virtual void setUseTransparency( bool useTransparency ) { painter_.setUseTransparency( useTransparency ); };

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
