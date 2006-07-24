/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef QORCA_XML_EDIT_H
#define QORCA_XML_EDIT_H

#include <q3listview.h>
#include <qdom.h>

class QorcaXmlEdit : public Q3ListView
{
    Q_OBJECT

public:
    QorcaXmlEdit( QDomElement& element,
        QWidget *parent = 0, const char *name = 0, Qt::WFlags f=Qt::WDestructiveClose );
    ~QorcaXmlEdit();

private:
    QDomElement* xml_;

    void xmlToTree();
    void treeToXml();
};

#endif
