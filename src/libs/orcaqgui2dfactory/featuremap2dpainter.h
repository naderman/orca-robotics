/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef QORC_FEATUREMAP2D_PAINTER_H
#define QORC_FEATUREMAP2D_PAINTER_H


#include <qcolor.h>
#include <orca/featuremap2d.h>
#include <orcaqgui2d/definitions2d.h>

// forward declarations
class QPainter;

namespace orcaqgui {
    
class IHumanManager;
    
class FeatureMap2dPainter
{
  public:
    FeatureMap2dPainter();
    ~FeatureMap2dPainter();

    void setData( const orca::FeatureMap2dDataPtr &featureDataPtr );
    void paint( QPainter *p, int z );
    bool paintThisLayer(int z) const {return z==Z_SLAM_MAP;}

    void clear();
    
    void toggleFeatureNumbers() { displayFeatureNumbers_ = !displayFeatureNumbers_; }
    void toggleUncertainty() { displayUncertainty_ = !displayUncertainty_; }
    
    int saveMap( const QString fileName, IHumanManager *humanManager ) const;

  private:
    orca::FeatureMap2dDataPtr data_;
    bool displayFeatureNumbers_;
    bool displayUncertainty_;
};

}

#endif
