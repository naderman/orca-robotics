/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <QMessageBox> 

#include "utils.h"
#include "orcaicons.h"

namespace orcaqt
{

void aboutOrca( QWidget* parent, const QString & title )
{
    QMessageBox mb(parent);

    QString c = title;
    if (c.isEmpty())
        c = "About Orca";
    mb.setWindowTitle(c);

    QString text;
    text += "Orca is an open-source framework for developing component-based robotic systems. It provides the means for defining and developing the building blocks which can be pieced together to form arbitrarily complex robotic systems, from single vehicles to distributed sensor networks.\n\n";
    text += "For more information visit\n";
    text += "    http://orca-robotics.sf.net\n";
    mb.setText( text );

    QPixmap pm( orca2_2x3_yellow_130_xpm );
    mb.setIconPixmap( pm );

    mb.exec();
}

} // namespace
