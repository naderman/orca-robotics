/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <QMessageBox> 
#include <QPixmap> 

#include "utils.h"
#include "orcaicons.h"

namespace orcaqt
{

void 
aboutOrca( QWidget* parent, const QString & title )
{
    QString c = title;
    if (c.isEmpty())
        c = "About Orca";

    QString text;
    text += "Orca is an open-source framework for developing component-based robotic systems. It provides the means for defining and developing the building blocks which can be pieced together to form arbitrarily complex robotic systems, from single vehicles to distributed sensor networks.\n";

    aboutApp( parent, c, text );
}

void 
aboutApp( QWidget* parent, const QString & title, const QString & text )
{
    QMessageBox mb(parent);

    QString c = title;
    if (c.isEmpty())
        c = "About Orca Application";
    mb.setWindowTitle(c);

    QString t = text;
    t += "\nFor more information visit\n";
    t += "    http://orca-robotics.sf.net\n";
    mb.setText( t );

    QPixmap pm( orca2_2x3_yellow_130_xpm );
    mb.setIconPixmap( pm );

    mb.exec();
}

} // namespace
