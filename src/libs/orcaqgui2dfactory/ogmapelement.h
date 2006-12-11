#ifndef OGMAPELEMENT_H
#define OGMAPELEMENT_H

#include <orcaqgui2dfactory/ogmappainter.h>
#include <orcaqgui2d/icestormelement.h>
#include <orcaqgui/ihumanmanager.h>
#include <orcaqgui2dfactory/connectutils.h>

namespace orcaqgui {

class OgMapElement
    : public IceStormElement<OgMapPainter,
                                             orca::OgMapData,
                                             orca::OgMapDataPtr,
                                             orca::OgMapPrx,
                                             orca::OgMapConsumer,
                                             orca::OgMapConsumerPrx>
{

public:
    OgMapElement( const orcaice::Context  &context,
                  const std::string       &proxyString,
                  IHumanManager* humanManager );

    virtual bool isInGlobalCS() { return true; }
    virtual void actionOnConnection() 
    { 
        std::cout << "Paint initial data begin" << std::endl;
        paintInitialData<orca::OgMapPrx, OgMapPainter>
            ( context_, listener_.interfaceName(), painter_ );
            std::cout << "Paint initial data finished" << std::endl;
    }
    virtual QStringList contextMenu();
    virtual void execute( int action );

public:
    void saveMapAs();
    void saveMap();

private:
    OgMapPainter painter_;
    IHumanManager *humanManager_;
    QString mapFileName_;
    bool mapFileNameSet_;
};

}

#endif
