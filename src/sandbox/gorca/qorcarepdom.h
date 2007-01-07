/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef GORCA_REPOSITORY_DOM_H
#define GORCA_REPOSITORY_DOM_H

#include <qdom.h>

class QorcaRepositoryDom : public QDomDocument
{
public:
    QorcaRepositoryDom( const QString fileName );
    ~QorcaRepositoryDom();

private:
    void parseHeader( const QDomElement &header );
    void parseRepository( const QDomElement &parentElement );
    void parseComponent( const QDomElement &parentElement );
    void parsePort( const QDomElement &parentElement );
};

#endif // QORCA_REPOSITORY_DOM_H
