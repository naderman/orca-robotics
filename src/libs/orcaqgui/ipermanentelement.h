/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAQGUI_IPERMANENT_ELEMENT_H
#define ORCA2_ORCAQGUI_IPERMANENT_ELEMENT_H

// #include <string>
// #include <assert.h>
// #include <QString>
// #include <QColor>
// #include <QPointF>
// #include <QMouseEvent>

namespace orcaqgui
{
       
/*!
 *
 * @brief All the permanent gui elements should derive from this, e.g. grid or background maps etc.
 *
 * @author Tobias Kaupp
*/
class IPermanentElement
{                                 
public:
    
    virtual ~IPermanentElement() {};
};

} // namespace

#endif
